// chunk_deduplication.h
#ifndef CHUNK_DEDUPLICATION_H
#define CHUNK_DEDUPLICATION_H

#include <stdint.h>

#define NUM_CHUNKS 1000
#define MAX_CHUNK_SIZE 8192
#define HASH_TABLE_SIZE 1000

// Function to compute a simple hash by computing addition modulo 2^64
uint64_t compute_hash(char *chunk, int size);

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
HashTable *initialize_hash_table();

// Function to deduplicate chunks
// Returns NULL upon no match found
int *deduplicate_chunks(uint8_t chunks[NUM_CHUNKS][MAX_CHUNK_SIZE], int chunk_sizes[NUM_CHUNKS], HashTable *hash_table);

// Function to lookup in the hash table
HashEntry *lookup_hash_table(HashTable *hash_table, uint64_t key);

// Function to insert into the hash table
void insert_hash_table(HashTable *hash_table, uint64_t key, int *value, int size);

#endif // CHUNK_DEDUPLICATION_H
