#ifndef LZW_HLS_H
#define LZW_HLS_H

#include <stdint.h>

#define TABLE_SIZE 512  // Dictionary size
#define INPUT_SIZE 256  // Maximum input size
#define CHAR_MAX 255    // Maximum single-character value


typedef struct {
    char str[INPUT_SIZE];  
    int code;              
} DictionaryEntry;

typedef struct {
    int code;              
    char str[INPUT_SIZE];  
} DecodeEntry;

//void encoding(const char *s, int *output_code, int *output_size);
//
//void decoding(const int *encoded_data, int encoded_size, char *output);

void lzw_fpga(const char *s, int *output_code, int *output_size, char *output);
void decoding(const int *encoded_data, int encoded_size, char *output);
void encoding(const char *input, int *output_code, int *output_size);

#endif 
