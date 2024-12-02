#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl2.hpp> // For OpenCL 1.2
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "lzw_hls.h" // Kernel header file
#include "Utilities.h" // OpenCL utility functions

#define INPUT_SIZE 256   // Maximum input size
#define OUTPUT_SIZE 512  // Maximum output size

// Error-checking macro with debugging messages
inline void CHECK_ERROR(cl_int err, const char* msg) {
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: " << msg << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "DEBUG: " << msg << " succeeded." << std::endl;
    }
}

void encoding_sw(
    const char input[INPUT_SIZE],
    int input_size,
    int output_code[INPUT_SIZE],
    int &output_size
) {
    struct DictionaryEntry {
        int prefix_code;
        char character;
    };

    DictionaryEntry table[TABLE_SIZE];

    int table_size = 256; // Start after single-character entries

    // Initialize the dictionary with single characters
    for (int i = 0; i < 256; i++) {
        table[i].prefix_code = -1; // No prefix
        table[i].character = (char)i;
    }

    int p = (unsigned char)input[0]; // Code of the current prefix
    int out_index = 0;

    for (int i = 1; i < input_size; i++) {
        char c = input[i];

        // Search for (p, c) in the dictionary
        int found = -1;
        for (int j = 256; j < table_size; j++) {
            if (table[j].prefix_code == p && table[j].character == c) {
                found = j;
                break;
            }
        }

        if (found != -1) {
            // Update p to the code of (p, c)
            p = found;
        } else {
            // Output code for p
            output_code[out_index++] = p;

            // Add (p, c) to the dictionary if space is available
            if (table_size < TABLE_SIZE) {
                table[table_size].prefix_code = p;
                table[table_size].character = c;
                table_size++;
            }

            // Update p to code of c
            p = (unsigned char)c;
        }
    }

    // Output code for the last sequence in p
    output_code[out_index++] = p;

    output_size = out_index;
}

void decoding_sw(
    const int encoded_data[INPUT_SIZE],
    int encoded_size,
    char output[INPUT_SIZE],
    int &output_length
) {
    struct DictionaryEntry {
        int prefix_code;
        char character;
    };

    DictionaryEntry table[TABLE_SIZE];

    int table_size = 256; // Start after single-character entries

    // Initialize the dictionary with single characters
    for (int i = 0; i < 256; i++) {
        table[i].prefix_code = -1; // No prefix
        table[i].character = (char)i;
    }

    int out_index = 0;
    int prev_code = encoded_data[0];

    // Output the string corresponding to prev_code
    char stack[MAX_STACK_SIZE];
    int stack_index = 0;

    // Build the string for prev_code
    int current_code = prev_code;
    while (current_code != -1 && stack_index < MAX_STACK_SIZE) {
        stack[stack_index++] = table[current_code].character;
        current_code = table[current_code].prefix_code;
    }

    // Output in reverse order
    for (int i = stack_index - 1; i >= 0; i--) {
        output[out_index++] = stack[i];
    }

    // Main decoding loop
    for (int i = 1; i < encoded_size; i++) {
        int current_code = encoded_data[i];
        stack_index = 0;

        int temp_code = current_code;
        bool code_in_table = (current_code < table_size);

        // Build the string for current_code
        while (true) {
            if (code_in_table) {
                if (stack_index < MAX_STACK_SIZE) {
                    stack[stack_index++] = table[temp_code].character;
                    temp_code = table[temp_code].prefix_code;
                    if (temp_code == -1) break;
                } else {
                    // Stack overflow protection
                    break;
                }
            } else {
                // Special case handling
                if (stack_index < MAX_STACK_SIZE) {
                    stack[stack_index++] = table[prev_code].character;
                    temp_code = prev_code;
                    code_in_table = true;
                } else {
                    // Stack overflow protection
                    break;
                }
            }
        }

        // Output in reverse order
        for (int j = stack_index - 1; j >= 0; j--) {
            output[out_index++] = stack[j];
        }

        // Add new entry to the dictionary
        if (table_size < TABLE_SIZE) {
            table[table_size].prefix_code = prev_code;
            table[table_size].character = stack[stack_index - 1];
            table_size++;
        }

        prev_code = current_code;
    }

    output_length = out_index;
}

void lzw_sw(
    const char input[INPUT_SIZE],
    int input_size,
    int output_code[INPUT_SIZE],
    int &output_size,
    char output[INPUT_SIZE],
    int &output_length
) {
    encoding_sw(input, input_size, output_code, output_size);
    decoding_sw(output_code, output_size, output, output_length);
}


int main(int argc, char* argv[]) {
    std::cout << "DEBUG: Starting the program." << std::endl;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    // Read the XCLBIN file path from the command-line argument
    std::string binaryFile = argv[1];
    std::cout << "DEBUG: XCLBIN file path: " << binaryFile << std::endl;

    const char Input[] = "WYS*WYGWYS*WYSWYSG"; // Example input string
    int input_size = strlen(Input);
    std::cout << "DEBUG: Input string: " << Input << std::endl;

    int Output_HW[OUTPUT_SIZE] = {0};  // Hardware encoded output buffer
    int output_size_hw = 0;           // Hardware encoded output size
    char decoded_hw[INPUT_SIZE] = {0}; // Hardware decoded output buffer
    int output_length_hw = 0;         // Hardware decoded output size

    // ------------------------------------------------------------------------------------
    // Step 1: Initialize the OpenCL environment
    // ------------------------------------------------------------------------------------
    std::cout << "DEBUG: Initializing OpenCL environment." << std::endl;
    cl_int err;
    unsigned fileBufSize;

    // Discover Xilinx devices
    std::vector<cl::Device> devices = get_xilinx_devices();
    if (devices.empty()) {
        std::cerr << "ERROR: No Xilinx devices found. Ensure the FPGA is properly connected." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "DEBUG: Found " << devices.size() << " Xilinx device(s)." << std::endl;

    cl::Device device = devices[0];
    std::cout << "Using device: " << device.getInfo<CL_DEVICE_NAME>(&err) << std::endl;
    CHECK_ERROR(err, "Getting device name");

    cl::Context context(device, NULL, NULL, NULL, &err);
    CHECK_ERROR(err, "Creating OpenCL context");

    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    CHECK_ERROR(err, "Creating command queue");

    // Load the XCLBIN binary
    std::cout << "DEBUG: Reading XCLBIN file." << std::endl;
    char* fileBuf = read_binary_file(binaryFile, fileBufSize);
    if (!fileBuf) {
        std::cerr << "ERROR: Failed to read XCLBIN file: " << binaryFile << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "DEBUG: XCLBIN file read successfully. Size: " << fileBufSize << " bytes." << std::endl;

    // Create program from binary
    std::cout << "DEBUG: Creating program from binary." << std::endl;
    cl::Program::Binaries bins;
    bins.push_back({fileBuf, fileBufSize});

    devices.resize(1); // Ensure devices vector has only one device
    cl::Program program(context, devices, bins, NULL, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: Failed to create program from binary (" << err << ")." << std::endl;

        // Additional info
        std::vector<cl::Device> devices_in_program = program.getInfo<CL_PROGRAM_DEVICES>(&err);
        std::cerr << "DEBUG: Number of devices in program: " << devices_in_program.size() << std::endl;

        // Check build log
        std::string build_log;
        program.getBuildInfo(device, CL_PROGRAM_BUILD_LOG, &build_log);
        std::cerr << "Build Log:" << std::endl << build_log << std::endl;
        return EXIT_FAILURE;
    } else {
        std::cout << "DEBUG: Program created successfully from binary." << std::endl;
    }

    // Create the kernel
    std::cout << "DEBUG: Creating kernel." << std::endl;
    cl::Kernel krnl_lzw(program, "lzw_fpga", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: Failed to create kernel ('lzw_fpga') (" << err << ")." << std::endl;

        // Check if kernel names are correct
        std::vector<cl::Kernel> all_kernels;
        program.createKernels(&all_kernels);
        std::cout << "DEBUG: Available kernels in the program:" << std::endl;
        for (size_t i = 0; i < all_kernels.size(); ++i) {
            std::string kernel_name = all_kernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>(&err);
            CHECK_ERROR(err, "Getting kernel function name");
            std::cout << " - " << kernel_name << std::endl;
        }
        return EXIT_FAILURE;
    } else {
        std::cout << "DEBUG: Kernel created successfully." << std::endl;
    }

    // ------------------------------------------------------------------------------------
    // Step 2: Create buffers and initialize data
    // ------------------------------------------------------------------------------------
    std::cout << "DEBUG: Creating buffers." << std::endl;
    size_t input_size_bytes = input_size * sizeof(char); // Use actual input size
    size_t output_size_bytes = OUTPUT_SIZE * sizeof(int);
    size_t decoded_size_bytes = INPUT_SIZE * sizeof(char);

    cl::Buffer input_buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, input_size_bytes, NULL, &err);
    CHECK_ERROR(err, "Creating input buffer");

    cl::Buffer output_code_buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, output_size_bytes, NULL, &err);
    CHECK_ERROR(err, "Creating output code buffer");

    cl::Buffer decoded_buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, decoded_size_bytes, NULL, &err);
    CHECK_ERROR(err, "Creating decoded buffer");

    cl::Buffer output_size_buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int), NULL, &err);
    CHECK_ERROR(err, "Creating output size buffer");

    cl::Buffer decoded_length_buffer(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(int), NULL, &err);
    CHECK_ERROR(err, "Creating decoded length buffer");

    // ------------------------------------------------------------------------------------
    // Step 3: Write input data to device memory
    // ------------------------------------------------------------------------------------
    std::cout << "DEBUG: Writing input data to device memory." << std::endl;
    err = q.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, input_size_bytes, Input);
    CHECK_ERROR(err, "Writing input data to buffer");

    // ------------------------------------------------------------------------------------
    // Step 4: Set kernel arguments
    // ------------------------------------------------------------------------------------
    std::cout << "DEBUG: Setting kernel arguments." << std::endl;
    err = krnl_lzw.setArg(0, input_buffer);
    CHECK_ERROR(err, "Setting kernel argument 0");

    err = krnl_lzw.setArg(1, input_size);
    CHECK_ERROR(err, "Setting kernel argument 1");

    err = krnl_lzw.setArg(2, output_code_buffer);
    CHECK_ERROR(err, "Setting kernel argument 2");

    err = krnl_lzw.setArg(3, output_size_buffer);
    CHECK_ERROR(err, "Setting kernel argument 3");

    err = krnl_lzw.setArg(4, decoded_buffer);
    CHECK_ERROR(err, "Setting kernel argument 4");

    err = krnl_lzw.setArg(5, decoded_length_buffer);
    CHECK_ERROR(err, "Setting kernel argument 5");

    // ------------------------------------------------------------------------------------
    // Step 5: Run the kernel with explicit memory transfers and events
    // ------------------------------------------------------------------------------------
    std::cout << "DEBUG: Running the kernel with explicit memory transfers." << std::endl;

    // Create events
    cl::Event write_event, kernel_event, read_event;

    // Enqueue the kernel
    err = q.enqueueTask(krnl_lzw, NULL, &kernel_event);
    CHECK_ERROR(err, "Enqueueing kernel task");

    // Wait for kernel execution to finish
    std::cout << "DEBUG: Waiting for kernel to finish execution." << std::endl;
    kernel_event.wait();
    std::cout << "DEBUG: Kernel execution finished." << std::endl;

    // ------------------------------------------------------------------------------------
    // Step 6: Read back the results
    // ------------------------------------------------------------------------------------
    std::cout << "DEBUG: Reading back the results." << std::endl;

    err = q.enqueueReadBuffer(output_code_buffer, CL_TRUE, 0, output_size_bytes, Output_HW);
    CHECK_ERROR(err, "Reading output code buffer");

    err = q.enqueueReadBuffer(decoded_buffer, CL_TRUE, 0, decoded_size_bytes, decoded_hw);
    CHECK_ERROR(err, "Reading decoded buffer");

    err = q.enqueueReadBuffer(output_size_buffer, CL_TRUE, 0, sizeof(int), &output_size_hw);
    CHECK_ERROR(err, "Reading output size buffer");

    err = q.enqueueReadBuffer(decoded_length_buffer, CL_TRUE, 0, sizeof(int), &output_length_hw);
    CHECK_ERROR(err, "Reading decoded length buffer");

    // ------------------------------------------------------------------------------------
    // Step 7: Verify the results
    // ------------------------------------------------------------------------------------
    std::cout << "Hardware Encoded Output: ";
    for (int i = 0; i < output_size_hw; ++i) {
        std::cout << Output_HW[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Hardware Decoded Output: ";
    for (int i = 0; i < output_length_hw; ++i) {
        std::cout << decoded_hw[i];
    }
    std::cout << std::endl;

    if (strncmp(Input, decoded_hw, output_length_hw) == 0) {
        std::cout << "HARDWARE TEST PASSED: Decoded output matches the input." << std::endl;
    } else {
        std::cout << "HARDWARE TEST FAILED: Decoded output does not match the input." << std::endl;
    }

    // Clean up resources
    delete[] fileBuf;
    std::cout << "DEBUG: Program completed successfully." << std::endl;
    return 0;
}
