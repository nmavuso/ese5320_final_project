#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lzw_hw.h"
#include "cmd_hw.h"

#define NUM_CHUNKS 3
#define MAX_CHUNK_SIZE 8192
#define HASH_TABLE_SIZE 1000
#define INPUT_SIZE 256

HashEntry hash_table_entries[HASH_TABLE_SIZE];
HashEntry *hash_table_array[HASH_TABLE_SIZE];

uint64_t compute_hash(const unsigned char *chunk, int size) {
    uint64_t hash = 0;
    for (int i = 0; i < size; i++) {
        hash += chunk[i];
    }
    return hash % HASH_TABLE_SIZE;
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

int deduplicate_chunks(const unsigned char *chunk, int chunk_size, HashTable *hash_table) {
    if (hash_table == NULL) {
        fprintf(stderr, "Hash table is not initialized\n");
        return -1;
    }

    uint64_t chunk_hash = compute_hash(chunk, chunk_size);
    int existing_size;
    int *existing = lookup_hash_table(hash_table, chunk_hash, &existing_size);

    if (existing != NULL && existing_size == chunk_size &&
        memcmp(existing, chunk, chunk_size) == 0) {
        printf("Chunk is a duplicate\n");
        return 0;
    } else {
        int encoded_data[INPUT_SIZE];
        int encoded_size;
        int result = encoding((const char *)chunk, encoded_data, &encoded_size);

        if (result != 0) {
            fprintf(stderr, "Failed to encode chunk\n");
            return -1;
        }

        static int encoding_storage[HASH_TABLE_SIZE][INPUT_SIZE];
        memcpy(encoding_storage[chunk_hash], encoded_data, encoded_size * sizeof(int));

        insert_hash_table(hash_table, chunk_hash, encoding_storage[chunk_hash], encoded_size);
        printf("Chunk is unique\n");
        return 1;
    }
}
