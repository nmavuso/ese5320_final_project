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
#include "lzw_hls.h"
#include "lzw_hw.h"
#include "cdc_hw.h"
#include "cmd_hw.h"
#include "Utilities.h"
#include <sys/stat.h> // Fallback for older compilers
#include <cstring>     // For memset, if needed

#define NUM_PACKETS 8
#define PIPE_DEPTH 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)
// #define BLOCKSIZE 2048
// #define NUM_ELEMENTS 1024
// #define HEADER 16
// #define INPUT_SIZE 1024
const int MAX_INPUT_SIZE = 4096;
const int MAX_OUTPUT_SIZE = 4096;

int offset = 0;
unsigned char* file;

size_t getFileSize(const std::string& fileName) {
    struct stat fileStat;
    if (stat(fileName.c_str(), &fileStat) != 0) {
        std::cerr << "Error: Unable to retrieve file size for " << fileName << std::endl;
        return 0;
    }
    return fileStat.st_size;
}

int appHost(unsigned char* buffer, unsigned int length,
            cl::Kernel &krnl_lzw, cl::CommandQueue &q,
            cl::Buffer &input_buf, cl::Buffer &output_code_buf, cl::Buffer &output_size_buf, cl::Buffer &output_buf,cl::Buffer &output_length_buf,
            char *input_hw, int *output_code_hw, int *output_size_hw, char *output_hw, int *output_length_hw, std::string outputFileName) {
    std::cout << "Encoding Working Started" << std::endl;

    /*************************************/
    /********** Initialization ***********/
    /*************************************/
    std::cout << "Initializing hash table..." << std::endl;
    HashTable hash_table;
    initialize_hash_table(&hash_table);
    std::cout << "Hash table initialized successfully." << std::endl;

    // Step 1: Process Ethernet Input
    std::cout << "Processing Ethernet Input..." << std::endl;
    unsigned int buffer_size = length;
    std::cout << "Buffer size: " << buffer_size << std::endl;

    // Step 2: Chunking the Ethernet input
    std::cout << "Chunking Ethernet Input..." << std::endl;
    Chunk chunks[NUM_PACKETS];
    int num_chunks = 0;
    cdc(buffer, buffer_size, chunks, &num_chunks);
    std::cout << "Number of chunks created: " << num_chunks << std::endl;

    // Step 3: Deduplication and Encoding
    std::cout << "Starting deduplication and encoding..." << std::endl;
    for (int i = 0; i < num_chunks; ++i) {
        std::cout << "Processing chunk " << i + 1 << "..." << std::endl;
        const char* chunk_data = reinterpret_cast<const char*>(chunks[i].data);
        int chunk_size = chunks[i].size;
        std::cout << "Chunk size: " << chunk_size << std::endl;

        // Deduplicate chunk (conditional check done here)
        std::cout << "Before Deduplicate chunks HERE" << std::endl;
        int is_new_chunk = deduplicate_chunks(chunk_data, chunk_size, &hash_table, krnl_lzw, q, input_buf, output_buf, output_size_buf, output_buf, output_length_buf, input_hw, output_code_hw, output_size_hw, output_hw, output_length_hw, outputFileName);

        std::cout << "Deduplication result for chunk " << i + 1 << ": " << is_new_chunk << std::endl;
    }

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
                printf("Option -%c requires a parameter.\n", optopt);
                break;
            default:
                std::cerr << "Invalid option provided." << std::endl;
                break;
        }
    }
}

void handle_16bit_boundary(const std::string& outputFileName) {
    // Open the file for both reading and writing
    FILE* file = fopen(outputFileName.c_str(), "r+b");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);

    if (file_size <= 0) {
        std::cerr << "File is empty or corrupted." << std::endl;
        fclose(file);
        return;
    }

    // Check if the file is at a 16-bit boundary
    bool is_at_16bit_boundary = (file_size % 2 == 0);

    if (!is_at_16bit_boundary) {
        // Add a 0x00 byte to the file to align to a 16-bit boundary
        uint8_t padding = 0x00;
        fwrite(&padding, sizeof(uint8_t), 1, file);

        // Recalculate the file size after padding
        file_size += 1;

        // Swap the last two bytes in the file
        fseek(file, -2, SEEK_END); // Move to the second last byte
        uint8_t second_last_byte, last_byte;
        fread(&second_last_byte, sizeof(uint8_t), 1, file); // Read the second last byte
        fread(&last_byte, sizeof(uint8_t), 1, file);        // Read the last byte

        // Write them in swapped order
        fseek(file, -2, SEEK_END); // Move back to overwrite the last two bytes
        fwrite(&last_byte, sizeof(uint8_t), 1, file);
        fwrite(&second_last_byte, sizeof(uint8_t), 1, file);
    }

    fclose(file); // Close the file
}

int main(int argc, char* argv[]) {
    // Check if the output file name is provided
    if (argc <= 1) {
        std::cerr << "Error: No output file name provided. Please specify the output file name." << std::endl;
        return 1; // Exit with an error code
    }

    // File name for storing compressed data (make sure it's a bin file)
    std::string outputFileNameNotBin = argv[1];
    std::string outputFileName;

    if (outputFileNameNotBin.size() >= 4 && outputFileNameNotBin.substr(outputFileNameNotBin.size() - 4) == ".bin") {
        outputFileName = outputFileNameNotBin; // It's already a .bin file
    } else {
        // Change the extension to .bin
        outputFileName = outputFileNameNotBin + ".bin";
    }

    struct stat bufferInit;
    if (stat(outputFileName.c_str(), &bufferInit) == 0) {
        // File exists; delete it
        if (std::remove(outputFileName.c_str()) != 0) {
            std::cerr << "Error: Unable to delete existing file: " << outputFileName << std::endl;
            return 1; // Exit with an error code
        } else {
            std::cout << "Existing file " << outputFileName << " deleted successfully." << std::endl;
        }
    }

    // Assume the binary file "lzw_hls.xclbin" is in the same directory as the executable
    std::string binaryFile = "lzw_fpga.xclbin";

    // Initialize an event timer for monitoring the application
    printf("Sanity Check inside Encoder combined Host.cpp\n");
    EventTimer timer;

    // ------------------------------------------------------------------------------------
    // Step 1 FPGA: Initialize the OpenCL environment
    // ------------------------------------------------------------------------------------
    timer.add("OpenCL Initialization");
    cl_int err;

    // Get the Xilinx devices and create a context
    std::vector<cl::Device> devices = get_xilinx_devices();
    if (devices.empty()) {
        std::cerr << "Error: No Xilinx devices found." << std::endl;
        return 1;
    }
    devices.resize(1); // Use only the first device
    cl::Device device = devices[0];
    cl::Context context(device, NULL, NULL, NULL, &err);

    // Read the binary file and create a program
    unsigned fileBufSize;
    char *fileBuf = read_binary_file(binaryFile, fileBufSize);
    if (!fileBuf) {
        std::cerr << "Error: Unable to read binary file: " << binaryFile << std::endl;
        return 1;
    }
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    cl::Program program(context, devices, bins, NULL, &err);

    // Create a command queue and kernel
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    cl::Kernel krnl_lzw(program, "lzw_fpga", &err);

    // ------------------------------------------------------------------------------------
    // Step 2: Create FPGA Buffers
    // ------------------------------------------------------------------------------------
    // Create buffers for input and output
    cl::Buffer input_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(char) * MAX_INPUT_SIZE, NULL, &err);
    cl::Buffer output_code_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int) * MAX_INPUT_SIZE, NULL, &err);
    cl::Buffer output_size_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int), NULL, &err);
    cl::Buffer output_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(char) * MAX_INPUT_SIZE, NULL, &err);
    cl::Buffer output_length_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int), NULL, &err);

    // Map buffers to host pointers
    char *input_hw = (char *)q.enqueueMapBuffer(input_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(char) * MAX_INPUT_SIZE);
    int *output_code_hw = (int *)q.enqueueMapBuffer(output_code_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int) * MAX_INPUT_SIZE);
    char *output_hw = (char *)q.enqueueMapBuffer(output_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(char) * MAX_INPUT_SIZE);
    int *output_size_hw = (int *)q.enqueueMapBuffer(output_size_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int));
    int *output_length_hw = (int *)q.enqueueMapBuffer(output_length_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int));

    // ------------------------------------------------------------------------------------
    // Step 3: Set Kernel Arguments
    // ------------------------------------------------------------------------------------
    krnl_lzw.setArg(0, input_buf);
    // set Argument 1 of input size gets set in the 
    krnl_lzw.setArg(2, output_code_buf);
    krnl_lzw.setArg(3, output_size_buf);
    krnl_lzw.setArg(4, output_buf);
    krnl_lzw.setArg(5, output_length_buf);

    // ------------------------------------------------------------------------------------
    // Ethernet Setup
    // ------------------------------------------------------------------------------------
    stopwatch ethernet_timer;
    unsigned char* input[NUM_PACKETS];
    int writer = 0;
    int is_done = 0;
    unsigned int length = 0;
    int packet_count = 0;
    ESE532_Server server;

    // Default block size
    int blocksize = BLOCKSIZE;

    // Parse command line arguments for block size
    handle_input(argc, argv, &blocksize);

    file = (unsigned char*)malloc(sizeof(unsigned char) * 70000000);
    if (!file) {
        std::cerr << "Memory allocation failed for file buffer!" << std::endl;
        return 1;
    }

    for (int i = 0; i < NUM_PACKETS; i++) {
        input[i] = (unsigned char*)malloc(sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
        if (!input[i]) {
            std::cerr << "Memory allocation failed for input buffer!" << std::endl;

            // Free previously allocated buffers
            for (int j = 0; j < i; j++) {
                free(input[j]);
            }
            free(file);
            return 1;
        }
    }

    server.setup_server(blocksize);

    // ------------------------------------------------------------------------------------
    // Process Ethernet Packets
    // ------------------------------------------------------------------------------------
    writer = PIPE_DEPTH;

    while (true) {
        // Reset writer index if it exceeds the number of packets
        if (writer == NUM_PACKETS) {
            writer = 0;
        }

        ethernet_timer.start();
        int packet_status = server.get_packet(input[writer]);
        ethernet_timer.stop();

        if (packet_status < 0) {
            std::cerr << "Failed to retrieve packet from server." << std::endl;
            break;
        }

        packet_count++;

        // Process the received packet
        unsigned char* buffer = input[writer];
        is_done = buffer[1] & DONE_BIT_L;
        length = buffer[0] | (buffer[1] << 8);
        length &= ~DONE_BIT_H;

        std::cout << "Packet " << packet_count << " received:" << std::endl;
        std::cout << "  Writer index: " << writer << std::endl;
        std::cout << "  Packet length: " << length << " bytes" << std::endl;
        std::cout << "  Is last packet: " << (is_done ? "Yes" : "No") << std::endl;
        std::cout << "  Current offset: " << offset << " bytes" << std::endl;

        // Now that the packet is not done, call the appHost
        appHost(buffer, length, krnl_lzw, q, input_buf, output_code_buf, output_size_buf, output_buf, output_length_buf, input_hw, output_code_hw, output_size_hw, output_hw, output_length_hw, outputFileName);

        // Update the offset
        offset += length;
        writer++;

        // Exit the loop if done
        if (is_done) {
            break;
        }
    }

    // ------------------------------------------------------------------------------------
    // Clean-Up
    // ------------------------------------------------------------------------------------
      
    size_t compressed_outputFileLength = getFileSize(outputFileName);
    size_t inputBytesWritten = offset;

    std::cout << "--------------- Compression Ratio ---------------" << std::endl;
    if (compressed_outputFileLength > 0) {
       float compressionRatio = (float)compressed_outputFileLength/(float) inputBytesWritten;
        std::cout << "Compression Ratio: " << compressionRatio << std::endl;
    } else {
        std::cerr << "No bytes written to compressed file; compression ratio cannot be calculated." << std::endl;
    }

    std::cout << "Original File size: " << inputBytesWritten << std::endl;
    std::cout << "Compressed File size: " << compressed_outputFileLength << std::endl;  
    std::cout << "--------------- Key Throughputs ---------------" << std::endl;
    float ethernet_latency = ethernet_timer.latency() / 1000.0;
    float input_throughput = (inputBytesWritten / 1000000.0) / ethernet_latency; // Mb/s
    std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
    << " (Latency: " << ethernet_latency << " s)." << std::endl;

    for (int i = 0; i < NUM_PACKETS; i++) {
        free(input[i]);
    }

    delete[] fileBuf; // Free the binary file buffer
    q.enqueueUnmapMemObject(input_buf, input_hw);
    q.enqueueUnmapMemObject(output_code_buf, output_code_hw);
    q.enqueueUnmapMemObject(output_buf, output_hw);
    q.enqueueUnmapMemObject(output_size_buf, output_size_hw);
    q.enqueueUnmapMemObject(output_length_buf, output_length_hw);
    q.finish();

    handle_16bit_boundary(outputFileName);

    free(file);

    return 0;
}
