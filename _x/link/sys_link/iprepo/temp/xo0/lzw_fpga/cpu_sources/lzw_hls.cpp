//#include <stdio.h>
//#include <string.h>
//#include "lzw_hls.h"
//
//
//// Encoding Function
//void encoding(const char *input, int *output_code, int *output_size) {
//    DictionaryEntry table[TABLE_SIZE];
//    int table_size = CHAR_MAX + 1;
//    int code = CHAR_MAX + 1;
//
//    // Optimize memory access by partitioning
//    #pragma HLS array_partition variable=table cyclic factor=16 dim=1
//
//    // Initialize dictionary with single characters
//    for (int i = 0; i <= CHAR_MAX; i++) {
//        #pragma HLS unroll factor=16
//        table[i].str[0] = (char)i;
//        table[i].str[1] = '\0';
//        table[i].code = i;
//    }
//
//    char p[INPUT_SIZE] = {input[0], '\0'};
//    int out_index = 0;
//
//    for (int i = 1; input[i] != '\0'; i++) {
//        #pragma HLS pipeline II=1
//        char c = input[i];
//        char temp[INPUT_SIZE];
//        int p_len = 0;
//
//        // Combine p and c into temp
//        while (p[p_len] != '\0') {
//            temp[p_len] = p[p_len];
//            p_len++;
//        }
//        temp[p_len] = c;
//        temp[p_len + 1] = '\0';
//
//        // Search dictionary for temp
//        int found = -1;
//        for (int j = 0; j < table_size; j++) {
//            #pragma HLS pipeline II=1
//            int k = 0;
//            while (temp[k] != '\0' && table[j].str[k] == temp[k]) {
//                k++;
//            }
//            if (temp[k] == '\0' && table[j].str[k] == '\0') {
//                found = j;
//                break;
//            }
//        }
//
//        if (found != -1) {
//            // Update p to temp
//            int k = 0;
//            while ((p[k] = temp[k]) != '\0') {
//                k++;
//            }
//        } else {
//            // Output code for p
//            for (int j = 0; j < table_size; j++) {
//                #pragma HLS pipeline II=1
//                int k = 0;
//                while (p[k] != '\0' && table[j].str[k] == p[k]) {
//                    k++;
//                }
//                if (p[k] == '\0' && table[j].str[k] == '\0') {
//                    output_code[out_index++] = table[j].code;
//                    break;
//                }
//            }
//
//            // Add temp to dictionary if space available
//            if (table_size < TABLE_SIZE) {
//                int k = 0;
//                while ((table[table_size].str[k] = temp[k]) != '\0') {
//                    k++;
//                }
//                table[table_size].code = code++;
//                table_size++;
//            }
//
//            // Update p to c
//            p[0] = c;
//            p[1] = '\0';
//        }
//    }
//
//    // Output code for the last sequence in p
//    for (int j = 0; j < table_size; j++) {
//        int k = 0;
//        while (p[k] != '\0' && table[j].str[k] == p[k]) {
//            k++;
//        }
//        if (p[k] == '\0' && table[j].str[k] == '\0') {
//            output_code[out_index++] = table[j].code;
//            break;
//        }
//    }
//
//    *output_size = out_index;
//}
//
//// Decoding Function
//void decoding(const int *encoded_data, int encoded_size, char *output) {
//    DictionaryEntry table[TABLE_SIZE];
//    int table_size = CHAR_MAX + 1;
//
//    #pragma HLS array_partition variable=table cyclic factor=16 dim=1
//
//    // Initialize dictionary with single characters
//    for (int i = 0; i <= CHAR_MAX; i++) {
//        #pragma HLS unroll factor=16
//        table[i].str[0] = (char)i;
//        table[i].str[1] = '\0';
//        table[i].code = i;
//    }
//
//    int old_code = encoded_data[0];
//    int output_index = 0;
//
//    // Output the first character
//    int k = 0;
//    while ((output[output_index++] = table[old_code].str[k++]) != '\0') {
//    }
//    output_index--; // Remove null terminator
//    char c = table[old_code].str[0];
//
//    for (int i = 1; i < encoded_size; i++) {
//        int new_code = encoded_data[i];
//        char entry[INPUT_SIZE];
//
//        // Handle the special case where new_code is not yet in the dictionary
//        if (new_code >= table_size) {
//            k = 0;
//            while ((entry[k] = table[old_code].str[k]) != '\0') {
//                k++;
//            }
//            entry[k++] = c;
//            entry[k] = '\0';
//        } else {
//            k = 0;
//            while ((entry[k] = table[new_code].str[k]) != '\0') {
//                k++;
//            }
//        }
//
//        // Append entry to the output
//        k = 0;
//        while (entry[k] != '\0') {
//            output[output_index++] = entry[k++];
//        }
//
//        // Add new entry to the dictionary if space is available
//        if (table_size < TABLE_SIZE) {
//            k = 0;
//            int old_len = 0;
//            while ((table[table_size].str[k] = table[old_code].str[k]) != '\0') {
//                k++;
//            }
//            table[table_size].str[k] = entry[0];
//            table[table_size].str[k + 1] = '\0';
//            table[table_size].code = table_size;
//            table_size++;
//        }
//
//        // Update old_code and c
//        old_code = new_code;
//        c = entry[0];
//    }
//
//    output[output_index] = '\0'; // Null-terminate the output string
//}
//
//// Unified LZW Function
//void lzw_fpga(const char *s, int *output_code, int *output_size, char *output) {
//    encoding(s, output_code, output_size);
//    const int *encoded_data = output_code;
//    decoding(encoded_data, *output_size, output);
//}

//#include <hls_stream.h>
//#include <string.h>
//#include "lzw_hls.h"
//
//// Load Function: Prepares input data and initializes the dictionary
//void load(const char *input, hls::stream<char> &input_stream,
//          hls::stream<DictionaryEntry> &dictionary_stream) {
//    #pragma HLS INLINE
//    DictionaryEntry table[TABLE_SIZE];
//    #pragma HLS ARRAY_PARTITION variable=table cyclic factor=16 dim=1
//
//    // Initialize dictionary with single characters
//    for (int i = 0; i <= CHAR_MAX; i++) {
//        #pragma HLS UNROLL factor=16
//        table[i].str[0] = (char)i;
//        table[i].str[1] = '\0';
//        table[i].code = i;
//    }
//
//    // Stream dictionary entries
//    for (int i = 0; i <= CHAR_MAX; i++) {
//        dictionary_stream.write(table[i]);
//    }
//
//    // Stream input characters
//    for (int i = 0; input[i] != '\0'; i++) {
//        input_stream.write(input[i]);
//    }
//}
//
//// Compute Function: Encodes the input into LZW codes
//void compute(hls::stream<char> &input_stream, hls::stream<DictionaryEntry> &dictionary_stream,
//             hls::stream<int> &encoded_stream) {
//    #pragma HLS INLINE
//
//    DictionaryEntry table[TABLE_SIZE];
//    #pragma HLS ARRAY_PARTITION variable=table cyclic factor=16 dim=1
//    int table_size = CHAR_MAX + 1;
//    int code = CHAR_MAX + 1;
//
//    // Load dictionary into local memory
//    for (int i = 0; i <= CHAR_MAX; i++) {
//        #pragma HLS PIPELINE
//        table[i] = dictionary_stream.read();
//    }
//
//    char p[INPUT_SIZE] = {0};  // Initialize with an empty string
//    char c;
//
//    // Initialize the first character of p
//    if (!input_stream.empty()) {
//        p[0] = input_stream.read();
//        p[1] = '\0';
//    }
//
//    while (!input_stream.empty()) {
//        #pragma HLS PIPELINE II=1
//
//        // Read the next character
//        c = input_stream.read();
//        char temp[INPUT_SIZE];
//        int p_len = 0;
//
//        // Combine p and c into temp, ensuring no overflow
//        while (p[p_len] != '\0' && p_len < INPUT_SIZE - 1) {
//            temp[p_len] = p[p_len];
//            p_len++;
//        }
//        temp[p_len] = c;
//        temp[p_len + 1] = '\0';
//
//        // Search dictionary for temp
//        int found = -1;
//        for (int j = 0; j < table_size; j++) {
//            #pragma HLS PIPELINE II=1
//            int k = 0;
//            while (temp[k] != '\0' && table[j].str[k] == temp[k]) {
//                k++;
//            }
//            if (temp[k] == '\0' && table[j].str[k] == '\0') {
//                found = j;
//                break;
//            }
//        }
//
//        if (found != -1) {
//            // Update p to temp
//            for (int i = 0; i < INPUT_SIZE; i++) {
//                #pragma HLS UNROLL
//                p[i] = temp[i];
//                if (temp[i] == '\0') break;
//            }
//        } else {
//            // Output code for p
//            for (int j = 0; j < table_size; j++) {
//                #pragma HLS PIPELINE II=1
//                int k = 0;
//                while (p[k] != '\0' && table[j].str[k] == p[k]) {
//                    k++;
//                }
//                if (p[k] == '\0' && table[j].str[k] == '\0') {
//                    encoded_stream.write(table[j].code);
//                    break;
//                }
//            }
//
//            // Add temp to dictionary if space available
//            if (table_size < TABLE_SIZE) {
//                for (int i = 0; i < INPUT_SIZE; i++) {
//                    #pragma HLS UNROLL
//                    table[table_size].str[i] = temp[i];
//                    if (temp[i] == '\0') break;
//                }
//                table[table_size].code = code++;
//                table_size++;
//            }
//
//            // Update p to c
//            p[0] = c;
//            p[1] = '\0';
//        }
//    }
//
//    // Output the last sequence in p
//    for (int j = 0; j < table_size; j++) {
//        #pragma HLS PIPELINE II=1
//        int k = 0;
//        while (p[k] != '\0' && table[j].str[k] == p[k]) {
//            k++;
//        }
//        if (p[k] == '\0' && table[j].str[k] == '\0') {
//            encoded_stream.write(table[j].code);
//            break;
//        }
//    }
//}
//
//
//// Store Function: Outputs the encoded data
//void store(hls::stream<int> &encoded_stream, int *output_code, int *output_size) {
//    #pragma HLS INLINE
//    int index = 0;
//    while (!encoded_stream.empty()) {
//        #pragma HLS PIPELINE
//        output_code[index++] = encoded_stream.read();
//    }
//    *output_size = index;
//}
//
//// Unified LZW Function
//void lzw_fpga(const char *input, int *output_code, int *output_size) {
//    #pragma HLS INTERFACE m_axi depth=1024 port=input bundle=aximm1
//    #pragma HLS INTERFACE m_axi depth=1024 port=output_code bundle=aximm2
//    #pragma HLS INTERFACE m_axi depth=1024 port=output_size bundle=aximm3
//
//    hls::stream<char> input_stream;
//    hls::stream<DictionaryEntry> dictionary_stream;
//    hls::stream<int> encoded_stream;
//
//    #pragma HLS STREAM variable=input_stream depth=64
//    #pragma HLS STREAM variable=dictionary_stream depth=64
//    #pragma HLS STREAM variable=encoded_stream depth=64
//
//    #pragma HLS DATAFLOW
//    load(input, input_stream, dictionary_stream);
//    compute(input_stream, dictionary_stream, encoded_stream);
//    store(encoded_stream, output_code, output_size);
//}


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
    encoding(s, output_code, output_size);
    decoding(output_code, *output_size, output);
}


