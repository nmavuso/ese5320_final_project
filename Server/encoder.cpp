#include "encoder.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "server.h"
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>
#include "stopwatch.h"
#include "lzw_hw.h"
#include "cdc_hw.h"
#include "cmd_hw.h"

#define NUM_PACKETS 8
#define PIPE_DEPTH 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)
//#define BLOCKSIZE 2048
//#define NUM_ELEMENTS 1024
//#define HEADER 16
//#define INPUT_SIZE 1024

int offset = 0;
unsigned char* file;

int appHost(unsigned char* buffer, unsigned int length, FILE* outfc) {
    HashTable hash_table;
    initialize_hash_table(&hash_table);
    stopwatch cdc_timer;
    stopwatch deduplicate_chunks_timer;
    stopwatch encoding_timer;
    unsigned int buffer_size = length;
    Chunk chunks[NUM_PACKETS];
    int num_chunks = 0;
    
    for (int i = 0; i < length; ++i) {
     std::cout << "Buffer[" << i<< "].data = "<< buffer[i] <<std::endl;
   }

    cdc_timer.start();
    cdc(buffer, buffer_size, chunks, &num_chunks);
    //Question, are we getting the chunks?
    std::cout << "num_chunks: " << num_chunks <<std::endl;
   for (int i = 0; i < num_chunks; ++i) {
     std::cout << "Chunk[" << i<< "].data = "<< chunks[i].data <<std::endl;
   }
    cdc_timer.stop(); 
    std::cout <<"Delay for CDC: " <<cdc_timer.latency() <<std::endl;
    for (int i = 0; i < num_chunks; ++i) {
        unsigned char* chunk_data = chunks[i].data;
        int chunk_size = chunks[i].size;
        deduplicate_chunks_timer.start();
        int is_new_chunk = deduplicate_chunks(chunk_data, chunk_size, &hash_table);
        deduplicate_chunks_timer.stop();
        //std::cout << " << <<std::endl;
        if (is_new_chunk == 1) {
            int encoded_data[INPUT_SIZE];
            int encoded_size;
            encoding_timer.start();
            int encode_success = encoding((const char*)chunk_data, encoded_data, &encoded_size);
            encoding_timer.stop();

            //Print statement hurts latency
            //Remove them 
            if (encode_success == 0) {
                fwrite(encoded_data, sizeof(int), encoded_size, outfc);
                std::cout << "Encoded Chunk " << i + 1 << " successfully." << std::endl;
            } else {
                std::cerr << "Encoding failed for chunk " << i + 1 << std::endl;
            }
        } else {
            std::cout << "Chunk " << i + 1 << " is a duplicate." << std::endl;
        }
    }
    std::cout <<"Deduplicate chunks latency: " << deduplicate_chunks_timer.latency() << std::endl;
    std::cout << "Encoding Complete" << std::endl;
    return 0;
}

void handle_input(int argc, char* argv[], int* blocksize) {
    int option;
    extern char* optarg;

    while ((option = getopt(argc, argv, ":b:")) != -1) {
        switch (option) {
            case 'b':
                *blocksize = atoi(optarg);
                printf("Block size set to %d bytes.\n", *blocksize);
                break;
            case ':':
                std::cerr << "Option -" << (char)optopt << " requires a parameter." << std::endl;
                break;
            default:
                std::cerr << "Invalid option provided." << std::endl;
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    stopwatch ethernet_timer;
    unsigned char* input[NUM_PACKETS];
    int writer = 0;
    int done = 0;
    int length = 0;
    int count = 0;
    ESE532_Server server;
//   int buff_size;

    int blocksize = BLOCKSIZE;
    handle_input(argc, argv, &blocksize);

    file = (unsigned char*)malloc(70000000 * sizeof(unsigned char));
    if (!file) {
        std::cerr << "Error: Memory allocation for file buffer failed." << std::endl;
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_PACKETS; i++) {
        input[i] = (unsigned char*)malloc(sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
        if (!input[i]) {
            std::cerr << "Error: Memory allocation failed for input buffer at packet " << i << "." << std::endl;
            for (int j = 0; j < i; j++) free(input[j]);
            free(file);
            return EXIT_FAILURE;
        }
    }

    server.setup_server(blocksize);

    FILE* outfc = fopen("output_compressed.bin", "wb");
    if (!outfc) {
        perror("Error opening output_compressed.bin");
        for (int i = 0; i < NUM_PACKETS; i++) free(input[i]);
        free(file);
        return EXIT_FAILURE;
    }

    writer = PIPE_DEPTH;
   
   
   server.get_packet(input[writer]);

   count++;
  
  //get packet 
   unsigned char* buffer = input[writer];
 
  //decode 
  done = buffer[1] & DONE_BIT_L;
  length = buffer[0] | (buffer[1] << 8);
  length &= ~DONE_BIT_H;
 //My function here 
  // buff_size = buffer.size();
    
   std::cout << "Printing the Length: " << length << std::endl;
   appHost(buffer, length, outfc);
   memcpy(&file[offset], &buffer[HEADER], length);
   offset += length;
   writer++; 
   
  //last message 
  while (!done) {
    //reset ring bufffer 
    if (writer == NUM_PACKETS) {
      writer = 0;
   } 
    
  ethernet_timer.start(); 
  server.get_packet(input[writer]);
  ethernet_timer.stop();

 count++;

 //get packet 
   unsigned char* buffer = input[writer];
 
  //decode 
  done = buffer[1] & DONE_BIT_L;
  length = buffer[0] | (buffer[1] << 8);
  length &= ~DONE_BIT_H;
 //My function here
   std::cout << "Printing the Length: " << length << std::endl;
   appHost(buffer, length, outfc);
   memcpy(&file[offset], &buffer[HEADER], length);
   offset += length;
   writer++; 
}



    fseek(outfc, 0, SEEK_END);
    long compressed_outputFileLength = ftell(outfc);
    fclose(outfc);

    FILE* outfd = fopen("output_cpu.bin", "wb");
    int bytes_written = fwrite(&file[0], 1, offset, outfd);
    fclose(outfd);
    
    std::cout << "Original File Size: " << offset << " bytes" << std::endl;
    std::cout << "Compressed File Size: " << compressed_outputFileLength << " bytes" << std::endl;
    std::cout << "Compression Ratio: "
              << (offset > 0 ? (float)compressed_outputFileLength / offset : 0) << std::endl;

    float ethernet_latency = ethernet_timer.latency() / 1000.0;
    float input_throughput = (bytes_written / 1000000.0) / ethernet_latency;
    std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s (Latency: " << ethernet_latency << " s)." << std::endl;

    for (int i = 0; i < NUM_PACKETS; i++) free(input[i]);
    free(file);

    return EXIT_SUCCESS;
}
