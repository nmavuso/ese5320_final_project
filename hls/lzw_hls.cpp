#include <string.h>
#include <limits.h>
#include "lzw_hls.h"

// Utility Functions
void string_copy(char *dest, const char *src, int max_len) {
    int i = 0;
    for (i = 0; i < max_len - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

int string_compare(const char *s1, const char *s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return s1[i] - s2[i];
        }
        i++;
    }
    return s1[i] - s2[i];
}

int string_length(const char *str) {
    int len = 0;
    while (str[len] != '\0' && len < INPUT_SIZE) {
        len++;
    }
    return len;
}

// LZW Encoding Function
void encoding(const char *input, int *output_code, int *output_size) {
    DictionaryEntry table[TABLE_SIZE];
    int table_size = CHAR_MAX + 1; // Start after single-char entries
    int code = CHAR_MAX + 1;

    // Initialize the dictionary with single characters
    for (int i = 0; i <= CHAR_MAX; i++) {
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
        table[i].code = i;
    }

    char p[INPUT_SIZE] = {input[0], '\0'};
    int out_index = 0;

    for (int i = 1; input[i] != '\0'; i++) {
        char c = input[i];
        char temp[INPUT_SIZE];
        int p_len = string_length(p);

        // Combine p and c into temp
        string_copy(temp, p, INPUT_SIZE);
        temp[p_len] = c;
        temp[p_len + 1] = '\0';

        // Search dictionary for temp
        int found = -1;
        for (int j = 0; j < table_size; j++) {
            if (string_compare(temp, table[j].str) == 0) {
                found = j;
                break;
            }
        }

        if (found != -1) {
            // Update p to temp
            string_copy(p, temp, INPUT_SIZE);
        } else {
            // Output code for p
            for (int j = 0; j < table_size; j++) {
                if (string_compare(p, table[j].str) == 0) {
                    output_code[out_index++] = table[j].code;
                    break;
                }
            }

            // Add temp to dictionary if space is available
            if (table_size < TABLE_SIZE) {
                string_copy(table[table_size].str, temp, INPUT_SIZE);
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
        if (string_compare(p, table[j].str) == 0) {
            output_code[out_index++] = table[j].code;
            break;
        }
    }

    *output_size = out_index;
}

// LZW Decoding Function
void decoding(const int *encoded_data, int encoded_size, char *output) {
    DictionaryEntry table[TABLE_SIZE];
    int table_size = CHAR_MAX + 1;
    int code = CHAR_MAX + 1;

    // Initialize the dictionary with single characters
    for (int i = 0; i <= CHAR_MAX; i++) {
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
        table[i].code = i;
    }

    char temp[INPUT_SIZE] = "";
    int out_index = 0;

    // Handle the first code
    int current_code = encoded_data[0];
    if (current_code >= 0 && current_code < table_size) {
        string_copy(temp, table[current_code].str, INPUT_SIZE);
        string_copy(output + out_index, temp, INPUT_SIZE - out_index);
        out_index += string_length(temp);
    } else {
        return;
    }

    // Process the rest of the codes
    for (int i = 1; i < encoded_size; i++) {
        char new_str[INPUT_SIZE] = {0};
        current_code = encoded_data[i];

        if (current_code < table_size && current_code >= 0) {
            string_copy(new_str, table[current_code].str, INPUT_SIZE);
        } else if (current_code == table_size) {
            int temp_len = string_length(temp);
            string_copy(new_str, temp, INPUT_SIZE);
            new_str[temp_len] = temp[0];
            new_str[temp_len + 1] = '\0';
        } else {
            return;
        }

        if (out_index + string_length(new_str) >= INPUT_SIZE - 1) {
            return;
        }

        string_copy(output + out_index, new_str, INPUT_SIZE - out_index);
        out_index += string_length(new_str);

        if (table_size < TABLE_SIZE) {
            int temp_len = string_length(temp);
            string_copy(table[table_size].str, temp, INPUT_SIZE);
            table[table_size].str[temp_len] = new_str[0];
            table[table_size].str[temp_len + 1] = '\0';
            table[table_size].code = code++;
            table_size++;
        }

        string_copy(temp, new_str, INPUT_SIZE);
    }

    output[out_index] = '\0';
}

// Unified LZW Function
void lzw_fpga(const char *s, int *output_code, int *output_size, char *output) {
    encoding(s, output_code, output_size);
    decoding(output_code, *output_size, output);
}
