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
#include <fcntl.h>
#include <sys/mman.h>
#include "stopwatch.h"
#include "lzw_hw.h"
#include "cdc_hw.h"
#include "cmd_hw.h"

#define NUM_PACKETS 8
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)

int offset = 0;
unsigned char* file;

void handle_input(int argc, char* argv[], int* blocksize) {
	int x;
	extern char *optarg;

	while ((x = getopt(argc, argv, ":b:")) != -1) {
		switch (x) {
		case 'b':
			*blocksize = atoi(optarg);
			printf("blocksize is set to %d optarg\n", *blocksize);
			break;
		case ':':
			printf("-%c without parameter\n", optopt);
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

	// default is 2k
	int blocksize = BLOCKSIZE;

	// set blocksize if decalred through command line
	handle_input(argc, argv, &blocksize);

	file = (unsigned char*) malloc(sizeof(unsigned char) * 70000000);
	if (file == NULL) {
		printf("help\n");
	}

	for (int i = 0; i < NUM_PACKETS; i++) {
		input[i] = (unsigned char*) malloc(
				sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
		if (input[i] == NULL) {
			std::cout << "aborting " << std::endl;
			return 1;
		}
	}

	server.setup_server(blocksize);

	writer = pipe_depth;
	server.get_packet(input[writer]);
	count++;

  	//Ethernet input 
        // get packet
	//unsigned char* buffer = input[writer];
        unsigned char* buff = input[writer];
        /*************************************/
	/********** Initialization ***********/
	/*************************************/
 	HashTable hash_table;
 	initialize_hash_table(&hash_table);

     // Step 1: Ethernet Input .....
     //unsigned char buff[] = "I AM SAM SAM I AM WAIT WHO AM I? OH YES I AM SAM SAM I AM";
        unsigned int buff_size = sizeof(buff) - 1;
    //Step 2: Chunking the Ethernet input
        Chunk chunks[NUM_CHUNKS];
	int num_chunks = 0;


	//void cdc(unsigned char *buff, unsigned int buff_size, Chunk chunks[], int *num_chunks)
 	cdc(buff, buff_size, chunks, &num_chunks);
 	//Step 3: Deduplication


 	for (size_t i = 0; i < num_chunks; ++i) {
    	unsigned char *chunk_data = chunks[i].data;
    	int chunk_size = chunks[i].size;

    	int new_chunk = deduplicate_chunks(chunk_data, chunk_size, &hash_table);

    	if (new_chunk) {
      	int encoded_data[INPUT_SIZE];
      	int encoded_size;
      	int encode_success = encoding((const char *) chunk_data, encoded_data, &encoded_size);
      	if (encode_success == 0) {
        	printf("Chunk %d successfully encoded. Encoded data: ", i+1);
        	for (int j = 0; j < encoded_size; ++j) {
        	  printf("%d ", encoded_data[j]);
        	}
       	 printf("\n");
       	}
       	else {
        printf("Encoding failed for chunk %d\n", i + 1);
       	}
    	} else {
      printf("Chunk %d is a duplicate and was not re-encoded.\n", i + 1);
    	}
    }

 	printf("Encoding Complete");


	// decode
	done = buff[1] & DONE_BIT_L;
	length = buff[0] | (buff[1] << 8);
	length &= ~DONE_BIT_H;
	// printing takes time so be weary of transfer rate
	//printf("length: %d offset %d\n",length,offset);

	// we are just memcpy'ing here, but you should call your
	// top function here.
	memcpy(&file[offset], &buff[HEADER], length);

	offset += length;
	writer++;

	//last message
	while (!done) {
		// reset ring buffer
		if (writer == NUM_PACKETS) {
			writer = 0;
		}

		ethernet_timer.start();
		server.get_packet(input[writer]);
		ethernet_timer.stop();

		count++;

		// get packet
		unsigned char* buffer = input[writer];

		// decode
		done = buffer[1] & DONE_BIT_L;
		length = buffer[0] | (buffer[1] << 8);
		length &= ~DONE_BIT_H;
		//printf("length: %d offset %d\n",length,offset);
		memcpy(&file[offset], &buffer[HEADER], length);

		offset += length;
		writer++;
	}

	// write file to root and you can use diff tool on board
	FILE *outfd = fopen("output_cpu.bin", "wb");

	int bytes_written = fwrite(&file[0], 1, offset, outfd);
	printf("write file with %d\n", bytes_written);

	// Find size of output file
	fseek(outfd, 0, SEEK_END);
    long outputFileLength = ftell(outfd);
	std::cout << "--------------- Compression Ratio ---------------" << std::endl;
	long compressionRatio = offset / outputFileLength;
	std::cout << compressionRatio << std::endl;

	fclose(outfd);

	for (int i = 0; i < NUM_PACKETS; i++) {
		free(input[i]);
	}

	free(file);
	std::cout << "--------------- Key Throughputs ---------------" << std::endl;
	float ethernet_latency = ethernet_timer.latency() / 1000.0;
	float input_throughput = (bytes_written * 8 / 1000000.0) / ethernet_latency; // Mb/s
	std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
			<< " (Latency: " << ethernet_latency << "s)." << std::endl;

	return 0;
}

