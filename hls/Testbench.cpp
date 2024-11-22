#include "lzw_hls.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <cstring>
//#include <climits>

#define INPUT_SIZE 1024
#define TABLE_SIZE 4096 // Adjust as needed for your use case



int encoding_sw(const char *s, int *output_code, int *output_size) {
    if (!s || !output_code || !output_size) {
        fprintf(stderr, "Invalid input to encoding function\n");
        return -1;
    }

    DictionaryEntry table[TABLE_SIZE];
    int code = CHAR_MAX + 1;
    int table_size = CHAR_MAX + 1;

    // Initialize dictionary with single-character strings
    for (int i = 0; i < CHAR_MAX; i++) {
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
        table[i].code = i;
    }

    char p[INPUT_SIZE] = {s[0], '\0'};
    int out_index = 0;

    for (int i = 1; s[i] != '\0'; i++) {
        char c[2] = {s[i], '\0'};
        char temp[INPUT_SIZE];
        snprintf(temp, sizeof(temp), "%s%s", p, c);

        int found = -1;
        for (int j = 0; j < table_size; j++) {
            if (strcmp(table[j].str, temp) == 0) {
                found = j;
                break;
            }
        }

        if (found != -1) {
            strcpy(p, temp);
        } else {
            // Output the code for the current substring
            for (int j = 0; j < table_size; j++) {
                if (strcmp(table[j].str, p) == 0) {
                    output_code[out_index++] = table[j].code;
                    break;
                }
            }

            // Add the new substring to the dictionary
            if (table_size < TABLE_SIZE) {
                strncpy(table[table_size].str, temp, sizeof(table[table_size].str) - 1);
                table[table_size].str[sizeof(table[table_size].str) - 1] = '\0'; // Null-terminate
                table[table_size].code = code++;
                table_size++;
            }
            strcpy(p, c);
        }
    }

    // Output the code for the last substring
    for (int j = 0; j < table_size; j++) {
        if (strcmp(table[j].str, p) == 0) {
            output_code[out_index++] = table[j].code;
            break;
        }
    }

    *output_size = out_index;
    return 0;
}

int decoding_sw(const int *input_code, int input_size, char *output) {
    if (!input_code || !output || input_size <= 0) {
        fprintf(stderr, "Invalid input to decoding function\n");
        return -1;
    }

    DictionaryEntry table[TABLE_SIZE];
    int table_size = CHAR_MAX + 1;
    int code = CHAR_MAX + 1;

    // Initialize the dictionary with single-character strings
    for (int i = 0; i < CHAR_MAX; i++) {
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
        table[i].code = i;
    }

    char temp[INPUT_SIZE] = "";
    int out_index = 0;

    // Decode the first code
    if (input_code[0] >= table_size || input_code[0] < 0) {
        fprintf(stderr, "Invalid code in input stream\n");
        return -1;
    }

    strcpy(temp, table[input_code[0]].str);
    strcpy(output, temp);
    out_index = strlen(output);

    for (int i = 1; i < input_size; i++) {
        char new_str[INPUT_SIZE] = {0};
        int current_code = input_code[i];

        if (current_code < table_size && current_code >= 0) {
            strcpy(new_str, table[current_code].str);
        } else if (current_code == table_size) {
            snprintf(new_str, sizeof(new_str), "%s%c", temp, temp[0]);
        } else {
            fprintf(stderr, "Invalid code in input stream at position %d\n", i);
            return -1;
        }

        if (out_index + (int)strlen(new_str) >= INPUT_SIZE - 1) {
            fprintf(stderr, "Output buffer overflow\n");
            return -1;
        }

        strcat(output, new_str);
        out_index += strlen(new_str);

        if (table_size < TABLE_SIZE) {
            snprintf(table[table_size].str, sizeof(table[table_size].str), "%s%c", temp, new_str[0]);
            table[table_size].str[sizeof(table[table_size].str) - 1] = '\0'; // Null-terminate
            table[table_size].code = code++;
            table_size++;
        }

        strcpy(temp, new_str);
    }

    output[out_index] = '\0';
    return 0;
}

void lzw_sw(const char *s, int *output_code, int *output_size, char *output) {
    encoding_sw(s, output_code, output_size);
    decoding_sw(output_code, *output_size, output);
}

int main() {
    const char *Input = "I AM SAM SAM I AM";

    const int MAX_OUTPUT_SIZE = 1024;
    int Output_SW[MAX_OUTPUT_SIZE] = {0};
    int output_size_sw = 0;
    int Output_HW[MAX_OUTPUT_SIZE] = {0};
    int output_size_hw = 0;

    char decoded_sw[MAX_OUTPUT_SIZE] = {0};
    char decoded_hw[MAX_OUTPUT_SIZE] = {0};
    lzw_sw(Input, Output_SW, &output_size_sw, decoded_sw);
    lzw_fpga(Input, Output_HW, &output_size_hw, decoded_hw);

    // Print the encoded and decoded results
    std::cout << "Encoded Output: ";
    for (int i = 0; i < output_size_sw; ++i) {
        std::cout << Output_SW[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Decoded Output: " << decoded_sw << std::endl;


    // Print the encoded and decoded results
       std::cout << "Encoded Output: ";
       for (int i = 0; i < output_size_hw; ++i) {
           std::cout << Output_HW[i] << " ";
       }
       std::cout << std::endl;

       std::cout << "Decoded Output: " << decoded_hw << std::endl;

    if (strcmp(Input, decoded_hw) == 0) {
            std::cout << "TEST PASSED: Decoded output matches the input." << std::endl;
        } else {
            std::cout << "TEST FAILED: Decoded output does not match the input." << std::endl;
        }

        if (strcmp(decoded_sw, decoded_hw) == 0) {
            std::cout << "TEST PASSED: Decoded output matches the golden standard." << std::endl;
        } else {
            std::cout << "TEST FAILED: Decoded output does not match the golden standard." << std::endl;
        }
    return 0;
}
