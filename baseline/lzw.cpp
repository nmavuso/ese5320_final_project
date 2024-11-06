#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lzw.h"
#include <cstring>
#include <unordered_map>
#include <vector>
#include <iostream>

// Global dictionary variables
char **dictionary = NULL;
int dictSize = 0;
int dictCapacity = INITIAL_DICT_CAPACITY;

// Initialize dictionary only if it hasn't been initialized
void initializeDictionary()
{
    if (dictionary == NULL)
    {
        // Allocate memory for the dictionary with an initial capacity
        dictionary = (char **)malloc(dictCapacity * sizeof(char *));
        if (dictionary == NULL)
        {
            fprintf(stderr, "Memory allocation for dictionary failed\n");
            exit(1);
        }

        // Initialize dictionary with single-character ASCII entries (0-255)
        for (int i = 0; i < 256; i++)
        {
            dictionary[i] = (char *)malloc(2 * sizeof(char)); // 1 char + null terminator
            if (dictionary[i] == NULL)
            {
                fprintf(stderr, "Memory allocation for dictionary entry failed\n");
                exit(1);
            }
            dictionary[i][0] = static_cast<char>(i); // ASCII character
            dictionary[i][1] = '\0'; // Null terminator
        }

        // Set dictionary size to cover all initial single ASCII characters
        dictSize = 256;

        printf("Dictionary initialized with ASCII characters (0-255)\n");
    }
}

// Free dictionary memory to avoid memory leaks
void freeDictionary()
{
    if (dictionary != NULL)
    {
        for (int i = 0; i < dictSize; i++)
        {
            free(dictionary[i]);
        }
        free(dictionary);
        dictionary = NULL;
        dictSize = 0;
        dictCapacity = INITIAL_DICT_CAPACITY;
    }
}

// Function to encode a chunk of text using LZW encoding
// int *encode(const unsigned char *chunk, int *resultSize)
// {
//     if (dictionary == NULL)
//     {
//         initializeDictionary();
//     }

//     int chunkSize = std::strlen(reinterpret_cast<const char*>(chunk));
//     if (chunkSize == 0)
//     {
//         *resultSize = 0;
//         return NULL;
//     }

//     char *foundChars = (char *)malloc((chunkSize + 1) * sizeof(char));
//     if (foundChars == NULL)
//     {
//         fprintf(stderr, "Memory allocation for foundChars failed\n");
//         exit(1);
//     }
//     foundChars[0] = '\0';

//     int *result = (int *)malloc(chunkSize * sizeof(int));
//     if (result == NULL)
//     {
//         fprintf(stderr, "Memory allocation for result failed\n");
//         free(foundChars);
//         exit(1);
//     }
//     *resultSize = 0;

//     for (int i = 0; i < chunkSize; i++)
//     {
//         char character[2] = {static_cast<char>(chunk[i]), '\0'};
//         strcat(foundChars, character);

//         int foundIndex = -1;
//         for (int j = 0; j < dictSize; j++)
//         {
//             if (strcmp(dictionary[j], foundChars) == 0)
//             {
//                 foundIndex = j;
//                 break;
//             }
//         }

//         if (foundIndex != -1)
//         {
//             continue; // foundChars already in dictionary; continue to build string
//         }
//         else
//         {
//             if (dictSize >= dictCapacity)
//             {
//                 dictCapacity *= 2;
//                 dictionary = (char **)realloc(dictionary, dictCapacity * sizeof(char *));
//                 if (dictionary == NULL)
//                 {
//                     fprintf(stderr, "Memory reallocation for dictionary failed\n");
//                     free(result);
//                     free(foundChars);
//                     exit(1);
//                 }
//             }

//             dictionary[dictSize] = (char *)malloc((strlen(foundChars) + 1) * sizeof(char));
//             if (dictionary[dictSize] == NULL)
//             {
//                 fprintf(stderr, "Memory allocation for dictionary entry failed\n");
//                 free(result);
//                 free(foundChars);
//                 exit(1);
//             }
//             strcpy(dictionary[dictSize++], foundChars);

//             if (foundIndex != -1)
//             {
//                 result[*resultSize] = foundIndex;
//                 (*resultSize)++;
//             }

//             strcpy(foundChars, character); // Reset foundChars to current character
//         }
//     }

//     if (strlen(foundChars) > 0)
//     {
//         for (int j = 0; j < dictSize; j++)
//         {
//             if (strcmp(dictionary[j], foundChars) == 0)
//             {
//                 result[*resultSize] = j;
//                 (*resultSize)++;
//                 break;
//             }
//         }
//     }

//     free(foundChars);
//     return result;
// }

int *encode(const unsigned char *chunk, int *resultSize)
{
    // Check if the dictionary is initialized
    if (dictionary == NULL)
    {
        printf("Initializing dictionary...\n");
        initializeDictionary();
    }

    // Determine the size of the chunk (input string)
    int chunkSize = std::strlen(reinterpret_cast<const char*>(chunk));
    printf("Chunk size: %d\n", chunkSize);

    // If the chunk size is 0, return with resultSize set to 0
    if (chunkSize == 0)
    {
        *resultSize = 0;
        printf("Chunk is empty, exiting...\n");
        return NULL;
    }

    // Allocate memory for foundChars, which holds the current sequence being checked
    char *foundChars = (char *)malloc((chunkSize + 1) * sizeof(char));
    if (foundChars == NULL)
    {
        fprintf(stderr, "Memory allocation for foundChars failed\n");
        exit(1);
    }
    foundChars[0] = '\0';  // Initialize foundChars as an empty string

    // Allocate memory for the result array, which stores encoded output
    int *result = (int *)malloc(chunkSize * sizeof(int));
    if (result == NULL)
    {
        fprintf(stderr, "Memory allocation for result failed\n");
        free(foundChars);
        exit(1);
    }
    *resultSize = 0;

    int lastFoundIndex = -1;  // Track the last valid dictionary index found

    // Loop over each character in the chunk
    for (int i = 0; i < chunkSize; i++)
    {
        // Append the current character to foundChars
        char character[2] = {static_cast<char>(chunk[i]), '\0'};
        strcat(foundChars, character);
        printf("Building foundChars: %s\n", foundChars);

        // Search for foundChars in the dictionary
        int foundIndex = -1;
        for (int j = 0; j < dictSize; j++)
        {
            if (strcmp(dictionary[j], foundChars) == 0)
            {
                foundIndex = j;
                printf("Found '%s' in dictionary at index %d\n", foundChars, foundIndex);
                break;
            }
        }

        // If foundChars is in the dictionary, store the index and continue building the sequence
        if (foundIndex != -1)
        {
            lastFoundIndex = foundIndex;
            continue;
        }
        else
        {
            // Add the last valid dictionary index found to result
            if (lastFoundIndex != -1)
            {
                result[*resultSize] = lastFoundIndex;
                printf("Added index %d to result\n", lastFoundIndex);
                (*resultSize)++;
            }

            // If dictionary capacity is reached, resize the dictionary
            if (dictSize >= dictCapacity)
            {
                printf("Resizing dictionary...\n");
                dictCapacity *= 2;
                dictionary = (char **)realloc(dictionary, dictCapacity * sizeof(char *));
                if (dictionary == NULL)
                {
                    fprintf(stderr, "Memory reallocation for dictionary failed\n");
                    free(result);
                    free(foundChars);
                    exit(1);
                }
            }

            // Add new entry to the dictionary
            dictionary[dictSize] = (char *)malloc((strlen(foundChars) + 1) * sizeof(char));
            if (dictionary[dictSize] == NULL)
            {
                fprintf(stderr, "Memory allocation for dictionary entry failed\n");
                free(result);
                free(foundChars);
                exit(1);
            }
            strcpy(dictionary[dictSize++], foundChars);
            printf("Added '%s' to dictionary at index %d\n", foundChars, dictSize - 1);

            // Reset foundChars to the current character to start a new sequence
            strcpy(foundChars, "");
            printf("255 foundChars: %s\n", foundChars);
            printf("256 character: %s\n", character);

            lastFoundIndex = -1;  // Reset lastFoundIndex after adding to result
        }
    }

    // Add any remaining foundChars sequence in the dictionary to the result
    if (lastFoundIndex != -1)
    {
        result[*resultSize] = lastFoundIndex;
        printf("Final addition: Added index %d to result for remaining characters\n", lastFoundIndex);
        (*resultSize)++;
    }

    // Print the dictionary for debugging
    printf("Current Dictionary:\n");
    for (int i = 0; i < dictSize; i++) {
        if (dictionary[i] != NULL) {
            printf("Index %d: %s\n", i, dictionary[i]);
        } else {
            printf("Index %d: NULL\n", i);
        }
    }
    printf("\n");

    // Clean up memory
    free(foundChars);
    printf("Encoding complete. Result size: %d\n", *resultSize);

    return result;
}
