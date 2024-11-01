// Chunk Matching For DeDuplication

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "lzw.h"
#define NUM_CHUNKS 1000
#define MAX_CHUNK_SIZE 8192

#define HASH_TABLE_SIZE 1000
// Function to compute a simple hash by computing addition modulo 2^64
uint64_t compute_hash(char *chunk, int size)
{
    uint64_t hash = 0;
    for (int i = 0; i < size; i++)
    {
        hash += chunk[i];
    }
    return hash % (2 ^ 64);
}

// Hash table entry struct
typedef struct HashEntry
{
    uint64_t key;
    int *value;             // LZW encoded chunk
    int size;               // size of the encoded chunk
    struct HashEntry *next; // collision detection
} HashEntry;

// Hash table struct
typedef struct
{
    HashEntry **entries;
    int size;
} HashTable;

// Function to insert into hash table
void insert_hash_table(HashTable *table, uint64_t key, int *value, int size)
{
    int index = key % table->size;
    HashEntry *new_entry = (HashEntry *)malloc(sizeof(HashEntry));
    new_entry->key = key;
    new_entry->value = value;
    new_entry->size = size;
    new_entry->next = NULL;

    if (table->entries[index] == NULL)
    {
        table->entries[index] = new_entry;
    }
    else
    {
        // Handle collision by chaining
        HashEntry *current = table->entries[index];
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_entry;
    }
}

// Function to lookup in hash table
int *lookup_hash_table(HashTable *table, uint64_t key, int *size)
{
    int index = key % table->size;
    HashEntry *entry = table->entries[index];

    while (entry != NULL)
    {
        if (entry->key == key)
        {
            *size = entry->size;
            return entry->value;
        }
        entry = entry->next;
    }

    *size = 0;
    return NULL;
}
// Function to initialize the hash table
HashTable *initialize_hash_table()
{
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    table->size = HASH_TABLE_SIZE;
    table->entries = (HashEntry **)malloc(table->size * sizeof(HashEntry *));

    // initialize all entries to null
    for (int i = 0; i < table->size; i++)
    {
        table->entries[i] = NULL;
    }

    return table;
}
void deduplicate_chunks(char chunks[NUM_CHUNKS][MAX_CHUNK_SIZE], int chunk_sizes[NUM_CHUNKS], HashTable *hash_table)
{
    if (hash_table == NULL)
    {
        hash_table = initialize_hash_table();
    }

    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        uint64_t chunk_hash = compute_hash(chunks[i], chunk_sizes[i]);
        int existing_size;
        int *existing = lookup_hash_table(hash_table, chunk_hash, &existing_size);

        if (existing != NULL && existing_size == chunk_sizes[i] &&
            memcmp(existing, chunks[i], chunk_sizes[i]) == 0)
        {
            printf("Chunk %d is a duplicate\n", i);
        }
        else
        {
            int encoded_size;
            int *encoding = encode(chunks[i], &encoded_size); // lzw fucntion
            insert_hash_table(hash_table, chunk_hash, encoding, encoded_size);
            printf("Chunk %d is unique\n", i);
        }
    }
}
