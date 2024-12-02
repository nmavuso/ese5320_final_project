// encoder_combined_host.cpp

#include "encoder.h"
#include "lzw_hls.h"
#include "server.h"
#include "stopwatch.h"
#include "cdc_hw.h"
#include "cmd_hw.h"
#include "Utilities.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unordered_set>
#include <string_view>
#include <mutex>

constexpr int NUM_PACKETS = 8;
constexpr int PIPE_DEPTH = 4;
constexpr int DONE_BIT_L = (1 << 7);
constexpr int DONE_BIT_H = (1 << 15);
constexpr int CHUNK_SIZE = 1024;  // Default chunk size
constexpr int MAX_INPUT_SIZE = 4096;   // Define as per your FPGA design
constexpr int MAX_OUTPUT_SIZE = 4096;  // Define as per your FPGA design
constexpr int BLOCKSIZE = 2048;        // Default block size (adjust as needed)
constexpr int NUM_ELEMENTS = 1024;     // Adjust as per your application
constexpr int HEADER = 16;             // Adjust as per your application

std::mutex hash_table_mutex; // Mutex for thread safety

size_t getFileSize(const std::string& fileName) {
    struct stat fileStat;
    if (stat(fileName.c_str(), &fileStat) != 0) {
        std::cerr << "Error: Unable to retrieve file size for " << fileName << std::endl;
        return 0;
    }
    return fileStat.st_size;
}

struct Chunk {
    const unsigned char* data;
    int size;
};

void cdc(unsigned char* buffer, unsigned int buffer_size, Chunk* chunks, int* num_chunks) {
    int index = 0;
    *num_chunks = 0;
    while (index < static_cast<int>(buffer_size)) {
        chunks[*num_chunks].data = &buffer[index];
        int remaining = buffer_size - index;
        chunks[*num_chunks].size = (remaining >= CHUNK_SIZE) ? CHUNK_SIZE : remaining;
        index += chunks[*num_chunks].size;
        (*num_chunks)++;
        if (*num_chunks >= NUM_PACKETS) {
            break; // Prevent exceeding the buffer limit
        }
    }
}

struct HashTable {
    std::unordered_set<size_t> hashes;
};

void initialize_hash_table(HashTable& table) {
    std::lock_guard<std::mutex> lock(hash_table_mutex); // Ensure thread-safe initialization
    table.hashes.clear();
}

int deduplicate_chunks(const char* chunk_data, int chunk_size, HashTable& hash_table,
                       cl::Kernel& krnl_lzw, cl::CommandQueue& q,
                       cl::Buffer& input_buf, cl::Buffer& output_buf, cl::Buffer& output_size_buf,
                       cl::Buffer& output_r_buf, cl::Buffer& output_length_buf,
                       char* input, int* output_hw, int* output_size_hw, char* output_r, int* output_length_hw,
                       const std::string& outputFileName) {
    // Compute hash of chunk_data
    std::string_view chunk_view(chunk_data, chunk_size);
    std::hash<std::string_view> hasher;
    size_t hash_value = hasher(chunk_view);

    // Check if the hash is already in the hash table
    {
        std::lock_guard<std::mutex> lock(hash_table_mutex);
        if (hash_table.hashes.find(hash_value) != hash_table.hashes.end()) {
            std::cout << "Duplicate chunk detected. Skipping compression." << std::endl;
            return 0; // Duplicate chunk
        }
        hash_table.hashes.insert(hash_value); // Insert the hash
    }

    // Compress the chunk using FPGA
    memset(input, 0, MAX_INPUT_SIZE);
    memcpy(input, chunk_data, chunk_size);

    // Set the kernel arguments
    krnl_lzw.setArg(0, input_buf);
    krnl_lzw.setArg(1, chunk_size);
    krnl_lzw.setArg(2, output_buf);
    krnl_lzw.setArg(3, output_size_buf);
    krnl_lzw.setArg(4, output_r_buf);
    krnl_lzw.setArg(5, output_length_buf);

    // Enqueue the kernel
    q.enqueueMigrateMemObjects({input_buf}, 0); // Copy input data to device
    q.enqueueTask(krnl_lzw);
    q.enqueueMigrateMemObjects({output_buf, output_size_buf, output_r_buf, output_length_buf}, CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();

    // Validate the compressed and decompressed outputs
    if (*output_length_hw != chunk_size || memcmp(chunk_data, output_r, chunk_size) != 0) {
        std::cerr << "ERROR: Decoded output does not match the original input." << std::endl;
        return -1; // Error in decompression
    }

    // Write compressed data to the output file
    FILE* outfc = fopen(outputFileName.c_str(), "ab");
    if (!outfc) {
        std::cerr << "Failed to open output file for writing: " << outputFileName << std::endl;
        return -1;
    }

    // Write the size of the compressed data
    fwrite(output_size_hw, sizeof(int), 1, outfc);
    // Write the compressed data
    fwrite(output_hw, sizeof(int), *output_size_hw, outfc);
    fclose(outfc);

    return 1; // New chunk
}

void handle_input(int argc, char* argv[], int& blocksize) {
    int option;
    while ((option = getopt(argc, argv, ":b:")) != -1) {
        if (option == 'b') {
            blocksize = atoi(optarg);
            printf("Block size set to %d bytes.\n", blocksize);
        } else {
            std::cerr << "Invalid or missing option argument. Use -b for block size.\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "Error: No output file name provided. Please specify the output file name." << std::endl;
        return 1;
    }

    std::string outputFileName = argv[1];
    if (outputFileName.size() < 4 || outputFileName.substr(outputFileName.size() - 4) != ".bin") {
        outputFileName += ".bin";
    }

    // Delete the existing file if it exists
    if (stat(outputFileName.c_str(), nullptr) == 0 && remove(outputFileName.c_str()) != 0) {
        std::cerr << "Error: Unable to delete existing file: " << outputFileName << std::endl;
        return 1;
    }

    // OpenCL initialization
    cl_int err;
    std::vector<cl::Device> devices = get_xilinx_devices();
    if (devices.empty()) {
        std::cerr << "Error: No Xilinx devices found." << std::endl;
        return 1;
    }
    cl::Device device = devices[0];
    cl::Context context(device, nullptr, nullptr, nullptr, &err);

    // Read the binary file and create a program
    std::string binaryFile = "lzw_hls.xclbin";
    unsigned fileBufSize;
    char* fileBuf = read_binary_file(binaryFile, fileBufSize);
    if (!fileBuf) {
        std::cerr << "Error: Unable to read binary file: " << binaryFile << std::endl;
        return 1;
    }
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    cl::Program program(context, {device}, bins, nullptr, &err);

    // Create a command queue and kernel
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    cl::Kernel krnl_lzw(program, "lzw_fpga", &err);

    // Buffers for FPGA
    cl::Buffer input_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(char) * MAX_INPUT_SIZE, nullptr, &err);
    cl::Buffer output_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int) * MAX_OUTPUT_SIZE, nullptr, &err);
    cl::Buffer output_size_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int), nullptr, &err);
    cl::Buffer output_r_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(char) * MAX_OUTPUT_SIZE, nullptr, &err);
    cl::Buffer output_length_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int), nullptr, &err);

    char* input_hw = (char*)q.enqueueMapBuffer(input_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(char) * MAX_INPUT_SIZE, nullptr, nullptr, &err);
    int* output_hw = (int*)q.enqueueMapBuffer(output_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int) * MAX_OUTPUT_SIZE, nullptr, nullptr, &err);
    int* output_size_hw = (int*)q.enqueueMapBuffer(output_size_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int), nullptr, nullptr, &err);
    char* output_r = (char*)q.enqueueMapBuffer(output_r_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(char) * MAX_OUTPUT_SIZE, nullptr, nullptr, &err);
    int* output_length_hw = (int*)q.enqueueMapBuffer(output_length_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int), nullptr, nullptr, &err);

    memset(input_hw, 0, sizeof(char) * MAX_INPUT_SIZE);
    memset(output_hw, 0, sizeof(int) * MAX_OUTPUT_SIZE);
    *output_size_hw = 0;
    memset(output_r, 0, sizeof(char) * MAX_OUTPUT_SIZE);
    *output_length_hw = 0;

    // Set kernel arguments that are constant
    krnl_lzw.setArg(0, input_buf);
    krnl_lzw.setArg(2, output_buf);
    krnl_lzw.setArg(3, output_size_buf);
    krnl_lzw.setArg(4, output_r_buf);
    krnl_lzw.setArg(5, output_length_buf);

    // Initialize hash table
    HashTable hash_table;
    initialize_hash_table(hash_table);

    // Ethernet server setup
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
    handle_input(argc, argv, blocksize);

    for (int i = 0; i < NUM_PACKETS; i++) {
        input[i] = (unsigned char*)malloc(sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
        if (!input[i]) {
            std::cerr << "Memory allocation failed for input buffer!" << std::endl;
            for (int j = 0; j < i; j++) {
                free(input[j]);
            }
            return 1;
        }
    }

    server.setup_server(blocksize);

    // Process Ethernet Packets Using FPGA
    writer = PIPE_DEPTH;
    server.get_packet(input[writer]);
    packet_count++;

    unsigned char* buffer = input[writer];
    is_done = buffer[1] & DONE_BIT_L;
    length = buffer[0] | (buffer[1] << 8);
    length &= ~DONE_BIT_H;

    // Application Host Processing
    cdc(buffer + HEADER, length, nullptr, nullptr); // Assuming HEADER is the offset to the data

    // Process chunks
    int num_chunks;
    Chunk chunks[NUM_PACKETS];
    cdc(buffer + HEADER, length, chunks, &num_chunks);

    for (int i = 0; i < num_chunks; ++i) {
        const char* chunk_data = reinterpret_cast<const char*>(chunks[i].data);
        int chunk_size = chunks[i].size;

        int result = deduplicate_chunks(chunk_data, chunk_size, hash_table,
                                        krnl_lzw, q,
                                        input_buf, output_buf, output_size_buf,
                                        output_r_buf, output_length_buf,
                                        input_hw, output_hw, output_size_hw,
                                        output_r, output_length_hw,
                                        outputFileName);

        if (result == -1) {
            std::cerr << "Error during processing chunk " << i + 1 << std::endl;
            return 1;
        }
    }

    writer++;

    while (!is_done) {
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

        buffer = input[writer];
        is_done = buffer[1] & DONE_BIT_L;
        length = buffer[0] | (buffer[1] << 8);
        length &= ~DONE_BIT_H;

        // Application Host Processing
        cdc(buffer + HEADER, length, chunks, &num_chunks);

        for (int i = 0; i < num_chunks; ++i) {
            const char* chunk_data = reinterpret_cast<const char*>(chunks[i].data);
            int chunk_size = chunks[i].size;

            int result = deduplicate_chunks(chunk_data, chunk_size, hash_table,
                                            krnl_lzw, q,
                                            input_buf, output_buf, output_size_buf,
                                            output_r_buf, output_length_buf,
                                            input_hw, output_hw, output_size_hw,
                                            output_r, output_length_hw,
                                            outputFileName);

            if (result == -1) {
                std::cerr << "Error during processing chunk " << i + 1 << std::endl;
                return 1;
            }
        }

        writer++;
    }

    // Clean-Up
    for (int i = 0; i < NUM_PACKETS; i++) {
        free(input[i]);
    }

    delete[] fileBuf; // Free the binary file buffer
    q.enqueueUnmapMemObject(input_buf, input_hw);
    q.enqueueUnmapMemObject(output_buf, output_hw);
    q.enqueueUnmapMemObject(output_size_buf, output_size_hw);
    q.enqueueUnmapMemObject(output_r_buf, output_r);
    q.enqueueUnmapMemObject(output_length_buf, output_length_hw);
    q.finish();

    return 0;
}
