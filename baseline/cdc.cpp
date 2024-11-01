#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include "Encoder.h"

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

std::vector<std::vector<unsigned char>> test_cdc(const char* file) {
    FILE* fp = fopen(file, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return {};
    }

    fseek(fp, 0, SEEK_END);      // Seek to end of file
    int file_size = ftell(fp);    // Get file size
    fseek(fp, 0, SEEK_SET);       // Seek back to beginning of file

    unsigned char* buff = (unsigned char*)malloc(file_size);
    if (buff == NULL) {
        perror("Not enough memory");
        fclose(fp);
        return {};
    }

    int bytes_read = fread(buff, sizeof(unsigned char), file_size, fp);
    if (bytes_read != file_size) {
        perror("File read error");
        free(buff);
        fclose(fp);
        return {};
    }

    std::vector<std::vector<unsigned char>> chunks = cdc(buff, file_size);

    free(buff);
    fclose(fp);

    return chunks;
}

// Main function
int main() {
    std::vector<std::vector<unsigned char>> chunks = test_cdc("prince.txt");

    for (size_t i = 0; i < chunks.size(); ++i) {
        std::cout << "Chunk " << i + 1 << " size: " << chunks[i].size() << " bytes" << std::endl;
    }

    return 0;

}
