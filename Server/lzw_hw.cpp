#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 4096 // Maximum dictionary size
#define CHAR_MAX 256    // Initial number of ASCII characters
#define INPUT_SIZE 2048 // Maximum input string size

// Dictionary for encoding
typedef struct {
    char str[INPUT_SIZE];
    int code;
} DictionaryEntry;

// Dictionary for decoding
typedef struct {
    int code;
    char str[INPUT_SIZE];
} DecodeEntry;

// Function for encoding a string using LZW
int encoding(const char *s, int *output_code, int *output_size) {
    if (!s || !output_code || !output_size) {
        fprintf(stderr, "Invalid input to encoding function\n");
        return -1;
    }

    DictionaryEntry table[TABLE_SIZE];
    int code = CHAR_MAX;  // Start from 256 for new codes
    int table_size = CHAR_MAX;

    // Initialize the dictionary with single characters
    for (int i = 0; i < CHAR_MAX; i++) {
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
        table[i].code = i;
    }

    char p[INPUT_SIZE] = {s[0], '\0'};
    int out_index = 0;

    for (int i = 1; s[i] != '\0'; i++) {
        char c[2] = {s[i], '\0'};
        char temp[INPUT_SIZE];

        // Concatenate p and c into temp
        int ret = snprintf(temp, sizeof(temp), "%s%s", p, c);
        if (ret >= (int)sizeof(temp)) {
            fprintf(stderr, "Error: Output truncated during encoding\n");
            return -1; // Handle truncation error
        }

        // Check if temp exists in the dictionary
        int found = -1;
        for (int j = 0; j < table_size; j++) {
            if (strcmp(table[j].str, temp) == 0) {
                found = j;
                break;
            }
        }

        if (found != -1) {
            // If temp exists, set p to temp
            strcpy(p, temp);
        } else {
            // Output the code for p
            for (int j = 0; j < table_size; j++) {
                if (strcmp(table[j].str, p) == 0) {
                    output_code[out_index++] = table[j].code;
                    break;
                }
            }
            // Add temp (p + c) to the dictionary
            if (table_size < TABLE_SIZE) {
                strncpy(table[table_size].str, temp, sizeof(table[table_size].str) - 1);
                table[table_size].str[sizeof(table[table_size].str) - 1] = '\0'; // Null-terminate
                table[table_size].code = code++;
                table_size++;
            }
            // Reset p to the current character c
            strcpy(p, c);
        }
    }

    // Output the code for the last sequence in p
    for (int j = 0; j < table_size; j++) {
        if (strcmp(table[j].str, p) == 0) {
            output_code[out_index++] = table[j].code;
            break;
        }
    }

    *output_size = out_index;
    return 0;
}

// Function for decoding an LZW encoded sequence
int decoding(const int *encoded_data, int encoded_size) {
    if (!encoded_data || encoded_size <= 0) {
        fprintf(stderr, "Invalid input to decoding function\n");
        return -1;
    }

    DecodeEntry table[TABLE_SIZE];
    int table_size = CHAR_MAX;

    // Initialize the dictionary with single characters
    for (int i = 0; i < CHAR_MAX; i++) {
        table[i].code = i;
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
    }

    int old_code = encoded_data[0];
    printf("%s", table[old_code].str); // Print the first decoded string
    char c = table[old_code].str[0];
    int count = CHAR_MAX;

    for (int i = 1; i < encoded_size; i++) {
        int new_code = encoded_data[i];
        char entry[INPUT_SIZE];

        // Check if the new code exists in the dictionary
        if (new_code >= table_size) {
            snprintf(entry, sizeof(entry), "%s%c", table[old_code].str, c);
        } else {
            strncpy(entry, table[new_code].str, sizeof(entry) - 1);
            entry[sizeof(entry) - 1] = '\0'; // Null-terminate
        }

        printf("%s", entry); // Print the decoded string
        c = entry[0];

        // Add a new entry to the dictionary
        if (table_size < TABLE_SIZE) {
            snprintf(table[table_size].str, sizeof(table[table_size].str), "%s%c", table[old_code].str, c);
            table[table_size].code = count++;
            table_size++;
        }

        old_code = new_code;
    }
    return 0;
}
