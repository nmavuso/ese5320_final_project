#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Initial dictionary size and allocation
#define INITIAL_DICT_CAPACITY 256

static int dictSize = 0;
static int dictCapacity = INITIAL_DICT_CAPACITY;
char** dictionary = NULL; // Initialize dictionary as a pointer to hold dynamic allocation

// Function to initialize the dictionary
void initializeDictionary() {
    dictionary = (char**)malloc(dictCapacity * sizeof(char*));
    if (dictionary == NULL) {
        fprintf(stderr, "Memory allocation for dictionary failed\n");
        exit(1);
    }
}

// Function to encode a chunk of text
int* encode(char* chunk, int* resultSize) {
    if (dictionary == NULL) {
        initializeDictionary(); // Initialize dictionary if it hasn't been
    }

    int chunkSize = strlen(chunk);
    char foundChars[chunkSize + 1]; // +1 for null terminator
    foundChars[0] = '\0'; // Initialize as an empty string

    int* result = (int*)malloc(chunkSize * sizeof(int));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation for result failed\n");
        exit(1);
    }
    *resultSize = 0;

    for (int i = 0; i < chunkSize; i++) {
        char character[2] = { chunk[i], '\0' };
        char charsToAdd[chunkSize + 1];
        strcpy(charsToAdd, foundChars);
        strcat(charsToAdd, character);

        bool exists = false;
        int foundIndex = -1;

        // Check if charsToAdd is already in the dictionary
        for (int j = 0; j < dictSize; j++) {
            if (strcmp(dictionary[j], charsToAdd) == 0) {
                exists = true;
                foundIndex = j;
                break;
            }
        }

        if (exists) {
            strcpy(foundChars, charsToAdd);
        } else {
            // Output the code for foundChars if it exists in the dictionary
            for (int k = 0; k < dictSize; k++) {
                if (strcmp(dictionary[k], foundChars) == 0) {
                    result[*resultSize] = k;
                    (*resultSize)++;
                    break;
                }
            }

            // Check if we need to expand the dictionary
            if (dictSize >= dictCapacity) {
                dictCapacity *= 2;
                dictionary = (char**)realloc(dictionary, dictCapacity * sizeof(char*));
                if (dictionary == NULL) {
                    fprintf(stderr, "Memory reallocation for dictionary failed\n");
                    exit(1);
                }
            }

            // Add charsToAdd to the dictionary
            dictionary[dictSize] = (char*)malloc((strlen(charsToAdd) + 1) * sizeof(char));
            strcpy(dictionary[dictSize++], charsToAdd);

            // Update foundChars to the current character
            strcpy(foundChars, character);
        }
    }

    // Output the last foundChars if it's not empty
    if (strlen(foundChars) > 0) {
        for (int k = 0; k < dictSize; k++) {
            if (strcmp(dictionary[k], foundChars) == 0) {
                result[*resultSize] = k;
                (*resultSize)++;
                break;
            }
        }
    }

    return result;
}

// Main function for testing
int main() {
    char chunk[] = "YOUR_TEST_STRING_HERE";
    int resultSize;
    int* result = encode(chunk, &resultSize);

    printf("Encoded result: ");
    for (int i = 0; i < resultSize; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    // Free allocated memory for result and dictionary
    free(result);
    for (int i = 0; i < dictSize; i++) {
        free(dictionary[i]);
    }
    free(dictionary);

    return 0;
}
