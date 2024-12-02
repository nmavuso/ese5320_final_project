#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl2.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <chrono>
#include <cstdlib>
#include <memory>

#include "lzw_hls.h"
#include "Utilities.h"
#include "EventTimer.h"

// Error checking helper
inline void CHECK_ERROR(cl_int err, const char* msg) {
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: " << msg << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "DEBUG: " << msg << " succeeded." << std::endl;
    }
}

// Kernel execution time
void print_execution_time(cl::Event event, const char* operation_name) {
    cl_ulong start, end;
    event.getProfilingInfo(CL_PROFILING_COMMAND_START, &start);
    event.getProfilingInfo(CL_PROFILING_COMMAND_END, &end);
    double elapsed_time = (end - start) / 1e6; // Convert to milliseconds
    std::cout << "DEBUG: " << operation_name << " took " << elapsed_time << " ms." << std::endl;
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
    EventTimer timer1, timer2;
    timer1.add("Main program");

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    std::cout << "DEBUG: XCLBIN file path: " << binaryFile << std::endl;

    const char Input[] = "WYS*WYGWYS*WYSWYSG";
    int input_size = strlen(Input);
    if (input_size == 0) {
        std::cerr << "ERROR: Input string is empty!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "DEBUG: Input string: " << Input << std::endl;

    /// OpenCL Setup
    timer2.add("OpenCL Setup");
    cl_int err;
    unsigned fileBufSize;

    std::vector<cl::Device> devices = get_xilinx_devices();
    if (devices.empty()) {
        std::cerr << "ERROR: No Xilinx devices found." << std::endl;
        return EXIT_FAILURE;
    }

    cl::Device device = devices[0];
    std::cout << "DEBUG: Using device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;

    cl::Context context(device, NULL, NULL, NULL, &err);
    CHECK_ERROR(err, "Creating OpenCL context");

    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    CHECK_ERROR(err, "Creating command queue");

    std::unique_ptr<char[]> fileBuf(read_binary_file(binaryFile, fileBufSize));
    if (!fileBuf) {
        std::cerr << "ERROR: Failed to read XCLBIN file!" << std::endl;
        return EXIT_FAILURE;
    }

    cl::Program::Binaries bins{{fileBuf.get(), fileBufSize}};
    cl::Program program(context, {device}, bins, NULL, &err);
    CHECK_ERROR(err, "Creating program from binary");

    cl::Kernel krnl_lzw(program, "lzw_fpga", &err);
    CHECK_ERROR(err, "Creating kernel");

    // Allocate buffer sizes
    size_t input_size_bytes = input_size * sizeof(char);
    size_t output_size_bytes = input_size * sizeof(int);  // Conservative estimate
    size_t decoded_size_bytes = input_size * sizeof(char);

    // Create buffers
    cl::Buffer input_buffer(context, CL_MEM_READ_ONLY, input_size_bytes, NULL, &err);
    CHECK_ERROR(err, "Creating input buffer");
    cl::Buffer output_code_buffer(context, CL_MEM_WRITE_ONLY, output_size_bytes, NULL, &err);
    CHECK_ERROR(err, "Creating output code buffer");
    cl::Buffer decoded_buffer(context, CL_MEM_WRITE_ONLY, decoded_size_bytes, NULL, &err);
    CHECK_ERROR(err, "Creating decoded buffer");
    cl::Buffer output_size_buffer(context, CL_MEM_WRITE_ONLY, sizeof(int), NULL, &err);
    CHECK_ERROR(err, "Creating output size buffer");
    cl::Buffer decoded_length_buffer(context, CL_MEM_WRITE_ONLY, sizeof(int), NULL, &err);
    CHECK_ERROR(err, "Creating decoded length buffer");

    // Set kernel arguments
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

    // Write input data to device
    err = q.enqueueWriteBuffer(input_buffer, CL_TRUE, 0, input_size_bytes, Input);
    CHECK_ERROR(err, "Writing input buffer");

    // Launch the kernel
    cl::Event exec_ev;
    err = q.enqueueTask(krnl_lzw, NULL, &exec_ev);
    CHECK_ERROR(err, "Enqueueing kernel task");

    // Wait for execution
    exec_ev.wait();
    print_execution_time(exec_ev, "Kernel execution");

    // Read outputs
    int output_size_hw = 0;
    int output_length_hw = 0;
    char decoded_hw[input_size] = {0};
    int Output_HW[input_size] = {0};

    err = q.enqueueReadBuffer(output_size_buffer, CL_TRUE, 0, sizeof(int), &output_size_hw);
    CHECK_ERROR(err, "Reading output size buffer");
    err = q.enqueueReadBuffer(decoded_length_buffer, CL_TRUE, 0, sizeof(int), &output_length_hw);
    CHECK_ERROR(err, "Reading decoded length buffer");
    err = q.enqueueReadBuffer(output_code_buffer, CL_TRUE, 0, output_size_bytes, Output_HW);
    CHECK_ERROR(err, "Reading encoded output buffer");
    err = q.enqueueReadBuffer(decoded_buffer, CL_TRUE, 0, decoded_size_bytes, decoded_hw);
    CHECK_ERROR(err, "Reading decoded output buffer");

    // Print results
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

    // Validation
    if (strncmp(Input, decoded_hw, output_length_hw) == 0) {
        std::cout << "HARDWARE TEST PASSED: Decoded output matches the input." << std::endl;
    } else {
        std::cout << "HARDWARE TEST FAILED: Decoded output does not match the input." << std::endl;
    }

    // Timer output
    timer2.finish();
    timer2.print();
    timer1.finish();
    timer1.print();

    return 0;
}
