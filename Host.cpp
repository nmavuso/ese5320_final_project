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
    // Initialize an event timer for monitoring the application
    printf("Sanity Check inside Host.cpp");
    printf("Extra space check");

    EventTimer timer;

    // ------------------------------------------------------------------------------------
    // Step 1: Initialize the OpenCL environment
    // ------------------------------------------------------------------------------------
    timer.add("OpenCL Initialization");
    cl_int err;
    std::string binaryFile = argv[1];
    unsigned fileBufSize;
    std::vector<cl::Device> devices = get_xilinx_devices();
    devices.resize(1);
    cl::Device device = devices[0];
    cl::Context context(device, NULL, NULL, NULL, &err);
    char *fileBuf = read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    cl::Program program(context, devices, bins, NULL, &err);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    cl::Kernel krnl_lzw(program, "lzw_fpga", &err);

    // ------------------------------------------------------------------------------------
    // Step 2: Create buffers and initialize test values
    // ------------------------------------------------------------------------------------
    timer.add("Allocate contiguous OpenCL buffers");

    const int MAX_INPUT_SIZE = 1024;
    const int MAX_OUTPUT_SIZE = 1024;

    // Create buffers for input and output
    cl::Buffer input_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(char) * MAX_INPUT_SIZE, NULL, &err);
    cl::Buffer output_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int) * MAX_OUTPUT_SIZE, NULL, &err);
    cl::Buffer output_size_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int), NULL, &err);

    timer.add("Set kernel arguments");
    // Set kernel arguments
    krnl_lzw.setArg(0, input_buf);
    krnl_lzw.setArg(1, output_buf);
    krnl_lzw.setArg(2, output_size_buf);

    timer.add("Map buffers to userspace pointers");
    // Map buffers to host memory
    char *input = (char *)q.enqueueMapBuffer(input_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(char) * MAX_INPUT_SIZE);
    int *output_hw = (int *)q.enqueueMapBuffer(output_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int) * MAX_OUTPUT_SIZE);
    int *output_size_hw = (int *)q.enqueueMapBuffer(output_size_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(int));

    timer.add("Populating buffer inputs");
    // Prepare input data
    const char *test_input = "I AM SAM SAM I AM";
    strncpy(input, test_input, MAX_INPUT_SIZE);

    // ------------------------------------------------------------------------------------
    // Step 3: Run the kernel
    // ------------------------------------------------------------------------------------
    timer.add("Memory object migration enqueue host->device");
    cl::Event event_sp;
    q.enqueueMigrateMemObjects({input_buf}, 0 /* 0 means from host*/, NULL, &event_sp);
    clWaitForEvents(1, (const cl_event *)&event_sp);

    timer.add("Launch lzw kernel");
    q.enqueueTask(krnl_lzw, NULL, &event_sp);
    timer.add("Wait for lzw kernel to finish running");
    clWaitForEvents(1, (const cl_event *)&event_sp);

    timer.add("Read back computation results (implicit device->host migration)");
    q.enqueueMigrateMemObjects({output_buf, output_size_buf}, CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();
    timer.finish();
     

     //What is the purpose of this Host code??
      
    // ------------------------------------------------------------------------------------
    // Step 4: Testbench validation
    // ------------------------------------------------------------------------------------
    const int MAX_OUTPUT_SIZE_SW = 1024;
    int output_sw[MAX_OUTPUT_SIZE_SW] = {0};
    int output_size_sw = 0;
    char decoded_fpga[MAX_OUTPUT_SIZE] = {0};
    char decoded_sw[MAX_OUTPUT_SIZE_SW] = {0};

    // Call software LZW function
	char *encoding_success;
	lzw_fpga(test_input, output_sw, &output_size_sw, encoding_success);
	if (encoding_success != 0) {
    	std::cerr << "Error: Software encoding failed!" << std::endl;
    	return 1;
	}

    // Validate FPGA output against software output
    int failed = result_check(output_hw, *output_size_hw, output_sw, output_size_sw);

    std::cout << "--------------- Key execution times ---------------" << std::endl;
    timer.print();

    std::cout << "TEST " << (!failed ? "PASSED" : "FAILED") << std::endl;

    // ------------------------------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------------------------------
    delete[] fileBuf;
    q.enqueueUnmapMemObject(input_buf, input);
    q.enqueueUnmapMemObject(output_buf, output_hw);
    q.enqueueUnmapMemObject(output_size_buf, output_size_hw);
    q.finish();

    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
