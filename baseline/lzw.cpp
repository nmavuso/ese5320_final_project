#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lzw.h"

#define INITIAL_DICT_CAPACITY 256

static int dictSize = 0;
static int dictCapacity = INITIAL_DICT_CAPACITY;
char **dictionary = NULL;

// Initialize dictionary only if it hasn't been initialized
void initializeDictionary() {
    if (dictionary == NULL) {
        dictionary = (char **)malloc(dictCapacity * sizeof(char *));
        if (dictionary == NULL) {
            fprintf(stderr, "Memory allocation for dictionary failed\n");
            exit(1);
        }
        dictSize = 0;
    }
}

// Free dictionary memory to avoid memory leaks
void freeDictionary() {
    if (dictionary != NULL) {
        for (int i = 0; i < dictSize; i++) {
            free(dictionary[i]);
        }
        free(dictionary);
        dictionary = NULL;
        dictSize = 0;
        dictCapacity = INITIAL_DICT_CAPACITY;
    }
}

// Function to encode a chunk of text using LZW encoding
int *encode(char *chunk, int *resultSize) {
    if (dictionary == NULL) {
        initializeDictionary();
    }

    int chunkSize = strlen(chunk);
    if (chunkSize == 0) {
        *resultSize = 0;
        return NULL;
    }

    char *foundChars = (char *)malloc((chunkSize + 1) * sizeof(char));
    if (foundChars == NULL) {
        fprintf(stderr, "Memory allocation for foundChars failed\n");
        exit(1);
    }
    foundChars[0] = '\0';

    int *result = (int *)malloc(chunkSize * sizeof(int));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation for result failed\n");
        free(foundChars);
        exit(1);
    }
    *resultSize = 0;

    for (int i = 0; i < chunkSize; i++) {
        char character[2] = {chunk[i], '\0'};
        strcat(foundChars, character);

        int foundIndex = -1;
        for (int j = 0; j < dictSize; j++) {
            if (strcmp(dictionary[j], foundChars) == 0) {
                foundIndex = j;
                break;
            }
        }

        if (foundIndex != -1) {
            continue; // foundChars already in dictionary; continue to build string
        } else {
            if (dictSize >= dictCapacity) {
                dictCapacity *= 2;
                dictionary = (char **)realloc(dictionary, dictCapacity * sizeof(char *));
                if (dictionary == NULL) {
                    fprintf(stderr, "Memory reallocation for dictionary failed\n");
                    free(result);
                    free(foundChars);
                    exit(1);
                }
            }

            dictionary[dictSize] = (char *)malloc((strlen(foundChars) + 1) * sizeof(char));
            if (dictionary[dictSize] == NULL) {
                fprintf(stderr, "Memory allocation for dictionary entry failed\n");
                free(result);
                free(foundChars);
                exit(1);
            }
            strcpy(dictionary[dictSize++], foundChars);

            if (foundIndex != -1) {
                result[*resultSize] = foundIndex;
                (*resultSize)++;
            }

            strcpy(foundChars, character);  // Reset foundChars to current character
        }
    }

    if (strlen(foundChars) > 0) {
        for (int j = 0; j < dictSize; j++) {
            if (strcmp(dictionary[j], foundChars) == 0) {
                result[*resultSize] = j;
                (*resultSize)++;
                break;
            }
        }
    }

    free(foundChars);
    return result;
}

// Main function for testing
int test_lzw() {
    char chunk[] = "YOUR_TEST_STRING_HERE";
    int resultSize;
    int *result = encode(chunk, &resultSize);

    printf("Encoded result: ");
    for (int i = 0; i < resultSize; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");

    free(result);
    freeDictionary();

    return 0;
}
