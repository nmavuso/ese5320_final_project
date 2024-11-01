// dictionary_encoding.h
#ifndef DICTIONARY_ENCODING_H
#define DICTIONARY_ENCODING_H

#include <stdbool.h>

// Initial dictionary size and allocation
#define INITIAL_DICT_CAPACITY 256

// Function to initialize the dictionary
void initializeDictionary();

// Function to encode a chunk of text
// Parameters:
//   - chunk: the text chunk to encode
//   - resultSize: pointer to store the size of the result
// Returns: an integer array containing encoded values
int *encode(const char *chunk, int chunk_size, int *resultSize);

// Function to free memory allocated for the dictionary
void freeDictionary();

#endif // DICTIONARY_ENCODING_H
