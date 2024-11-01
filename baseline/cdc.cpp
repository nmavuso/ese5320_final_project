#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "App.h"
#include "cdc.h"
#define MIN_CHUNK_SIZE 1024
#define WINDOW_SIZE 16

uint64_t compute_prime_power_window() {
    uint64_t power = 1;
    for (int i = 0; i < WINDOW_SIZE; ++i) {
        power = (power * PRIME) % MODULUS;
    }
    return power;
}

const uint64_t PRIME_POWER_WINDOW_SIZE = compute_prime_power_window();

uint64_t initialize_hash(unsigned char* input, unsigned int window_size) {
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

std::vector<std::vector<unsigned char>> cdc(unsigned char* buff, unsigned int buff_size) {
    uint64_t rolling_hash = initialize_hash(buff, WINDOW_SIZE);
    std::vector<unsigned char> current_chunk;
    std::vector<std::vector<unsigned char>> list_of_chunks;

    for (unsigned int i = 0; i < buff_size; ++i) {
        unsigned char byte = buff[i];
        current_chunk.push_back(byte);

        if (i >= WINDOW_SIZE) {
            unsigned char outgoing = buff[i - WINDOW_SIZE];
            rolling_hash = update_hash(rolling_hash, outgoing, byte);
        }

        if ((rolling_hash == 0 && current_chunk.size() >= MIN_CHUNK_SIZE) || 
            (current_chunk.size() >= MAX_CHUNK_SIZE)) {
            
            list_of_chunks.push_back(current_chunk);
            current_chunk.clear();
            
            if (i + 1 < buff_size) {
                rolling_hash = initialize_hash(&buff[i + 1], WINDOW_SIZE);
            }
        }
    }

    if (!current_chunk.empty()) {
        list_of_chunks.push_back(current_chunk);
    }

    return list_of_chunks;
}
