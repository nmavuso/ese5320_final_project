#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cdc_hw.h"
#include "cmd_hw.h"
#include "lzw_hw.h"

int main() {

/*************************************/
/********** Initialization ***********/
/*************************************/
 HashTable *hash_table = initialize_hash_table();

 // Step 1: Ethernet Input .....
 unsigned char buff[] = "I AM SAM SAM I AM";
 unsigned int buff_size = sizeof(buff) - 1;
 //Step 2: Chunking the Ethernet input
 std::vector<std::vector<unsigned char>> chunks = cdc(buff, buff_size);  
 //Step 3: Deduplication
 

 for (size_t i = 0; i < chunks.size(); ++i) {
    unsigned char *chunk_data = chunks[i].data();
    int chunk_size = chunks[i].size();
    int new_chunk = deduplicate_chunks(chunk_data, chunk_size, *hash_table);
    
    if (new_chunk) {
      int encoded_data[INPUT_SIZE];
      int encoded_size;
      int encode_success = encoding((const char *) chunk_data, encoded_data, &encoded_size); 
    }
   
 }
  
 printf("Test_Complete");

 return 0;
}
