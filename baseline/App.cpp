#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "App.h"
#include "cmd.h"
#include "lzw.h"

std::vector<std::vector<unsigned char>> test_cdc(const char *file)
{
    FILE *fp = fopen(file, "rb");
    if (fp == nullptr)
    {
        perror("Error opening file");
        return {};
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *buff = (unsigned char *)malloc(file_size);
    if (buff == nullptr)
    {
        perror("Not enough memory");
        fclose(fp);
        return {};
    }

    int bytes_read = fread(buff, sizeof(unsigned char), file_size, fp);
    if (bytes_read != file_size)
    {
        perror("File read error");
        free(buff);
        fclose(fp);
        return {};
    }

    std::vector<std::vector<unsigned char>> chunks = cdc(buff, file_size);

    free(buff);
    free(fp);
    std::cout << "Success on CDC!" << std::endl;

    return chunks;
}

std::vector<int *> test_cmd(const std::vector<std::vector<unsigned char>> &chunks)
{
    HashTable *hash_table = initialize_hash_table();
    if (hash_table == nullptr)
    {
        return {};
    }

    std::vector<int *> decode_queue;
    for (const auto &chunk : chunks)
    {
        int *result = deduplicate_chunks(chunk.data(), chunk.size(), hash_table);
        if (result != NULL)
        {
            decode_queue.insert(decode_queue.begin(), result);
        }
        else
        {
            perror("ERROR! Deduplicate returned NULL");
        }
    }

    // cleanup
    for (int i = 0; i < HASH_TABLE_SIZE; ++i)
    {
        HashEntry *entry = hash_table->entries[i];
        while (entry)
        {
            HashEntry *next = entry->next;
            free(entry->value);
            free(entry);
            entry = next;
        }
    }

    free(hash_table->entries);
    free(hash_table);

    return decode_queue;
}

void test_lzw(const std::vector<std::vector<unsigned char>> &unique_chunks)
{
    for (size_t i = 0; i < unique_chunks.size(); ++i)
    {
        const auto &chunk = unique_chunks[i];

        int resultSize;
        int *encoded_result = encode((chunk.data()), &resultSize);

        std::cout << "Encoded result for chunk " << i + 1 << ": ";
        for (int j = 0; j < resultSize; ++j)
        {
            std::cout << encoded_result[j] << " ";
        }
        std::cout << std::endl;

        free(encoded_result);
    }
}

int main()
{
    std::cout << "Testing CDC..." << std::endl;
    std::vector<std::vector<unsigned char>> chunks = test_cdc("prince.txt");
    test_lzw(chunks); // NOTE: this will encode all chunks

    std::cout << "Testing CMD & LZW..." << std::endl;

    // Design choice: can either return list of unique chunks or decoding stream
    // std::vector<std::vector<unsigned char>> unique_chunks = test_cmd(chunks);
    std::vector<int *> decoding_stream = test_cmd(chunks);

    return 0;
}
