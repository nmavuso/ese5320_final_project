#include <stdio.h>
#include "lzw_hls.h"

//#define TABLE_SIZE 4096  // Maximum dictionary size for LZW
//#define INPUT_SIZE 1024  // Maximum input size
//#define CHAR_MAX 255     // Maximum ASCII value
//
//typedef struct {
//    char str[INPUT_SIZE]; // Stores dictionary entries
//    int code;             // Associated code for the string
//} DictionaryEntry;

// Synthesizable string copy function
void string_copy(char *dest, const char *src, int max_len) {
    int i = 0;
    for (i = 0; i < max_len - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

// Synthesizable string compare function
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

// Synthesizable string length function
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
    int table_size = CHAR_MAX + 1;
    int code = CHAR_MAX + 1;

    // Initialize dictionary with single characters
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

    // Initialize dictionary with single characters
    for (int i = 0; i <= CHAR_MAX; i++) {
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
        table[i].code = i;
    }

    int old_code = encoded_data[0];
    int output_index = 0;

    // Output the first character
    string_copy(output, table[old_code].str, INPUT_SIZE);
    output_index += string_length(table[old_code].str);
    char c = table[old_code].str[0];

    for (int i = 1; i < encoded_size; i++) {
        int new_code = encoded_data[i];
        char entry[INPUT_SIZE];

        // Handle the special case where new_code is not yet in the dictionary
        if (new_code >= table_size) {
            int k = 0;
            while (table[old_code].str[k] != '\0') {
                entry[k] = table[old_code].str[k];
                k++;
            }
            entry[k++] = c;
            entry[k] = '\0';
        } else {
            string_copy(entry, table[new_code].str, INPUT_SIZE);
        }

        // Append entry to the output
        int k = 0;
        while (entry[k] != '\0') {
            output[output_index++] = entry[k++];
        }

        // Add new entry to the dictionary if space is available
        if (table_size < TABLE_SIZE) {
            string_copy(table[table_size].str, table[old_code].str, INPUT_SIZE);
            int old_len = string_length(table[old_code].str);
            table[table_size].str[old_len] = c;
            table[table_size].str[old_len + 1] = '\0';
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
void lzw_fpga(const char *s, int *output_code, int *output_size, char *output) {
    printf("Inside Software LZW\n");
    encoding(s, output_code, output_size);
    decoding(output_code, *output_size, output);
}