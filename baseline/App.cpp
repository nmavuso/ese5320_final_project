#include "App.h"



std::vector<std::vector<unsigned char>> test_cdc(const char* file) {
    FILE* fp = fopen(file, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return {};
    }

    fseek(fp, 0, SEEK_END);      // Seek to end of file
    int file_size = ftell(fp);    // Get file size
    fseek(fp, 0, SEEK_SET);       // Seek back to beginning of file

    unsigned char* buff = (unsigned char*)malloc(file_size);
    if (buff == NULL) {
        perror("Not enough memory");
        fclose(fp);
        return {};
    }

    int bytes_read = fread(buff, sizeof(unsigned char), file_size, fp);
    if (bytes_read != file_size) {
        perror("File read error");
        free(buff);
        fclose(fp);
        return {};
    }

    std::vector<std::vector<unsigned char>> chunks = cdc(buff, file_size);

    free(buff);
    fclose(fp);

    return chunks;
}

void test_cmd () {

 char chunks[NUM_CHUNKS][MAX_CHUNK_SIZE];
    int chunk_sizes[NUM_CHUNKS];
    HashTable *hash_table = initialize_hash_table();

    // Initialize chunks with some sample data
    for (int i = 0; i < NUM_CHUNKS; i++)
    {
        chunk_sizes[i] = (i % 100) + 1; // Varying sizes from 1 to 100 bytes
        for (int j = 0; j < chunk_sizes[i]; j++)
        {
            chunks[i][j] = (uint8_t)(i + j);
        }
    }

    // Deduplicate chunks
    deduplicate_chunks(chunks, chunk_sizes, hash_table);

    // Clean up
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        free(hash_table->entries[i]);
    }
    free(hash_table);

}

void test_lzw () {

    char chunk[] = "YOUR_TEST_STRING_HERE I AM SAM SAM I AM";
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

}

// Main function
int main() {
    std::vector<std::vector<unsigned char>> chunks = test_cdc("prince.txt");

    for (size_t i = 0; i < chunks.size(); ++i) {
        std::cout << "Chunk " << i + 1 << " size: " << chunks[i].size() << " bytes" << std::endl;
    }

    //Send the list of chunks to CDM 

    //CDM

  //LZW
  test_lzw();
    
    
    return 0;

}
 
