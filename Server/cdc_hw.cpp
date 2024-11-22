#include <iostream>
#include <cstring> // For memset
#include <cstdint>
#include "cdc_hw.h"

#ifndef WINDOW_SIZE
#define WINDOW_SIZE 16
#endif

#ifndef MIN_CHUNK_SIZE
#define MIN_CHUNK_SIZE 2048
#endif

#ifndef MAX_CHUNK_SIZE
#define MAX_CHUNK_SIZE 8192
#endif

#ifndef PRIME
#define PRIME 257
#endif

#ifndef MODULUS
#define MODULUS 1000000007
#endif

#ifndef MASK
#define MASK 0x0FFF // 12-bit mask for chunk boundary detection
#endif

// Compute PRIME_POWER_WINDOW_SIZE = PRIME^(WINDOW_SIZE - 1) % MODULUS
uint64_t compute_prime_power_window() {
    uint64_t power = 1;
    for (int i = 1; i < WINDOW_SIZE; ++i) {
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
    // Remove outgoing byte
    hash = (hash + MODULUS - (outgoing * PRIME_POWER_WINDOW_SIZE) % MODULUS) % MODULUS;
    // Add incoming byte
    hash = (hash * PRIME + incoming) % MODULUS;
    return hash;
}

void cdc(const unsigned char* buff, unsigned int buff_size, Chunk chunks[], int* num_chunks) {
    if (buff == nullptr || buff_size == 0) {
        std::cerr << "Error: Input buffer is empty." << std::endl;
        *num_chunks = 0;
        return;
    }

    // Ensure the number of chunks can be stored in the provided array
    int max_chunks = *num_chunks;
    *num_chunks = 0; // Reset the chunk counter

    Chunk current_chunk;
    current_chunk.size = 0;
    memset(current_chunk.data, 0, sizeof(current_chunk.data));

    uint64_t rolling_hash = 0;
    unsigned int window_start = 0;

    for (unsigned int i = 0; i < buff_size; ++i) {
        current_chunk.data[current_chunk.size++] = buff[i];

        if (i - window_start + 1 >= WINDOW_SIZE) {
            if (i - window_start + 1 == WINDOW_SIZE) {
                // Initialize the rolling hash over the current window
                rolling_hash = initialize_hash(&buff[window_start], WINDOW_SIZE);
            } else {
                // Update the rolling hash
                unsigned char outgoing = buff[i - WINDOW_SIZE];
                unsigned char incoming = buff[i];
                rolling_hash = update_hash(rolling_hash, outgoing, incoming);
            }

            // Check for chunk boundary or max chunk size
            if (((rolling_hash & MASK) == 0 && current_chunk.size >= MIN_CHUNK_SIZE) ||
                (current_chunk.size >= MAX_CHUNK_SIZE)) {
                if (*num_chunks >= max_chunks) {
                    std::cerr << "Error: Chunk array size exceeded." << std::endl;
                    return;
                }

                // Store the current chunk
                chunks[*num_chunks] = current_chunk;
                (*num_chunks)++;

                // Reset the current chunk
                current_chunk.size = 0;
                memset(current_chunk.data, 0, sizeof(current_chunk.data));

                // Move window_start to the next position
                window_start = i + 1;
            }
        }
    }

    // Handle any remaining data in the current chunk
    if (current_chunk.size > 0) {
         chunks[*num_chunks] = current_chunk;
        (*num_chunks)++;
    }
}
