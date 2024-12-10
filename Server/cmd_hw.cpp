#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lzw_hls.h"
#include "lzw_hw.h"
#include "cmd_hw.h"
#include "sha3.h"
#include "Utilities.h"

#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include <stdio.h>
#include <stdbool.h>

const int MAX_INPUT_SIZE = 4096;
const int MAX_OUTPUT_SIZE = 4096;

HashEntry hash_table_entries[HASH_TABLE_SIZE];
HashEntry *hash_table_array[HASH_TABLE_SIZE];

uint64_t compute_hash(const char *chunk, int size) {
 
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

void handle_16bit_boundary(const std::string& outputFileName) {
    // Open the file for both reading and writing
    FILE* file = fopen(outputFileName.c_str(), "r+b");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    if (file_size <= 0) {
        std::cerr << "File is empty or corrupted." << std::endl;
        fclose(file);
        return;
    }

    // Check if the file is at a 16-bit boundary
    bool is_at_16bit_boundary = (file_size % 2 == 0);

    if (!is_at_16bit_boundary) {
        // Add a 0x00 byte to the file to align to a 16-bit boundary
        uint8_t padding = 0x00;
        fwrite(&padding, sizeof(uint8_t), 1, file);

        // Recalculate the file size after padding
        file_size += 1;

        // Swap the last two bytes in the file
        fseek(file, -2, SEEK_END); // Move to the second last byte
        uint8_t second_last_byte, last_byte;
        fread(&second_last_byte, sizeof(uint8_t), 1, file); // Read the second last byte
        fread(&last_byte, sizeof(uint8_t), 1, file);        // Read the last byte

        // Write them in swapped order
        fseek(file, -2, SEEK_END); // Move back to overwrite the last two bytes
        fwrite(&last_byte, sizeof(uint8_t), 1, file);
        fwrite(&second_last_byte, sizeof(uint8_t), 1, file);
    }

    fclose(file); // Close the file
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

void pack_codes_msb_first(const uint16_t* codes, size_t num_codes, uint8_t* packed_data, size_t& packed_data_size, size_t max_packed_data_size, FILE* file, uint32_t header, std::string outputFileName) {
    uint32_t bit_buffer = 0;   // Buffer to store bits before writing
    int bits_in_buffer = 0;    // Number of bits currently in the buffer
    int code_length = std::ceil(std::log2(MAX_CHUNK_SIZE));
    packed_data_size = 0;      // Initialize packed data size

    // Break the header into bytes and add to the array
    if (packed_data_size + 4 > max_packed_data_size) {
        std::cerr << "Error: packed_data array is full." << std::endl;
        return;
    }

    packed_data[packed_data_size++] = (header >> 24) & 0xFF;
    packed_data[packed_data_size++] = (header >> 16) & 0xFF;
    packed_data[packed_data_size++] = (header >> 8) & 0xFF;
    packed_data[packed_data_size++] = header & 0xFF;

    std::cout << "Reformatting Endianness Starting" << std::endl;
    int counter = 0;
    int i = -1;
    while (counter < num_codes && i < counter * 2 + 10) {
        i++;
        if (codes[i] == 0) continue;
        counter++;

        // Add the current code_length code to the buffer
        bit_buffer = (bit_buffer << code_length) | (codes[i] & ((1 << code_length) - 1));
        bits_in_buffer += code_length;

        // Write out full bytes from the buffer
        while (bits_in_buffer >= 8) {
            if (packed_data_size >= max_packed_data_size) {
                std::cerr << "Error: packed_data array is full." << std::endl;
                return;
            }

            uint8_t byte_to_write = (bit_buffer >> (bits_in_buffer - 8)) & 0xFF;
            packed_data[packed_data_size++] = byte_to_write;
            bits_in_buffer -= 8;
        }
    }

    // Write any remaining bits from the buffer, padded to align to an 8-bit boundary
    if (bits_in_buffer > 0) {
        if (packed_data_size >= max_packed_data_size) {
            std::cerr << "Error: packed_data array is full." << std::endl;
            return;
        }

        uint8_t byte_to_write = (bit_buffer << (8 - bits_in_buffer)) & 0xFF;
        packed_data[packed_data_size++] = byte_to_write;
    }

    for (size_t i = 0; i + 1 < packed_data_size; i += 2) {
        std::swap(packed_data[i], packed_data[i + 1]);
    }

    // Step 3: Write the Packed Compressed Data
    size_t written_data = fwrite(packed_data, sizeof(uint8_t), packed_data_size, file);
    if (written_data != packed_data_size) {
        std::cerr << "Error: Failed to write packed LZW compressed data to file." << std::endl;
        fclose(file);
        return;
    }

    fclose(file);
}

void clean_chunk(const char* chunk, int chunk_size, char* clean_data, int& clean_size) {
    clean_size = 0;

    // Clean and filter only valid payload data
    for (int i = 0; i < chunk_size; ++i) {
        // Skip any unexpected values like flags or padding
        if (0 < chunk[i] && chunk[i] < 128) {
            clean_data[clean_size++] = static_cast<char>(chunk[i]);
        }
    }

    // Null-terminate the cleaned data
    clean_data[clean_size] = '\0';
}
int deduplicate_chunks(const char *chunk, int chunk_size, HashTable *hash_table,
                       cl::Kernel &krnl_lzw, cl::CommandQueue &q, cl::Buffer &input_buf,
                       cl::Buffer &output_code_buf, cl::Buffer &output_size_buf,
                       cl::Buffer &output_buf, cl::Buffer &output_length_buf,
                       char *input_hw, int *output_code_hw, int *output_size,
                       char *output_hw, int *output_length, std::string outputFileName)
 {
    if (hash_table == NULL) {
        fprintf(stderr, "Hash table is not initialized\n");
        return -1;
    }

    uint64_t chunk_hash = compute_hash(chunk, chunk_size);

    int existing_size;
    int *existing = lookup_hash_table(hash_table, chunk_hash, &existing_size);

    FILE *outfc = fopen(outputFileName.c_str(), "ab");
    if (!outfc) {
        perror("Failed to open output file for appending compressed data.");
        return 1;
    }

    if (existing != NULL && existing_size == chunk_size &&
        memcmp(existing, chunk, chunk_size) == 0) {
        uint32_t header = (chunk_hash | 0x80000000);
        size_t written_header = fwrite(&header, sizeof(uint32_t), 1, outfc);
        if (written_header != 1) {
            std::cerr << "Error: Failed to write LZW chunk header to file." << std::endl;
            fclose(outfc);
            return 1;
        }
        return 0;
    }     else {
        
        // ----------------------
        // Kernel Hardware Execution
        // ----------------------

        // Clean and then copy the chunk data into the input hardware array that then gets sent to the FPGA
        printf("Copying memory...\n");
        memcpy(input_hw, chunk, chunk_size * sizeof(char));
        // Set the input_size argument
        printf("Cleaned size: %d\n", chunk_size);
        // std::cerr << "Cleaned Size: " << chunk_size << std::endl;
        krnl_lzw.setArg(1, chunk_size);

        // lzw_sw(chunk, chunk_size, output_code, *output_size, output, *output_length);

        // Migrate input buffer to FPGA
        printf("KRNL 2...\n");
        q.enqueueMigrateMemObjects({input_buf}, 0 /* Host to device */, NULL, NULL);
        printf("KRNL 3...\n");
        q.finish();
        cl_int err; 
        // Launch the FPGA kernel
       err = q.enqueueTask(krnl_lzw);
        if (err != CL_SUCCESS) {
            std::cerr << "Error enqueueing kernel task: " << err << std::endl;
            return -1;
        }
        std::cout << "Kernel enqueued successfully" << std::endl;

        err = q.finish();
        if (err != CL_SUCCESS) {
            std::cerr << "Error waiting for kernel completion: " << err << std::endl;
            return -1;
        }
        std::cout << "Kernel execution completed" << std::endl;

                printf("KRNL 4...\n");
        q.finish();

        // Migrate output buffer back to host
        printf("KRNL 5...\n");
        q.enqueueMigrateMemObjects({output_code_buf, output_size_buf, output_buf, output_length_buf}, CL_MIGRATE_MEM_OBJECT_HOST);
        q.finish();

        // Retrieve encoded data
        int encoded_size = *output_size;
        int decoded_length = *output_length;

        // Print encoded data FOR DEBUGGING
        // printf("Encoded Output Codes FPGA: ");
        // for (int j = 0; j < encoded_size; ++j) {
        //     printf("%d ", output_code[j]);
        // }

        // printf("Decoded Output FPGA: ");
        // for (int j = 0; j < decoded_length; ++j) {
        //     printf("%c", output[j]);
        // }

        // Print out decoded output:
        // std::cout << "output_hw: " << output_r << std::endl;

        if (encoded_size == 0) {
            fprintf(stderr, "Failed to encode chunk\n");
            return -1;
        }

        // Step 1: Write the LZW Chunk Header
        // Header: Bit 0 = 0 (LZW chunk), Bits 31–1 = encoded_size (compressed data length)
        uint32_t header = (encoded_size & 0x7FFFFFFF);
        
        // Step 2: Pack the Compressed Data (e.g. 13-bit codes) into an 8-bit-aligned format
        // std::vector<uint8_t> packed_data;
        // pack_codes_msb_first(reinterpret_cast<const uint16_t*>(output_code), encoded_size, packed_data, outfc, header, outputFileName);

        size_t max_packed_data_size = encoded_size * 2;
        uint8_t packed_data[max_packed_data_size];
        size_t packed_data_size = 0;
        pack_codes_msb_first(reinterpret_cast<const uint16_t*>(output_code_hw), encoded_size, packed_data, packed_data_size, max_packed_data_size, outfc, header, outputFileName);

        // Make sure the file always ends at a 16bit boundery
        handle_16bit_boundary(outputFileName);

        // Ensure chunk_hash is within bounds
        if (chunk_hash >= HASH_TABLE_SIZE) {
            fprintf(stderr, "Error: chunk_hash (%lu) out of bounds\n", chunk_hash);
            return -1;
        }

        static int encoding_storage[HASH_TABLE_SIZE][MAX_CHUNK_STORAGE];

        // Check encoded_size bounds
        if (encoded_size > MAX_CHUNK_STORAGE) {
            fprintf(stderr, "Error: encoded_size (%d) exceeds MAX_CHUNK_STORAGE (%d)\n", encoded_size, MAX_CHUNK_STORAGE);
            return -1;
        }

        // Copy encoded data to storage
        // memcpy(encoding_storage[chunk_hash], output, encoded_size * sizeof(int));
        printf("Storing encoded data for hash: %lu\n", chunk_hash);

        // Insert into hash table
        insert_hash_table(hash_table, chunk_hash, encoding_storage[chunk_hash], encoded_size);
        return 1;
    }
}
