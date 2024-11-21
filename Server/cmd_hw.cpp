#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lzw_hls.h"
#include "cmd_hw.h"
#include "sha3.h"
#include "Utilities.h"

const int MAX_INPUT_SIZE = 4096;
const int MAX_OUTPUT_SIZE = 4096;

HashEntry hash_table_entries[HASH_TABLE_SIZE];
HashEntry *hash_table_array[HASH_TABLE_SIZE];

uint64_t compute_hash(const unsigned char *chunk, int size) {
 
    uint8_t hash_output[64]; 
    sha3_ctx_t sha3; 
    sha3_init(&sha3, 64); 
    sha3_update(&sha3, chunk, (size_t) size); 
    sha3_final(hash_output, &sha3); 
    uint64_t res = 0; 
    for(int i = 0; i < 8; i++) {
        res |= ((uint64_t) hash_output[i]) << (8 * i); 
    }
    return res % HASH_TABLE_SIZE;
}

void initialize_hash_table(HashTable *table) {
    table->size = HASH_TABLE_SIZE;
    table->entries = hash_table_array;  
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
}

void reset_hash_table(HashTable *table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
}

void insert_hash_table(HashTable *table, uint64_t key, int *value, int size) {
    int index = (int)(key % table->size);
    HashEntry *new_entry = &hash_table_entries[index];

    new_entry->key = key;
    new_entry->value = value;
    new_entry->size = size;
    new_entry->next = table->entries[index];
    table->entries[index] = new_entry;
}

int *lookup_hash_table(HashTable *table, uint64_t key, int *size) {
    int index = (int)(key % table->size);
    HashEntry *entry = table->entries[index];

    while (entry != NULL) {
        if (entry->key == key) {
            *size = entry->size;
            return entry->value;
        }
        entry = entry->next;
    }
    *size = 0;
    return NULL;
}

int deduplicate_chunks(const unsigned char *chunk, int chunk_size, HashTable *hash_table,
                       cl::Kernel &krnl_lzw, cl::CommandQueue &q,
                       cl::Buffer &input_buf, cl::Buffer &output_buf, cl::Buffer &output_size_buf,
                       char *input, int *output_hw, int *output_size_hw, std::string outputFileName) {
    if (hash_table == NULL) {
        fprintf(stderr, "Hash table is not initialized\n");
        return -1;
    }

    printf("Computing hash for chunk of size %d\n", chunk_size);
    uint64_t chunk_hash = compute_hash(chunk, chunk_size);
    printf("Computed hash: %lu\n", chunk_hash);

    int existing_size;
    int *existing = lookup_hash_table(hash_table, chunk_hash, &existing_size);
    printf("Lookup result: %p, existing size: %d\n", (void *)existing, existing_size);

    if (existing != NULL && existing_size == chunk_size &&
        memcmp(existing, chunk, chunk_size) == 0) {
        printf("Chunk is a duplicate\n");
        return 0;
    } else {
        // ----------------------
        // Kernel Hardware Execution
        // ----------------------
        printf("Inside the Kernel Hardware Execution...\n");

        // AHMED TO DO <--- Make sure the test input loads through here
        // Load the current chunk into the input buffer
        const char *test_input = "I AM SAM SAM I AM";

        strncpy(input, (const char *)test_input, MAX_INPUT_SIZE);

        // Migrate input buffer to FPGA
        q.enqueueMigrateMemObjects({input_buf}, 0 /* Host to device */, NULL, NULL);
        q.finish();

        // Launch the FPGA kernel
        q.enqueueTask(krnl_lzw, NULL, NULL);
        q.finish();

        // Migrate output buffer back to host
        q.enqueueMigrateMemObjects({output_buf, output_size_buf}, CL_MIGRATE_MEM_OBJECT_HOST);
        q.finish();

        // Retrieve encoded data
        int encoded_size = *output_size_hw;
        printf("Encoded Size: %d\n", encoded_size);

        // Print encoded data for debugging
        printf("Encoded Chunk FPGA: ");
        for (int j = 0; j < encoded_size; ++j) {
            printf("%d ", output_hw[j]);
        }
        printf("\n");

        if (encoded_size == 0) {
            fprintf(stderr, "Failed to encode chunk\n");
            return -1;
        }

        // Output File
        FILE *outfc = fopen(outputFileName.c_str(), "ab");
        if (!outfc) {
            perror("Failed to open output file for appending compressed data.");
            return 1; // Exit with an error code
        }

        size_t written_data = fwrite(output_hw, sizeof(int), encoded_size, outfc);
        if (written_data != (size_t)encoded_size) {
            std::cerr << "Error: Failed to write encoded data to file." << std::endl;
        }
        fclose(outfc);

        // Ensure chunk_hash is within bounds
        if (chunk_hash >= HASH_TABLE_SIZE) {
            fprintf(stderr, "Error: chunk_hash (%lu) out of bounds\n", chunk_hash);
            return -1;
        }

        static int encoding_storage[HASH_TABLE_SIZE][INPUT_SIZE];

        // Check encoded_size bounds
        if (encoded_size > INPUT_SIZE) {
            fprintf(stderr, "Error: encoded_size (%d) exceeds INPUT_SIZE (%d)\n", encoded_size, INPUT_SIZE);
            return -1;
        }

        // Copy encoded data to storage
        memcpy(encoding_storage[chunk_hash], output_hw, encoded_size * sizeof(int));
        printf("Storing encoded data for hash: %lu\n", chunk_hash);

        // Insert into hash table
        insert_hash_table(hash_table, chunk_hash, encoding_storage[chunk_hash], encoded_size);
        printf("Chunk is unique\n");

        return 1;
    }
}
