#ifndef CDC_HW_H
#define CDC_HW_H

#include <vector>
#include <cstdint>

#define MIN_CHUNK_SIZE 1024
#define WINDOW_SIZE 16
#define MAX_CHUNK_SIZE 8192
#define PRIME 257
#define MODULUS 1000000007

extern const uint64_t PRIME_POWER_WINDOW_SIZE;

uint64_t compute_prime_power_window();

uint64_t initialize_hash(unsigned char* input, unsigned int window_size);

uint64_t update_hash(uint64_t hash, unsigned char outgoing, unsigned char incoming);

std::vector<std::vector<unsigned char>> cdc(unsigned char* buff, unsigned int buff_size);

#endif // CDC_HW_H
