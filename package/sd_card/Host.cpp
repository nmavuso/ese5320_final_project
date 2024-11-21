/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#include <iostream>
#include <ostream>
#include "Utilities.h"
#include "lzw_hls.h"

// Function to check results
static int result_check(const int *Output_HW, int output_size_hw, const int *Output_SW, int output_size_sw) {
    // Check if the output sizes are equal
    if (output_size_hw != output_size_sw) {
        std::cout << "Mismatch in output size: HW=" << output_size_hw
                  << " SW=" << output_size_sw << std::endl;
        return 1;
    }

    // Check if the content matches
    for (int i = 0; i < output_size_hw; ++i) {
        if (Output_HW[i] != Output_SW[i]) {
            std::cout << "Mismatch in test at index " << i
                      << " expected=" << Output_SW[i]
                      << " , got=" << Output_HW[i] << std::endl;
            return 1;
        }
    }

    return 0; // Pass
}

// ------------------------------------------------------------------------------------
// Main program
// ------------------------------------------------------------------------------------
int main(int argc, char **argv) {

    // Check if the output file name is provided
    if (argc <= 1) {
        std::cerr << "Error: No output file name provided. Please specify the output file name." << std::endl;
        return 1; // Exit with an error code
    }

    // File name for storing compressed data
    std::string outputFileName = argv[1];

    // Assume the binary file "lzw_hls.xclbin" is in the same directory as the executable
    std::string binaryFile = "lzw_hls.xclbin";

    // Initialize an event timer for monitoring the application
    printf("Sanity Check inside Host.cpp\n");
    EventTimer timer;

    // ------------------------------------------------------------------------------------
    // Step 1: Initialize the OpenCL environment
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
    // Step 2: Create buffers and initialize test values
    // ------------------------------------------------------------------------------------
    timer.add("Allocate contiguous OpenCL buffers");

    const int MAX_INPUT_SIZE = 1024;
    const int MAX_OUTPUT_SIZE = 1024;

    // Create buffers for input and output
    printf("6\n");
    cl::Buffer input_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(char) * MAX_INPUT_SIZE, NULL, &err);
    cl::Buffer output_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int) * MAX_OUTPUT_SIZE, NULL, &err);
    cl::Buffer output_size_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int), NULL, &err);
    cl::Buffer output_r_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(char) * MAX_OUTPUT_SIZE, NULL, &err);

    timer.add("Set kernel arguments");
    // Set kernel arguments
    krnl_lzw.setArg(0, input_buf);
    krnl_lzw.setArg(1, output_buf);
    krnl_lzw.setArg(2, output_size_buf);
    krnl_lzw.setArg(3, output_r_buf);

    printf("7\n");

    timer.add("Map buffers to userspace pointers");
    // Map buffers to host memory
    char *input = (char *)q.enqueueMapBuffer(input_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(char) * MAX_INPUT_SIZE);
    int *output_hw = (int *)q.enqueueMapBuffer(output_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int) * MAX_OUTPUT_SIZE);
    int *output_size_hw = (int *)q.enqueueMapBuffer(output_size_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int));
    char *output_r = (char *)q.enqueueMapBuffer(output_r_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(char) * MAX_OUTPUT_SIZE);

    printf("8\n");

    timer.add("Populating buffer inputs");
    // Prepare input data
    const char *test_input = "I AM SAM SAM I AM";
    strncpy(input, test_input, MAX_INPUT_SIZE);

    // ------------------------------------------------------------------------------------
    // Step 3: Run the kernel
    // ------------------------------------------------------------------------------------
    timer.add("Memory object migration enqueue host->device");
    printf("9\n");
    cl::Event event_sp;

    // Migrate input buffer to device
    q.enqueueMigrateMemObjects({input_buf}, 0 /* 0 means from host*/, NULL, &event_sp);
    clWaitForEvents(1, (const cl_event *)&event_sp);

    // Launch the kernel
    timer.add("Launch lzw kernel");
    q.enqueueTask(krnl_lzw, NULL, &event_sp);
    timer.add("Wait for lzw kernel to finish running");
    clWaitForEvents(1, (const cl_event *)&event_sp);

    // Migrate output buffers back to host
    timer.add("Read back computation results (implicit device->host migration)");
    q.enqueueMigrateMemObjects({output_buf, output_size_buf, output_r_buf}, CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();

    timer.finish();
    printf("10\n");
      
    // ------------------------------------------------------------------------------------
    // Step 4: Testbench validation
    // ------------------------------------------------------------------------------------

    // HARDWARE WRITING
    std::ofstream outputFile(argv[1], std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open output file: " << argv[1] << std::endl;
        return 1;
    }

    // Write the size of the compressed data
    int compressed_size = *output_size_hw;
    outputFile.write(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));

    // Write the compressed data
    outputFile.write(reinterpret_cast<char*>(output_r), compressed_size * sizeof(int));

    std::cout << "Compressed data successfully written to " << argv[1] << std::endl;
    outputFile.close();
        
    // SOFTWARE AND HARDWARE TESTING
    const int MAX_OUTPUT_SIZE_SW = 1024;
    int output_code_sw[MAX_OUTPUT_SIZE_SW] = {0};
    char output_sw[MAX_OUTPUT_SIZE_SW] = {0};
    int output_size_sw = 0;
    char decoded_fpga[MAX_OUTPUT_SIZE] = {0};
    char decoded_sw[MAX_OUTPUT_SIZE_SW] = {0};
    printf("11\n");

    // Call software LZW function
    lzw_fpga(test_input, output_code_sw, &output_size_sw, output_sw);

    // Check the software version and make sure it's correct
    bool is_same = (strcmp(test_input, output_sw) == 0);

    if (is_same) {
        std::cout << "The output_sw string matches the test_input string." << std::endl;
    } else {
        std::cout << "Mismatch detected between output_sw and test_input." << std::endl;
        std::cout << "test_input: " << test_input << std::endl;
        std::cout << "output_sw: " << output_sw << std::endl;
    }


    bool is_same_hw = (strcmp(test_input, output_r) == 0);

    if (is_same_hw) {
        std::cout << "The output_hw string matches the test_input string." << std::endl;
    } else {
        std::cout << "Mismatch detected between output_hw and test_input." << std::endl;
        std::cout << "test_input: " << test_input << std::endl;
        std::cout << "output_hw: " << output_r << std::endl;
    }

    // For debugging purposes to see where we are at
    printf("12\n");

    // Validate FPGA output against software output
    int failed = result_check(output_hw, *output_size_hw, output_code_sw, output_size_sw);

    std::cout << "--------------- Key execution times ---------------" << std::endl;
    timer.print();

    std::cout << "TEST " << (!failed ? "PASSED" : "FAILED") << std::endl;

    // ------------------------------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------------------------------
    delete[] fileBuf; // Free the binary file buffer
    q.enqueueUnmapMemObject(input_buf, input);
    q.enqueueUnmapMemObject(output_buf, output_hw);
    q.enqueueUnmapMemObject(output_size_buf, output_size_hw);
    q.finish();
     
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
