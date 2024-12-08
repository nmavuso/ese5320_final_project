#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lzw_hls.h"
#include "lzw_hw.h"
#include "cmd_hw.h"
#include "sha3.h"
#include "Utilities.h"

#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include <stdio.h>
#include <stdbool.h>

const int MAX_INPUT_SIZE = 4096;
const int MAX_OUTPUT_SIZE = 4096;

HashEntry hash_table_entries[HASH_TABLE_SIZE];
HashEntry *hash_table_array[HASH_TABLE_SIZE];

bool is_at_16bit_boundary(FILE* file) {
    long offset = ftell(file); // Get the current file position
    if (offset == -1) {
        perror("ftell failed");
        return false; // Error occurred
    }

    return (offset % 2 == 0); // True if aligned to 16-bit boundary
}


uint64_t compute_hash(const char *chunk, int size) {
 
    uint8_t hash_output[64]; 
    sha3_ctx_t sha3; 
    sha3_init(&sha3, 64); 
    sha3_update(&sha3, chunk, (size_t) size); 
    sha3_final(hash_output, &sha3); 
    uint64_t res = 0; 
    for(int i = 0; i < 8; i++) {
        res |= ((uint64_t) hash_output[i]) << (8 * i); 
    }
    return res % HASH_TABLE_SIZE;
}

void initialize_hash_table(HashTable *table) {
    table->size = HASH_TABLE_SIZE;
    table->entries = hash_table_array;  
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
}

void reset_hash_table(HashTable *table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
}

void insert_hash_table(HashTable *table, uint64_t key, int *value, int size) {
    int index = (int)(key % table->size);
    HashEntry *new_entry = &hash_table_entries[index];

    new_entry->key = key;
    new_entry->value = value;
    new_entry->size = size;
    new_entry->next = table->entries[index];
    table->entries[index] = new_entry;
}

int *lookup_hash_table(HashTable *table, uint64_t key, int *size) {
    int index = (int)(key % table->size);
    HashEntry *entry = table->entries[index];

    while (entry != NULL) {
        if (entry->key == key) {
            *size = entry->size;
            return entry->value;
        }
        entry = entry->next;
    }
    *size = 0;
    return NULL;
}

// void pack_codes_msb_first(const uint16_t* codes, size_t num_codes, std::vector<uint8_t>& packed_data, FILE* file, uint32_t header, std::string outputFileName) {
//     uint32_t bit_buffer = 0;   // Buffer to store bits before writing
//     int bits_in_buffer = 0;    // Number of bits currently in the buffer
//     int code_length = std::ceil(std::log2(MAX_CHUNK_SIZE));

//     // Add the header to the packed data
//     std::cerr << "Header (Hex): 0x" 
//               << std::hex << std::setw(8) << std::setfill('0') << header << std::endl;

//     // Retrieve the last byte from the file if necessary (This comes in handy for later in the function when we swap the bytes)
//     bool isAt16BitBoundary = is_at_16bit_boundary(file);

//     if (!isAt16BitBoundary && ftell(file) > 0) {
//         // Save current file position
//         long current_pos = ftell(file);

//         // Move back to the last byte
//         uint8_t last_byte;
//         FILE* fileSeek = fopen(outputFileName.c_str(), "rb");
//         fseek(fileSeek, -1, SEEK_END);
//         fread(&last_byte, sizeof(uint8_t), 1, fileSeek);
//         std::cout << "Inside Last Byte: 0x" << std::hex << static_cast<int>(last_byte) << std::endl;

//         // Push the last byte into packed_data
//         packed_data.push_back(last_byte);

//         // Truncate the file to remove the last byte
//         fseek(file, -1, SEEK_END);
//         ftruncate(fileno(file), current_pos - 1);
//     }


//     // Break the header into bytes and add to the vector
//     packed_data.push_back((header >> 24) & 0xFF);
//     packed_data.push_back((header >> 16) & 0xFF);
//     packed_data.push_back((header >> 8) & 0xFF);
//     packed_data.push_back(header & 0xFF);

//     std::cout << "Reformating Endianess Starting" << std::endl;
//     for (size_t i = 0; i < num_codes; ++i) {
//         if (codes[i] == 0) continue;

//         // Add the current code_length code to the buffer
//         bit_buffer = (bit_buffer << code_length) | (codes[i] & ((1 << code_length) - 1));
//         bits_in_buffer += code_length;

//         // Write out full bytes from the buffer
//         while (bits_in_buffer >= 8) {
//             uint8_t byte_to_write = (bit_buffer >> (bits_in_buffer - 8)) & 0xFF;

//             if (packed_data.capacity() == 0) {
//                 std::cerr << "Error: packed_data vector is not properly initialized." << std::endl;
//                 return;
//             }

//             packed_data.push_back(byte_to_write);
//             bits_in_buffer -= 8;
//         }
//     }

//     std::cout << "Finished Outside For Loop" << std::endl;

//     // Write any remaining bits from the buffer, padded to align to an 8-bit boundary
//     if (bits_in_buffer > 0) {
//         uint8_t byte_to_write = (bit_buffer << (8 - bits_in_buffer)) & 0xFF;
//         packed_data.push_back(byte_to_write);
//     }

//     std::cout << "Packed Data (Hex and Binary BEFORE ENDIANESS):" << std::endl;
//     for (size_t i = 0; i < packed_data.size(); ++i) {
//         // Print the hex representation
//         printf("Hex: %02x ", packed_data[i]);

//         // Print the binary representation
//         printf("Binary: ");
//         for (int bit = 7; bit >= 0; --bit) {
//             printf("%d", (packed_data[i] >> bit) & 1);
//         }

//         printf("  "); // Separate each byte for readability

//         // Newline every 4 bytes for compactness (or 16 if preferred)
//         if ((i + 1) % 4 == 0) {
//             printf("\n");
//         }
//     }
//     std::cout << std::endl;

//     for (size_t i = 0; i + 1 < packed_data.size(); i += 2) {
//         std::swap(packed_data[i], packed_data[i + 1]);
//     }

//     // Step 3: Write the Packed Compressed Data
//     size_t written_data = fwrite(packed_data.data(), sizeof(uint8_t), packed_data.size(), file);
//     if (written_data != packed_data.size()) {
//         std::cerr << "Error: Failed to write packed LZW compressed data to file." << std::endl;
//         fclose(file);
//         return;
//     }

//     fclose(file);
// }

void pack_codes_msb_first(const uint16_t* codes, size_t num_codes, uint8_t* packed_data, size_t& packed_data_size, size_t max_packed_data_size, FILE* file, uint32_t header, std::string outputFileName) {
    uint32_t bit_buffer = 0;   // Buffer to store bits before writing
    int bits_in_buffer = 0;    // Number of bits currently in the buffer
    int code_length = std::ceil(std::log2(MAX_CHUNK_SIZE));
    packed_data_size = 0;      // Initialize packed data size

    // Add the header to the packed data
    std::cerr << "Header (Hex): 0x" 
              << std::hex << std::setw(8) << std::setfill('0') << header << std::endl;

    // Retrieve the last byte from the file if necessary
    bool isAt16BitBoundary = is_at_16bit_boundary(file);

    if (!isAt16BitBoundary && ftell(file) > 0) {
        // Save current file position
        long current_pos = ftell(file);

        // Move back to the last byte
        uint8_t last_byte;
        FILE* fileSeek = fopen(outputFileName.c_str(), "rb");
        fseek(fileSeek, -1, SEEK_END);
        fread(&last_byte, sizeof(uint8_t), 1, fileSeek);
        std::cout << "Inside Last Byte: 0x" << std::hex << static_cast<int>(last_byte) << std::endl;

        // Push the last byte into packed_data
        if (packed_data_size >= max_packed_data_size) {
            std::cerr << "Error: packed_data array is full." << std::endl;
            return;
        }

        packed_data[packed_data_size++] = last_byte;

        // Truncate the file to remove the last byte
        fseek(file, -1, SEEK_END);
        ftruncate(fileno(file), current_pos - 1);
    }

    // Break the header into bytes and add to the array
    if (packed_data_size + 4 > max_packed_data_size) {
        std::cerr << "Error: packed_data array is full." << std::endl;
        return;
    }

    packed_data[packed_data_size++] = (header >> 24) & 0xFF;
    packed_data[packed_data_size++] = (header >> 16) & 0xFF;
    packed_data[packed_data_size++] = (header >> 8) & 0xFF;
    packed_data[packed_data_size++] = header & 0xFF;

    std::cout << "Reformatting Endianness Starting" << std::endl;
    for (size_t i = 0; i < num_codes; ++i) {
        if (codes[i] == 0) continue;

        // Add the current code_length code to the buffer
        bit_buffer = (bit_buffer << code_length) | (codes[i] & ((1 << code_length) - 1));
        bits_in_buffer += code_length;

        // Write out full bytes from the buffer
        while (bits_in_buffer >= 8) {
            if (packed_data_size >= max_packed_data_size) {
                std::cerr << "Error: packed_data array is full." << std::endl;
                return;
            }

            uint8_t byte_to_write = (bit_buffer >> (bits_in_buffer - 8)) & 0xFF;
            packed_data[packed_data_size++] = byte_to_write;
            bits_in_buffer -= 8;
        }
    }

    std::cout << "Finished Outside For Loop" << std::endl;

    // Write any remaining bits from the buffer, padded to align to an 8-bit boundary
    if (bits_in_buffer > 0) {
        if (packed_data_size >= max_packed_data_size) {
            std::cerr << "Error: packed_data array is full." << std::endl;
            return;
        }

        uint8_t byte_to_write = (bit_buffer << (8 - bits_in_buffer)) & 0xFF;
        packed_data[packed_data_size++] = byte_to_write;
    }

    // std::cout << "Packed Data (Hex and Binary BEFORE ENDIANESS):" << std::endl;
    // for (size_t i = 0; i < packed_data_size; ++i) {
    //     // Print the hex representation
    //     printf("Hex: %02x ", packed_data[i]);

    //     // Print the binary representation
    //     printf("Binary: ");
    //     for (int bit = 7; bit >= 0; --bit) {
    //         printf("%d", (packed_data[i] >> bit) & 1);
    //     }

    //     printf("  "); // Separate each byte for readability

    //     // Newline every 4 bytes for compactness (or 16 if preferred)
    //     if ((i + 1) % 4 == 0) {
    //         printf("\n");
    //     }
    // }
    // std::cout << std::endl;

    for (size_t i = 0; i + 1 < packed_data_size; i += 2) {
        std::swap(packed_data[i], packed_data[i + 1]);
    }

    // Step 3: Write the Packed Compressed Data
    size_t written_data = fwrite(packed_data, sizeof(uint8_t), packed_data_size, file);
    if (written_data != packed_data_size) {
        std::cerr << "Error: Failed to write packed LZW compressed data to file." << std::endl;
        fclose(file);
        return;
    }

    fclose(file);
}

void clean_chunk(const char* chunk, int chunk_size, char* clean_data, int& clean_size) {
    clean_size = 0;

    // Clean and filter only valid payload data
    for (int i = 0; i < chunk_size; ++i) {
        // Skip any unexpected values like flags or padding
        if (0 < chunk[i] && chunk[i] < 128) {
            clean_data[clean_size++] = static_cast<char>(chunk[i]);
        }
    }

    // Null-terminate the cleaned data
    clean_data[clean_size] = '\0';
}

int deduplicate_chunks(const char *chunk, int chunk_size, HashTable *hash_table, cl::Kernel &krnl_lzw, cl::CommandQueue &q, cl::Buffer &input_buf, cl::Buffer &output_code_buf, cl::Buffer &output_size_buf, cl::Buffer &output_buf, cl:: Buffer &output_length_buf, char *input_hw, int *output_code, int *output_size, char *output, int *output_length, std::string outputFileName){
    if (hash_table == NULL) {
        fprintf(stderr, "Hash table is not initialized\n");
        return -1;
    }

    printf("Computing hash for chunk of size %d\n", chunk_size);
    uint64_t chunk_hash = compute_hash(chunk, chunk_size);
    printf("Computed hash: %lu\n", chunk_hash);

    int existing_size;
    int *existing = lookup_hash_table(hash_table, chunk_hash, &existing_size);
    printf("Lookup result: %p, existing size: %d\n", (void *)existing, existing_size);

    // Output File (Get Ready to Write)
    FILE *outfc = fopen(outputFileName.c_str(), "ab");
    if (!outfc) {
        perror("Failed to open output file for appending compressed data.");
        return 1; // Exit with an error code
    }

    if (existing != NULL && existing_size == chunk_size &&
        memcmp(existing, chunk, chunk_size) == 0) {
        printf("Chunk is a duplicate\n");

        // Write the chunk with its corresponding Chunk Index
        uint32_t header = (chunk_hash | 0x80000000);
        size_t written_header = fwrite(&header, sizeof(uint32_t), 1, outfc);
        if (written_header != 1) {
            std::cerr << "Error: Failed to write LZW chunk header to file." << std::endl;
            fclose(outfc);
            return 1; // Exit with an error code
        }

        return 0;
    } else {
        // ----------------------
        // Kernel Hardware Execution
        // ----------------------
        printf("Inside the Kernel Hardware Execution...\n");

        std::cout << "Chunk content (size: " << chunk_size << "): ";
        // for (int i = 0; i < chunk_size; ++i) {
        //     std::cout << chunk[i];
        // }
        std::cout << std::endl;

        // Clean and then copy the chunk data into the input hardware array that then gets sent to the FPGA
        std::cerr << "Chunk Size (No Cleaned): " << chunk_size << std::endl;

        // Test out values in input are valid
        // char clean_data[MAX_INPUT_SIZE];
        // int clean_size;

        // clean_chunk(chunk, chunk_size, clean_data, clean_size);
        //         std::cerr << "Cleaned Size: " << clean_size << std::endl;

        // strncpy(input_hw, (const char *)clean_data, MAX_INPUT_SIZE);

        // std::cout << "Cleaned Data: ";
        // for (int i = 0; i < clean_size; ++i) {
        //     std::cout << input_hw[i];
        // }
        // std::cout << std::endl;

        // Set the input_size argument
        // std::cerr << "Cleaned Size: " << clean_size << std::endl;
        // krnl_lzw.setArg(1, clean_size);

        // printf("Running Software LZW...\n");
        // lzw_sw(input_hw, clean_size, output_code, *output_size, output, *output_length);
        lzw_sw(chunk, chunk_size, output_code, *output_size, output, *output_length);

        // // Migrate input buffer to FPGA
        // printf("KRNL 2...\n");
        // q.enqueueMigrateMemObjects({input_buf}, 0 /* Host to device */, NULL, NULL);
        // printf("KRNL 3...\n");
        // q.finish();

        // // Launch the FPGA kernel
        // q.enqueueTask(krnl_lzw, NULL, NULL);
        // printf("KRNL 4...\n");
        // q.finish();

        // // Migrate output buffer back to host
        // printf("KRNL 5...\n");
        // q.enqueueMigrateMemObjects({output_code_buf, output_size_buf, output_buf, output_length_buf}, CL_MIGRATE_MEM_OBJECT_HOST);
        // q.finish();

        // Retrieve encoded data
        int encoded_size = *output_size;
        printf("Encoded Size: %d\n", encoded_size);

        // Print encoded data 
        // printf("Encoded Output Codes FPGA: ");
        // for (int j = 0; j < encoded_size; ++j) {
        //     printf("%d ", output_code[j]);
        // }

        // printf("Decoded Output FPGA: ");
        // for (int j = 0; j < encoded_size; ++j) {
        //     printf("%c", output[j]);
        // }

        // Print out decoded output:
        // std::cout << "output_hw: " << output_r << std::endl;

        printf("\n");

        if (encoded_size == 0) {
            fprintf(stderr, "Failed to encode chunk\n");
            return -1;
        }

        // Step 1: Write the LZW Chunk Header
        // Header: Bit 0 = 0 (LZW chunk), Bits 31–1 = encoded_size (compressed data length)
        uint32_t header = (encoded_size & 0x7FFFFFFF);
        
        // Step 2: Pack the Compressed Data (e.g. 13-bit codes) into an 8-bit-aligned format
        // std::vector<uint8_t> packed_data;
        // pack_codes_msb_first(reinterpret_cast<const uint16_t*>(output_code), encoded_size, packed_data, outfc, header, outputFileName);

        size_t max_packed_data_size = encoded_size + 10;
        uint8_t packed_data[max_packed_data_size];
        size_t packed_data_size = 0;
        pack_codes_msb_first(reinterpret_cast<const uint16_t*>(output_code), encoded_size, packed_data, packed_data_size, max_packed_data_size, outfc, header, outputFileName);

        // Ensure chunk_hash is within bounds
        if (chunk_hash >= HASH_TABLE_SIZE) {
            fprintf(stderr, "Error: chunk_hash (%lu) out of bounds\n", chunk_hash);
            return -1;
        }

        static int encoding_storage[HASH_TABLE_SIZE][MAX_CHUNK_STORAGE];

        // Check encoded_size bounds
        if (encoded_size > MAX_CHUNK_STORAGE) {
            fprintf(stderr, "Error: encoded_size (%d) exceeds MAX_CHUNK_STORAGE (%d)\n", encoded_size, MAX_CHUNK_STORAGE);
            return -1;
        }

        // Copy encoded data to storage
        memcpy(encoding_storage[chunk_hash], output, encoded_size * sizeof(int));
        printf("Storing encoded data for hash: %lu\n", chunk_hash);

        // Insert into hash table
        insert_hash_table(hash_table, chunk_hash, encoding_storage[chunk_hash], encoded_size);
        return 1;
    }
}
