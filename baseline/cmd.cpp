// Chunk Matching For DeDuplication

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define NUM_CHUNKS 1000
#define MAX_CHUNK_SIZE 8192

// Struct to hold chunk information
typedef struct
{
    uint64_t hash;
    int index;
    uint8_t content[MAX_CHUNK_SIZE];
    int size;
} ChunkInfo;

// Simple hash table
#define HASH_TABLE_SIZE 1024
ChunkInfo *hash_table[HASH_TABLE_SIZE] = {NULL};

// Function to compute a simple hash by computing addition modulo 2^64
uint64_t compute_hash(uint8_t *chunk, int size)
{
    uint64_t hash = 0;
    for (int i = 0; i < size; i++)
    {
        hash += chunk[i];
    }
    return hash % (2 ^ 64);
}
