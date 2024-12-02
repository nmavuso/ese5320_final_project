#define INPUT_SIZE 256   // Maximum input size
#define OUTPUT_SIZE 512  // Maximum output size

// Error-checking macro with debugging messages
inline void CHECK_ERROR(cl_int err, const char* msg) {
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: " << msg << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        std::cout << "DEBUG: " << msg << " succeeded." << std::endl;
    }
}

void encoding_sw(
    const char input[INPUT_SIZE],
    int input_size,
    int output_code[INPUT_SIZE],
    int &output_size
);

void decoding_sw(
    const int encoded_data[INPUT_SIZE],
    int encoded_size,
    char output[INPUT_SIZE],
    int &output_length
);



    
void lzw_sw(
    const char input[INPUT_SIZE],
    int input_size,
    int output_code[INPUT_SIZE],
    int &output_size,
    char output[INPUT_SIZE],
    int &output_length
);
