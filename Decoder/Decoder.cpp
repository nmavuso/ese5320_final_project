#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdint.h>
#include "../Server/lzw_hw.h"
#include <bitset> // For std::bitset
#include <arpa/inet.h> // For ntohl

#define CODE_LENGTH (13)
#define MAX_CHUNK_SIZE 8192
#define MAX_CHUNK_STORAGE (MAX_CHUNK_SIZE * (13 / 8))

typedef std::vector<std::string> code_table;
typedef std::vector<std::string> chunk_list;

static code_table Code_table;

static std::ifstream Input;
static size_t Input_position;

uint32_t manual_swap_endian_16bit(uint32_t value) {
    return ((value >> 16) & 0xFFFF) |  // Move upper 16 bits to lower
           ((value & 0xFFFF) << 16);  // Move lower 16 bits to upper
}

static int Read_code(void)
{
  static unsigned char Byte;

  int Code = 0;
  int Length = CODE_LENGTH;
  for (int i = 0; i < Length; i++)
  {
    if (Input_position % 8 == 0)
      Byte = Input.get();
    Code = (Code << 1) | ((Byte >> (7 - Input_position % 8)) & 1);
    Input_position++;
  }
  return Code;
}

// static const std::string Decompress(size_t Size)
// {
//   std::cerr << "Inside Decompress" << std::endl;
//   Input_position = 0;

//   Code_table.clear();
//   for (int i = 0; i < 256; i++)
//     Code_table.push_back(std::string(1, (char) i));

//   int Old = Read_code();
//   std::string Symbol(1, Old);
//   std::string Output = Symbol;
//   while (Input_position / 8 < Size - 1)
//   {
//     int New = Read_code();
//     std::cerr << "Inside While Loop Value (Binary): 0b"
//     << std::bitset<13>(New) << std::endl;

//     std::string Symbols;
//     if (New >= (int) Code_table.size())
//       Symbols = Code_table[Old] + Symbol;
//     else
//       Symbols = Code_table[New];
//     Output += Symbols;
//     Symbol = std::string(1, Symbols[0]);
//     Code_table.push_back(Code_table[Old] + Symbol);
//     Old = New;
//   }

//   return Output;
// }

int main(int Parameter_count, char* Parameters[]) {
    if (Parameter_count < 3) {
        std::cout << "Usage: " << Parameters[0] << " <Compressed file> <Decompressed file>\n";
        return EXIT_SUCCESS;
    }

    // Open the input file in binary mode
    FILE* Input = fopen(Parameters[1], "rb");
    if (!Input) {
        std::cerr << "Could not open input file.\n";
        return EXIT_FAILURE;
    }

    // Open the output file in binary mode
    FILE* Output = fopen(Parameters[2], "wb");
    if (!Output) {
        std::cerr << "Could not open output file.\n";
        fclose(Input);
        return EXIT_FAILURE;
    }

    chunk_list Chunks; // List to store decompressed chunks
    int i = 0;

    while (true) {
        // Read the 32-bit header
        uint32_t Header;
        size_t bytes_read = fread(&Header, sizeof(uint32_t), 1, Input);
        if (bytes_read != 1) {
            if (feof(Input)) break; // End of file reached
            std::cerr << "Error reading input file.\n";
            fclose(Input);
            fclose(Output);
            return EXIT_FAILURE;
        }

        // Swap endianess for the header
        Header = manual_swap_endian_16bit(Header);

        std::cout << "Header NEW (Hex): 0x"
                  << std::hex << Header << std::endl;

        // Check the MSB to determine chunk type
        if ((Header & 0x80000000) == 0) { // LZW Chunk
            int Chunk_size = Header & 0x7FFFFFFF; // Remove MSB to get size

            // Read the encoded data (packed in CODE_LENGTH bits per code)
            int total_bytes = (Chunk_size * CODE_LENGTH + 7) / 8; // Calculate total bytes to read
            uint8_t buffer[total_bytes];

            // Read the whole chunk into the buffer
            size_t bytes_read = fread(buffer, sizeof(uint8_t), total_bytes, Input);

            for (size_t i = 0; i + 1 < bytes_read; i += 2) {
                std::swap(buffer[i], buffer[i + 1]);
            }

            int codes_read = bytes_read * 8 / CODE_LENGTH;
            int EncodedData[codes_read];
            int current_code = 0;
            for (int bit_index = 0; bit_index < codes_read * CODE_LENGTH; bit_index += CODE_LENGTH) {
                uint32_t code = 0;
                for (int bit = 0; bit < CODE_LENGTH; ++bit) {
                    int byte_index = (bit_index + bit) / 8;
                    int bit_offset = (bit_index + bit) % 8;

                    code |= ((buffer[byte_index] >> (7 - bit_offset)) & 1) << (CODE_LENGTH - 1 - bit);
                }
                EncodedData[current_code++] = code;
            }

            std::cout << "Last Three: " << EncodedData[codes_read - 3] << std::endl;
            std::cout << "Last Three: " << EncodedData[codes_read - 2] << std::endl;
            std::cout << "Last Three: " << EncodedData[codes_read - 1] << std::endl;

            // Decode the LZW chunk
            char DecodedOutput[MAX_CHUNK_STORAGE];
            int DecodedOutputLength = 0;
            decoding_sw(EncodedData, codes_read, DecodedOutput, DecodedOutputLength);

            // Store the decoded chunk and write to the output file
            Chunks.emplace_back(DecodedOutput, DecodedOutputLength);
            std::cout << "Decompressed chunk with size " << DecodedOutputLength << ".\n";
            fwrite(DecodedOutput, sizeof(char), DecodedOutputLength, Output);
        } else { // Duplicate Chunk
            int Location = Header & 0x7FFFFFFF; // Remove MSB to get location
            if (Location < static_cast<int>(Chunks.size())) { // Validate index
                const std::string& Chunk = Chunks[Location];
                std::cout << "Found chunk of size " << Chunk.length() << " in database.\n";
                fwrite(Chunk.data(), sizeof(char), Chunk.length(), Output);
            } else {
                std::cerr << "Location " << Location << " not in database of length " << Chunks.size()
                          << " ignoring block. Likely encoder error.\n";
            }
        }

        i++;
    }

    fclose(Input);
    fclose(Output);
    return EXIT_SUCCESS;
}