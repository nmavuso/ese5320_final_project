#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "lzw.h"
#define NUM_CHUNKS 1000
#define MAX_CHUNK_SIZE 8192
#define HASH_TABLE_SIZE 1000

uint64_t compute_hash(const unsigned char *chunk, int size)
{
uint64_t hash = 0;
    for (int i = 0; i < size; i++)
    {
        hash += chunk[i];
    }
    return hash;
}

typedef struct HashEntry
{
    uint64_t key;
    int *value;             // LZW encoded chunk
    int size;               // Size of the encoded chunk
    struct HashEntry *next; // Collision handling
} HashEntry;

typedef struct
{
    HashEntry **entries;
    int size;
} HashTable;

HashTable *initialize_hash_table()
{
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    if (!table)
    {
        perror("Failed to allocate hash table");
        return NULL;
    }
    table->size = HASH_TABLE_SIZE;
    table->entries = (HashEntry **)malloc(table->size * sizeof(HashEntry *));
    if (!table->entries)
    {
        perror("Failed to allocate hash table entries");
        free(table);
        return NULL;
    }

    for (int i = 0; i < table->size; i++)
    {
        table->entries[i] = NULL;
    }

    return table;
}

void free_hash_table(HashTable *table)
{
    if (!table)
        return;

    for (int i = 0; i < table->size; i++)
    {
        HashEntry *entry = table->entries[i];
        while (entry)
        {
            HashEntry *next = entry->next;
            free(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(table->entries);
    free(table);
}

void insert_hash_table(HashTable *table, uint64_t key, int *value, int size)
{
    int index = key % table->size;
    HashEntry *new_entry = (HashEntry *)malloc(sizeof(HashEntry));
    if (!new_entry)
    {
        perror("Failed to allocate new hash entry");
        return;
    }

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
        HashEntry *current = table->entries[index];
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_entry;
    }
}

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
bool deduplicate_chunks(const unsigned char *chunk, int chunk_size, HashTable *hash_table)
{
    // Initialize hash table if it’s null
    if (hash_table == nullptr)
    {
        hash_table = initialize_hash_table();
        if (!hash_table)
        {
            fprintf(stderr, "Failed to initialize hash table\n");
            return false;
        }
    }

    // Compute the hash for the single chunk
    uint64_t chunk_hash = compute_hash(chunk, chunk_size);

    // Check if chunk already exists in hash table
    int existing_size;
    int *existing = lookup_hash_table(hash_table, chunk_hash, &existing_size);

    if (existing != nullptr && existing_size == chunk_size &&
        memcmp(existing, chunk, chunk_size) == 0)
    {
        printf("Chunk is a duplicate\n");
        return false; // Indicate that this chunk is a duplicate
    }
    else
    {
        // Encode the unique chunk using LZW
        int encoded_size;
        int *encoding = encode(chunk, &encoded_size);
        if (encoding == nullptr)
        {
            fprintf(stderr, "Failed to encode chunk\n");
            return false; // Indicate encoding failure
        }

        // Insert the encoded chunk into the hash table
        insert_hash_table(hash_table, chunk_hash, encoding, encoded_size);
        printf("Chunk is unique\n");
        return true; // Indicate that this chunk is unique
    }
}
