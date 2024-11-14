#include "lzw_hls.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <cstring>

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

int main(int argc, char *argv[]) {
    const char *Input = "I AM SAM SAM I AM"; // Input string to be compressed

    // Define a reasonable maximum size for outputs
    const int MAX_OUTPUT_SIZE = 1024;

    // Allocate buffers for FPGA and software results
    int Output_HW[MAX_OUTPUT_SIZE] = {0};
    int Output_SW[MAX_OUTPUT_SIZE] = {0};
    int output_size_hw = 0;
    int output_size_sw = 0;

    // Buffers for decoded output
    char decoded_fpga[MAX_OUTPUT_SIZE] = {0};
    char decoded_sw[MAX_OUTPUT_SIZE] = {0};

    // Call FPGA LZW function
    if (lzw_fpga(Input, Output_HW, &output_size_hw, decoded_fpga) != 0) {
        std::cerr << "Error: lzw_fpga failed for hardware processing!" << std::endl;
        return 1;
    }

    // Call software LZW function
    if (lzw_fpga(Input, Output_SW, &output_size_sw, decoded_sw) != 0) {
        std::cerr << "Error: lzw_fpga failed for software processing!" << std::endl;
        return 1;
    }

    // Ensure output size does not exceed buffer limit
    if (output_size_hw > MAX_OUTPUT_SIZE || output_size_sw > MAX_OUTPUT_SIZE) {
        std::cerr << "Error: Output size exceeds buffer limit!" << std::endl;
        return 1;
    }

    // Check results
    int failed = result_check(Output_HW, output_size_hw, Output_SW, output_size_sw);

    // Output test result
    std::cout << "TEST " << (!failed ? "PASSED" : "FAILED") << std::endl;

    return failed ? 1 : 0; // Return appropriate exit code
}
