#ifndef CHUNK_DEDUPLICATION_H
#define CHUNK_DEDUPLICATION_H

#include <stdint.h>
#include <stdbool.h>
#include "lzw_hls.h"
#include "Utilities.h"
#include <string.h>

#define NUM_CHUNKS 3
#define MAX_CHUNK_SIZE 8192
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

int deduplicate_chunks(const char *chunk, int chunk_size, HashTable *hash_table,
                       cl::Kernel &krnl_lzw, cl::CommandQueue &q,
                       cl::Buffer &input_buf, cl::Buffer &output_buf, cl::Buffer &output_size_buf, cl::Buffer &output_r_buf,
                       char *input, int *output_hw, int *output_size_hw, char *output_r, std::string outputFileName);
#endif 
