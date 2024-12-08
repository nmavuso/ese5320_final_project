#ifndef CHUNK_DEDUPLICATION_H
#define CHUNK_DEDUPLICATION_H

#include <stdint.h>
#include <stdbool.h>
#include "lzw_hls.h"
#include "Utilities.h"
#include <string.h>

#define NUM_CHUNKS 3
#define MAX_CHUNK_SIZE 8192

// This is because each 8 bit char can in the worst case map to 13 bits
#define MAX_CHUNK_STORAGE (MAX_CHUNK_SIZE * (13 / 8))
#define HASH_TABLE_SIZE 1024
// #define INPUT_SIZE 256

typedef struct HashEntry {
    uint64_t key;
    int *value;             
    int size;               
    struct HashEntry *next; 
} HashEntry;

typedef struct {
    HashEntry **entries;   
    int size;               
} HashTable;


uint64_t compute_hash(const char *chunk, int size);

void initialize_hash_table(HashTable *table);

void free_hash_table(HashTable *table);

void insert_hash_table(HashTable *table, uint64_t key, int *value, int size);

int *lookup_hash_table(HashTable *table, uint64_t key, int *size);

int deduplicate_chunks(const char *chunk, int chunk_size, HashTable *hash_table, cl::Kernel &krnl_lzw, cl::CommandQueue &q, cl::Buffer &input_buf, cl::Buffer &output_code_buf, cl::Buffer &output_size_buf, cl::Buffer &output_buf, cl:: Buffer &output_length_buf, char *input_hw, int *output_code, int *output_size, char *output, int *output_length, std::string outputFileName);

#endif 
