#ifndef CDC_HW_H
#define CDC_HW_H

#include <vector>
#include <cstdint>

#define AVERAGE_CHUNK_SIZE 4096
#define MIN_CHUNK_SIZE 1024
#define WINDOW_SIZE 16//changed from 16
#define MAX_CHUNK_SIZE 2048
#define PRIME 257
#define MODULUS 1000000007

extern const uint64_t PRIME_POWER_WINDOW_SIZE;

struct Chunk {
    unsigned char data[MAX_CHUNK_SIZE];
    int size;
};

uint64_t compute_prime_power_window();

uint64_t initialize_hash(const unsigned char* input, unsigned int window_size);

uint64_t update_hash(uint64_t hash, unsigned char outgoing, unsigned char incoming);

void cdc(const unsigned char *buff, unsigned int buff_size, std::vector<Chunk>& chunks, int *num_chunks);


#endif // CDC_HW_H
