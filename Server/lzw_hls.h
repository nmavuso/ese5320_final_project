#ifndef LZW_HLS_H
#define LZW_HLS_H

#include <hls_stream.h>

#define INPUT_SIZE     256   // Maximum input size
#define TABLE_SIZE     512   // Maximum dictionary size
#define MAX_STACK_SIZE 16    // Maximum depth for decoding

typedef struct {
    int  prefix_code;
    char character;
} DictionaryEntry;


void encoding(
    hls::stream<char> &input_stream,
    int input_size,
    hls::stream<int> &code_stream,
    int &local_output_size,
    int local_output_code[INPUT_SIZE]
);


void decoding(
    hls::stream<int> &code_stream,
    int encoded_size,
    hls::stream<char> &output_stream
);


void load_input(
    const char input[INPUT_SIZE],
    int input_size,
    hls::stream<char> &input_stream
);


void store_output(
    hls::stream<char> &output_stream,
    char local_output[INPUT_SIZE],
    int &local_output_length
);


void copy_output_code(
    int local_output_code[INPUT_SIZE],
    int output_code[INPUT_SIZE],
    int output_size
);


void copy_output(
    char local_output[INPUT_SIZE],
    char output[INPUT_SIZE],
    int output_length
);

void lzw_fpga(
    const char input[INPUT_SIZE],
    int input_size,
    int output_code[INPUT_SIZE],
    int *output_size,
    char output[INPUT_SIZE],
    int *output_length
);

#endif // LZW_HLS_H
