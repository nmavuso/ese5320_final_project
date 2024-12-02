// lzw_testbench.cpp

#include "lzw_hls.h"
#include <iostream>
#include <cstring>

// Constants matching the hardware implementation
#define INPUT_SIZE     256   // Maximum input size (should match lzw_hls.h)
#define TABLE_SIZE     512   // Maximum dictionary size (should match lzw_hls.h)
#define MAX_STACK_SIZE 16    // Maximum depth for decoding (should match lzw_hls.h)

// Software LZW Encoding Function (mirrors hardware logic)
void encoding_sw(
    const char input[INPUT_SIZE],
    int input_size,
    int output_code[INPUT_SIZE],
    int &output_size
) {
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

// Software LZW Decoding Function (mirrors hardware logic)
void decoding_sw(
    const int encoded_data[INPUT_SIZE],
    int encoded_size,
    char output[INPUT_SIZE],
    int &output_length
) {
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

// Unified Software LZW Function
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

// Main Testbench Function
int main() {
    // Test Input String
    const char Input[] = "WYS*WYGWYS*WYSWYSG";
    int input_size = strlen(Input);

    // Output Buffers for Software Implementation
    int Output_SW[INPUT_SIZE] = {0};
    int output_size_sw = 0;
    char decoded_sw[INPUT_SIZE] = {0};
    int output_length_sw = 0;

    // Output Buffers for Hardware Implementation
    int Output_HW[INPUT_SIZE] = {0};
    int output_size_hw = 0;
    char decoded_hw[INPUT_SIZE] = {0};
    int output_length_hw = 0;

    // Run Software LZW Implementation
    lzw_sw(Input, input_size, Output_SW, output_size_sw, decoded_sw, output_length_sw);

    // Run Hardware LZW Implementation
    lzw_fpga(Input, input_size, Output_HW, &output_size_hw, decoded_hw, &output_length_hw);

    // Print Software Encoded Output
    std::cout << "Software Encoded Output: ";
    for (int i = 0; i < output_size_sw; ++i) {
        std::cout << Output_SW[i] << " ";
    }
    std::cout << std::endl;

    // Print Software Decoded Output
    std::cout << "Software Decoded Output: ";
    for (int i = 0; i < output_length_sw; ++i) {
        std::cout << decoded_sw[i];
    }
    std::cout << std::endl;

    // Verify Software Decoded Output Matches Input
    if (strncmp(Input, decoded_sw, output_length_sw) == 0) {
        std::cout << "SOFTWARE TEST PASSED: Decoded output matches the input." << std::endl;
    } else {
        std::cout << "SOFTWARE TEST FAILED: Decoded output does not match the input." << std::endl;
    }

    // Print Hardware Encoded Output
    std::cout << "Hardware Encoded Output: ";
    for (int i = 0; i < output_size_hw; ++i) {
        std::cout << Output_HW[i] << " ";
    }
    std::cout << std::endl;

    // Print Hardware Decoded Output
    std::cout << "Hardware Decoded Output: ";
    for (int i = 0; i < output_length_hw; ++i) {
        std::cout << decoded_hw[i];
    }
    std::cout << std::endl;

    // Verify Hardware Decoded Output Matches Input
    if (strncmp(Input, decoded_hw, output_length_hw) == 0) {
        std::cout << "HARDWARE TEST PASSED: Decoded output matches the input." << std::endl;
    } else {
        std::cout << "HARDWARE TEST FAILED: Decoded output does not match the input." << std::endl;
    }

    // Compare Software and Hardware Encoded Outputs
    bool codes_match = (output_size_sw == output_size_hw);
    for (int i = 0; i < output_size_sw && codes_match; ++i) {
        if (Output_SW[i] != Output_HW[i]) {
            codes_match = false;
            break;
        }
    }

    if (codes_match) {
        std::cout << "TEST PASSED: Software and hardware encoded outputs match." << std::endl;
    } else {
        std::cout << "TEST FAILED: Software and hardware encoded outputs do not match." << std::endl;
    }

    return 0;
}
