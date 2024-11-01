// Chunk Matching For DeDuplication

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
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
    int **value;            // LZW encoded chunk
    int size;               // size of the encoded chunk
    struct HashEntry *next; // collision detection
} HashEntry;

// Hash table struct
typedef struct
{
    HashEntry **entries;
    int size;
} HashTable;

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

// Function to deduplicate chunks
void deduplicate_chunks(char chunks[NUM_CHUNKS][MAX_CHUNK_SIZE], int chunk_sizes[NUM_CHUNKS], HashTable *hash_table)
{
    // First, initialize our hash table
    if (hash_table == NULL)
    {
        hash_table = initialize_hash_table();
    }
    // TODO: Replace placeholder
    //  key -> hash ; value -> array of ints
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        // compute our hash
        uint64_t chunk_hash = compute_hash(chunks[i], chunk_sizes[i]);
        int **existing = lookup_hash_table(chunk_hash);

        if (existing != NULL && existing->size == chunk_sizes[i] &&
            memcmp(existing->content, chunks[i], chunk_sizes[i]) == 0)
        {
            printf("Chunk %d is a duplicate of chunk %d\n", i, existing->index);
        }
        else
        {
            int *encoding = lzw::encode(chunks[i], NULL); // LZW encode the chunk
            insert_hash_table(chunk_hash, encoding);      // insert new entry into our hash map
            printf("Chunk %d is unique\n", i);
        }
    }
}

int main()
{
    char chunks[NUM_CHUNKS][MAX_CHUNK_SIZE];
    int chunk_sizes[NUM_CHUNKS];
    HashTable *hash_table = initialize_hash_table();

    // Initialize chunks with some sample data
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        chunk_sizes[i] = (i % 100) + 1; // Varying sizes from 1 to 100 bytes
        for (int j = 0; j < chunk_sizes[i]; j++)
        {
            chunks[i][j] = (uint8_t)(i + j);
        }
    }

    // Deduplicate chunks
    deduplicate_chunks(chunks, chunk_sizes, hash_table);

    // Clean up
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        free(hash_table->entries[i]);
    }
    free(hash_table);
    return 0;
}
