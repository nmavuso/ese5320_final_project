#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lzw_hw.h"
#include "cmd_hw.h"
#include "sha3.h"

// Define constants
#define HASH_TABLE_SIZE 1024
#define INPUT_SIZE 8192

// Hash table and storage definitions
HashEntry hash_table_entries[HASH_TABLE_SIZE];
HashEntry *hash_table_array[HASH_TABLE_SIZE];
static int encoding_storage[HASH_TABLE_SIZE][INPUT_SIZE]; // Fixed storage indexed by hash table entries

// Function to compute hash for a chunk
uint64_t compute_hash(const unsigned char *chunk, int size) {
    if (!chunk || size <= 0) {
        fprintf(stderr, "Invalid chunk for hashing\n");
        return 0; // Return 0 for invalid input
    }

    uint8_t hash_output[64]; // SHA3-512 outputs 64 bytes
    sha3_ctx_t sha3;

    sha3_init(&sha3, 64);               // Initialize SHA3 context for 64 bytes
    sha3_update(&sha3, chunk, size);   // Update hash with chunk data
    sha3_final(hash_output, &sha3);    // Finalize hash

    uint64_t res = 0;
    for (int i = 0; i < 8; i++) {
        res |= ((uint64_t)hash_output[i]) << (8 * i); // Use first 8 bytes for hash
    }
    return res % HASH_TABLE_SIZE; // Return hash mod table size
}

// Initialize hash table
void initialize_hash_table(HashTable *table) {
    if (!table) {
        fprintf(stderr, "Hash table is null\n");
        return;
    }

    table->size = HASH_TABLE_SIZE;
    table->entries = hash_table_array;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
}

// Reset hash table
void reset_hash_table(HashTable *table) {
    if (!table) {
        fprintf(stderr, "Hash table is null\n");
        return;
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
}

// Insert entry into hash table
void insert_hash_table(HashTable *table, uint64_t key, int *value, int size) {
    if (!table || !value) {
        fprintf(stderr, "Invalid table or value for insertion\n");
        return;
    }

    int index = (int)(key % table->size);
    HashEntry *new_entry = &hash_table_entries[index];

    new_entry->key = key;
    new_entry->value = value;
    new_entry->size = size;
    new_entry->next = table->entries[index]; // Link new entry to the existing chain
    table->entries[index] = new_entry;
}

// Lookup entry in hash table
int *lookup_hash_table(HashTable *table, uint64_t key, int *size) {
    if (!table || !size) {
        fprintf(stderr, "Invalid table or size pointer for lookup\n");
        return NULL;
    }

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

// Deduplicate chunks using hash table
int deduplicate_chunks(const unsigned char *chunk, int chunk_size, HashTable *hash_table) {
    if (!chunk || chunk_size <= 0 || !hash_table) {
        fprintf(stderr, "Invalid input or uninitialized hash table\n");
        return -1;
    }

    uint64_t chunk_hash = compute_hash(chunk, chunk_size);
    int existing_size = 0;
    int *existing = lookup_hash_table(hash_table, chunk_hash, &existing_size);

    // Check for duplicate chunk
    if (existing != NULL && existing_size == chunk_size &&
        memcmp(existing, chunk, chunk_size) == 0) {
        printf("Chunk is a duplicate\n");
        return 0; // Duplicate found
    }

    // Encode chunk
    int encoded_data[INPUT_SIZE];
    int encoded_size = 0;
    int result = encoding((const char *)chunk, encoded_data, &encoded_size);

    if (result != 0 || encoded_size <= 0) {
        fprintf(stderr, "Failed to encode chunk\n");
        return -1; // Encoding failed
    }

    // Check bounds
    if (encoded_size > INPUT_SIZE) {
        fprintf(stderr, "Encoded data exceeds storage capacity\n");
        return -1; // Prevent buffer overflow
    }

    // Store encoded data into encoding storage
    int index = (int)(chunk_hash % HASH_TABLE_SIZE);
    memcpy(encoding_storage[index], encoded_data, encoded_size * sizeof(int));

    // Insert into hash table
    insert_hash_table(hash_table, chunk_hash, encoding_storage[index], encoded_size);
    printf("Chunk is unique\n");
    return 1; // Unique chunk stored
}
