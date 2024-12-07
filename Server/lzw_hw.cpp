#define INPUT_SIZE 256   // Maximum input size
#define OUTPUT_SIZE 512  // Maximum output size

// // Error-checking macro with debugging messages
// inline void CHECK_ERROR(cl_int err, const char* msg) {
//     if (err != CL_SUCCESS) {
//         std::cerr << "ERROR: " << msg << " (" << err << ")" << std::endl;
//         exit(EXIT_FAILURE);
//     } else {
//         std::cout << "DEBUG: " << msg << " succeeded." << std::endl;
//     }
// }

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


