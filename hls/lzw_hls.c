#include "lzw_hls.h"
#include <stdio.h>
#include <string.h>

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

void concat_strings(const char *a, const char *b, char *result) {
    int i = 0, j = 0;
    while (a[i] != '\0') {
        result[i] = a[i];
        i++;
    }
    while (b[j] != '\0') {
        result[i++] = b[j++];
    }
    result[i] = '\0';
}

int string_compare(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }
    return a[i] == b[i];
}

void string_copy(const char *src, char *dest) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int encoding(const char *s, int *output_code, int *output_size) {
    DictionaryEntry table[TABLE_SIZE];
    int code = CHAR_MAX;
    int table_size = CHAR_MAX;

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

        concat_strings(p, c, temp);

        int found = -1;
        for (int j = 0; j < table_size; j++) {
            if (string_compare(table[j].str, temp)) {
                found = j;
                break;
            }
        }

        if (found != -1) {
            string_copy(temp, p);
        } else {
            for (int j = 0; j < table_size; j++) {
                if (string_compare(table[j].str, p)) {
                    output_code[out_index++] = table[j].code;
                    break;
                }
            }
            if (table_size < TABLE_SIZE) {
                string_copy(temp, table[table_size].str);
                table[table_size].code = code++;
                table_size++;
            }
            string_copy(c, p);
        }
    }

    for (int j = 0; j < table_size; j++) {
        if (string_compare(table[j].str, p)) {
            output_code[out_index++] = table[j].code;
            break;
        }
    }

    *output_size = out_index;
    return 0;
}

int decoding(const int *encoded_data, int encoded_size) {
    DecodeEntry table[TABLE_SIZE];
    int table_size = CHAR_MAX;

    // Initialize the dictionary with single characters
    for (int i = 0; i < CHAR_MAX; i++) {
        table[i].code = i;
        table[i].str[0] = (char)i;
        table[i].str[1] = '\0';
    }

    int old_code = encoded_data[0];
    char c = table[old_code].str[0];
    int count = CHAR_MAX;

    for (int i = 1; i < encoded_size; i++) {
        int new_code = encoded_data[i];
        char entry[INPUT_SIZE];

        // If the new code is not in the table, create a new entry
        if (new_code >= table_size) {
            concat_strings(table[old_code].str, &c, entry);
        } else {
            string_copy(table[new_code].str, entry);
        }

        // Update the current character
        c = entry[0];

        // Add the new entry to the table
        if (table_size < TABLE_SIZE) {
            concat_strings(table[old_code].str, &c, table[table_size].str);
            table[table_size].code = count++;
            table_size++;
        }

        old_code = new_code;
    }

    return 0;
}

void lzw_fpga (const char *s, int *output_code, int *output_size, const int *encoded_data, int encoded_size) {
	int encoding_success;
	int decoding_success;
	encoding_success = encoding(s, output_code, output_size);
	decoding_success = decoding(encoded_data, encoded_size);
}