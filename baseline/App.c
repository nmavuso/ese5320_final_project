#include "Encoder.h"


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

void test_cdm () {

}

void test_lzw () {

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

}

// Main function
int main() {
    std::vector<std::vector<unsigned char>> chunks = test_cdc("prince.txt");

    for (size_t i = 0; i < chunks.size(); ++i) {
        std::cout << "Chunk " << i + 1 << " size: " << chunks[i].size() << " bytes" << std::endl;
    }

    //Send the list of chunks to CDM 

    //EN
    
    
    return 0;

}
