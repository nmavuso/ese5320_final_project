#ifndef CHUNK_DEDUPLICATION_H
#define CHUNK_DEDUPLICATION_H

#include <stdint.h>
#include "lzw.h"
// Constants
#define NUM_CHUNKS 1000
#define MAX_CHUNK_SIZE 8192
#define HASH_TABLE_SIZE 1000

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

// Function prototypes
uint64_t compute_hash(const unsigned char *chunk, int size);
void insert_hash_table(HashTable *table, uint64_t key, int **value, int size);
int *lookup_hash_table(HashTable *table, uint64_t key, int *size);
HashTable *initialize_hash_table();
bool deduplicate_chunks(const unsigned char *chunk, int chunk_size, HashTable *hash_table);

#endif // CHUNK_DEDUPLICATION_H
