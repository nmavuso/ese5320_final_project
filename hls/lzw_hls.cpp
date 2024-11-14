#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 4096  // Dictionary size
#define INPUT_SIZE 1024  // Maximum input size
#define CHAR_MAX 255     // Maximum single-character value

typedef struct {
    char str[INPUT_SIZE];
    int code;
} DictionaryEntry;

// Encoding Function
void encoding(const char *input, int *output_code, int *output_size) {
    DictionaryEntry table[TABLE_SIZE];
    int table_size = CHAR_MAX + 1;
    int code = CHAR_MAX + 1;

    // Optimize memory access by partitioning
    #pragma HLS array_partition variable=table cyclic factor=16 dim=1

    // Initialize dictionary with single characters
    for (int i = 0; i <= CHAR_MAX; i++) {
        #pragma HLS unroll factor=16
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
        table[i].code = i;
    }

    char p[INPUT_SIZE] = {input[0], '\0'};
    int out_index = 0;

    for (int i = 1; input[i] != '\0'; i++) {
        #pragma HLS pipeline II=1
        char c = input[i];
        char temp[INPUT_SIZE];
        int p_len = 0;

        // Combine p and c into temp
        while (p[p_len] != '\0') {
            temp[p_len] = p[p_len];
            p_len++;
        }
        temp[p_len] = c;
        temp[p_len + 1] = '\0';

        // Search dictionary for temp
        int found = -1;
        for (int j = 0; j < table_size; j++) {
            #pragma HLS pipeline II=1
            int k = 0;
            while (temp[k] != '\0' && table[j].str[k] == temp[k]) {
                k++;
            }
            if (temp[k] == '\0' && table[j].str[k] == '\0') {
                found = j;
                break;
            }
        }

        if (found != -1) {
            // Update p to temp
            int k = 0;
            while ((p[k] = temp[k]) != '\0') {
                k++;
            }
        } else {
            // Output code for p
            for (int j = 0; j < table_size; j++) {
                #pragma HLS pipeline II=1
                int k = 0;
                while (p[k] != '\0' && table[j].str[k] == p[k]) {
                    k++;
                }
                if (p[k] == '\0' && table[j].str[k] == '\0') {
                    output_code[out_index++] = table[j].code;
                    break;
                }
            }

            // Add temp to dictionary if space available
            if (table_size < TABLE_SIZE) {
                int k = 0;
                while ((table[table_size].str[k] = temp[k]) != '\0') {
                    k++;
                }
                table[table_size].code = code++;
                table_size++;
            }

            // Update p to c
            p[0] = c;
            p[1] = '\0';
        }
    }

    // Output code for the last sequence in p
    for (int j = 0; j < table_size; j++) {
        int k = 0;
        while (p[k] != '\0' && table[j].str[k] == p[k]) {
            k++;
        }
        if (p[k] == '\0' && table[j].str[k] == '\0') {
            output_code[out_index++] = table[j].code;
            break;
        }
    }

    *output_size = out_index;
}

// Decoding Function
void decoding(const int *encoded_data, int encoded_size, char *output) {
    DictionaryEntry table[TABLE_SIZE];
    int table_size = CHAR_MAX + 1;

    #pragma HLS array_partition variable=table cyclic factor=16 dim=1

    // Initialize dictionary with single characters
    for (int i = 0; i <= CHAR_MAX; i++) {
        #pragma HLS unroll factor=16
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
        table[i].code = i;
    }

    int old_code = encoded_data[0];
    int output_index = 0;

    // Output the first character
    int k = 0;
    while ((output[output_index++] = table[old_code].str[k++]) != '\0') {
    }
    output_index--; // Remove null terminator
    char c = table[old_code].str[0];

    for (int i = 1; i < encoded_size; i++) {
        int new_code = encoded_data[i];
        char entry[INPUT_SIZE];

        // Handle the special case where new_code is not yet in the dictionary
        if (new_code >= table_size) {
            k = 0;
            while ((entry[k] = table[old_code].str[k]) != '\0') {
                k++;
            }
            entry[k++] = c;
            entry[k] = '\0';
        } else {
            k = 0;
            while ((entry[k] = table[new_code].str[k]) != '\0') {
                k++;
            }
        }

        // Append entry to the output
        k = 0;
        while (entry[k] != '\0') {
            output[output_index++] = entry[k++];
        }

        // Add new entry to the dictionary if space is available
        if (table_size < TABLE_SIZE) {
            k = 0;
            int old_len = 0;
            while ((table[table_size].str[k] = table[old_code].str[k]) != '\0') {
                k++;
            }
            table[table_size].str[k] = entry[0];
            table[table_size].str[k + 1] = '\0';
            table[table_size].code = table_size;
            table_size++;
        }

        // Update old_code and c
        old_code = new_code;
        c = entry[0];
    }

    output[output_index] = '\0'; // Null-terminate the output string
}

// Unified LZW Function
void lzw_fpga(const char *s, int *output_code, int *output_size, const int *encoded_data, int encoded_size, char *output) {
    encoding(s, output_code, output_size);
    decoding(encoded_data, encoded_size, output);
}
