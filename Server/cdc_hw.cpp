
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "cdc_hw.h"
//#define MIN_CHUNK_SIZE 1024
//#define WINDOW_SIZE 16

uint64_t compute_prime_power_window() {
    uint64_t power = 1;
    for (int i = 0; i < WINDOW_SIZE; ++i) {
        power = (power * PRIME) % MODULUS;
    }
    return power;
}

const uint64_t PRIME_POWER_WINDOW_SIZE = compute_prime_power_window();

uint64_t initialize_hash(const unsigned char* input, unsigned int window_size) {
    uint64_t hash = 0;
    for (unsigned int i = 0; i < window_size; ++i) {
        hash = (hash * PRIME + input[i]) % MODULUS;
    }
    return hash;
}

uint64_t update_hash(uint64_t hash, unsigned char outgoing, unsigned char incoming) {
    hash = (hash * PRIME + incoming - (outgoing * PRIME_POWER_WINDOW_SIZE % MODULUS) + MODULUS) % MODULUS;
    return hash;
}

#define MAX_CHUNKS 8192 // Adjust this based on your needs
void cdc(const unsigned char* buff, unsigned int buff_size, std::vector<Chunk>& chunks, int *num_chunks) {
    std::cout << "Buffer Size: " << buff_size << std::endl;

    uint64_t rolling_hash = initialize_hash(buff, WINDOW_SIZE);
    
    Chunk current_chunk;
    current_chunk.size = 0;

    const uint64_t MASK = (1ULL << 10) - 1;  // Creates chunks on average every 1KB

    for (unsigned int i = 0; i < buff_size; ++i) {
        if (current_chunk.size >= MAX_CHUNK_SIZE) {
            chunks.push_back(current_chunk);
            current_chunk.size = 0;
        }

        current_chunk.data[current_chunk.size++] = buff[i];

        if (i >= WINDOW_SIZE) {
            unsigned char outgoing = buff[i - WINDOW_SIZE];
            rolling_hash = update_hash(rolling_hash, outgoing, buff[i]);
        }

        if ((current_chunk.size >= MIN_CHUNK_SIZE && (rolling_hash & MASK) == 0) || 
            (current_chunk.size >= MAX_CHUNK_SIZE)) {
            
            chunks.push_back(current_chunk);
            current_chunk.size = 0;
            
            if (i + 1 < buff_size) {
                rolling_hash = initialize_hash(&buff[i + 1], WINDOW_SIZE);
            }
        }
    }

    if (current_chunk.size > 0) {
        chunks.push_back(current_chunk);
    }

    *num_chunks = chunks.size();
    std::cout << "Number of chunks: " << *num_chunks << std::endl;
}
