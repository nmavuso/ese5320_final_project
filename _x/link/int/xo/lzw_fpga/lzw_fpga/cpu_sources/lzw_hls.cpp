/*********
 *
 * Engineer: Nhlanhla Mavuso
 * Company: University of Pennsylvania
 * Date: 11/29/2024
 * Description: Highly optimized LZW accelerator built using High Level Synthesis
 * Status: Fully tested and functional
 */

#include "lzw_hls.h"
#include <hls_stream.h>

void encoding(
    hls::stream<char> &input_stream,
    int input_size,
    hls::stream<int> &code_stream,
    int &local_output_size,
    int local_output_code[INPUT_SIZE]
) {
	#pragma HLS INLINE off
    DictionaryEntry table[TABLE_SIZE];
	#pragma HLS ARRAY_PARTITION variable=table cyclic factor=4 dim=1

    int table_size = 256;

    init_loop: for (int i = 0; i < 256; i++) {
	#pragma HLS UNROLL factor=4
        table[i].prefix_code = -1;
        table[i].character = (char)i;
    }

    int p = input_stream.read();
    int out_index = 0;

    main_loop: for (int i = 1; i < input_size; i++) {
	#pragma HLS PIPELINE II=1
        char c = input_stream.read();

        int found = -1;
        search_loop: for (int j = 256; j < table_size; j++) {
		#pragma HLS UNROLL factor=4
            if (table[j].prefix_code == p && table[j].character == c) {
                found = j;
                break;
            }
        }

        if (found != -1) {
            p = found;
        } else {
            code_stream.write(p);
            local_output_code[out_index++] = p;

            if (table_size < TABLE_SIZE) {
                table[table_size].prefix_code = p;
                table[table_size].character = c;
                table_size++;
            }

            p = (unsigned char)c;
        }
    }

    code_stream.write(p);
    local_output_code[out_index++] = p;

    local_output_size = out_index;
}

void copy_output_code(int local_output_code[INPUT_SIZE], int output_code[INPUT_SIZE], int output_size) {
    copy_output_code_loop: for (int i = 0; i < output_size; i++) {
		#pragma HLS PIPELINE II=1
        output_code[i] = local_output_code[i];
    }
}

void decoding(hls::stream<int> &code_stream, int encoded_size, hls::stream<char> &output_stream) {
	#pragma HLS INLINE off
    DictionaryEntry table[TABLE_SIZE];
	#pragma HLS ARRAY_PARTITION variable=table cyclic factor=4 dim=1

    int table_size = 256;

    init_loop: for (int i = 0; i < 256; i++) {
	#pragma HLS UNROLL factor=4
        table[i].prefix_code = -1;
        table[i].character = (char)i;
    }

    int prev_code = code_stream.read();

    char stack[MAX_STACK_SIZE];
	#pragma HLS ARRAY_PARTITION variable=stack complete dim=1
    int stack_index = 0;

    int current_code = prev_code;
    build_string_loop: for (int k = 0; k < MAX_STACK_SIZE; k++) {
		#pragma HLS PIPELINE II=2
        if (stack_index < MAX_STACK_SIZE) {
            stack[stack_index++] = table[current_code].character;
            current_code = table[current_code].prefix_code;
            if (current_code == -1) break;
        } else {
            break;
        }
    }


    output_loop1: for (int i = stack_index - 1; i >= 0; i--) {
		#pragma HLS PIPELINE II=1
        output_stream.write(stack[i]);
    }

    decode_main_loop: for (int i = 1; i < encoded_size; i++) {
		#pragma HLS PIPELINE II=1
        int current_code = code_stream.read();
        stack_index = 0;

        int temp_code = current_code;
        bool code_in_table = (current_code < table_size);

        build_string_loop2: for (int k = 0; k < MAX_STACK_SIZE; k++) {
		#pragma HLS PIPELINE II=1
            if (code_in_table) {
                if (stack_index < MAX_STACK_SIZE) {
                    stack[stack_index++] = table[temp_code].character;
                    temp_code = table[temp_code].prefix_code;
                    if (temp_code == -1) break;
                } else {
                    break;
                }
            } else {
                if (stack_index < MAX_STACK_SIZE) {
                    stack[stack_index++] = table[prev_code].character;
                    temp_code = prev_code;
                    code_in_table = true;
                } else {
                    break;
                }
            }
        }

        output_loop2: for (int j = stack_index - 1; j >= 0; j--) {
			#pragma HLS PIPELINE II=1
            output_stream.write(stack[j]);
        }

        if (table_size < TABLE_SIZE) {
            table[table_size].prefix_code = prev_code;
            table[table_size].character = stack[stack_index - 1];
            table_size++;
        }

        prev_code = current_code;
    }
}

void load_input(const char input[INPUT_SIZE], int input_size, hls::stream<char> &input_stream) {
    load_input_loop: for (int i = 0; i < input_size; i++) {
	#pragma HLS PIPELINE II=1
        input_stream.write(input[i]);
    }
}

void store_output(hls::stream<char> &output_stream, char local_output[INPUT_SIZE], int &local_output_length) {
    int out_index = 0;
    store_output_loop: for (int i = 0; i < INPUT_SIZE; i++) {
		#pragma HLS PIPELINE II=1
        if (!output_stream.empty() && out_index < INPUT_SIZE) {
            local_output[out_index++] = output_stream.read();
        } else {
            break;
        }
    }
    local_output_length = out_index;
}

void copy_output(char local_output[INPUT_SIZE], char output[INPUT_SIZE], int output_length) {
    copy_output_loop: for (int i = 0; i < output_length; i++) {
	#pragma HLS PIPELINE II=1
        output[i] = local_output[i];
    }
}


void lzw_fpga(
    const char input[INPUT_SIZE],
    int input_size,
    int output_code[INPUT_SIZE],
    int *output_size,
    char output[INPUT_SIZE],
    int *output_length
) {
	#pragma HLS INTERFACE m_axi port=input        offset=slave bundle=gmem0
	#pragma HLS INTERFACE m_axi port=output_code  offset=slave bundle=gmem1
	#pragma HLS INTERFACE m_axi port=output       offset=slave bundle=gmem2

	#pragma HLS INTERFACE s_axilite port=input_size      bundle=control
	#pragma HLS INTERFACE s_axilite port=output_size     bundle=control
	#pragma HLS INTERFACE s_axilite port=output_length   bundle=control
	#pragma HLS INTERFACE s_axilite port=return          bundle=control

	#pragma HLS INTERFACE s_axilite port=input           bundle=control
	#pragma HLS INTERFACE s_axilite port=output_code     bundle=control
	#pragma HLS INTERFACE s_axilite port=output          bundle=control


    int local_output_code[INPUT_SIZE];
    char local_output[INPUT_SIZE];
    int local_output_size = 0;
    int local_output_length = 0;

	#pragma HLS DATAFLOW

    hls::stream<char> input_stream("input_stream");
    hls::stream<int> code_stream("code_stream");
    hls::stream<char> output_stream("output_stream");

	#pragma HLS STREAM variable=input_stream depth=16
	#pragma HLS STREAM variable=code_stream depth=16
	#pragma HLS STREAM variable=output_stream depth=16

    load_input(input, input_size, input_stream);

    encoding(input_stream, input_size, code_stream, local_output_size, local_output_code);

    decoding(code_stream, local_output_size, output_stream);

    store_output(output_stream, local_output, local_output_length);

    copy_output_code(local_output_code, output_code, local_output_size);

    copy_output(local_output, output, local_output_length);

    *output_size = local_output_size;
    *output_length = local_output_length;
}
