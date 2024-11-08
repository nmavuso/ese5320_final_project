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
 HashTable hash_table;
 initialize_hash_table(&hash_table);

 // Step 1: Ethernet Input .....
 unsigned char buff[] = "I AM SAM SAM I AM";
 unsigned int buff_size = sizeof(buff) - 1;
 //Step 2: Chunking the Ethernet input
 Chunk chunks[NUM_CHUNKS];
 int num_chunks = 0;
 

//void cdc(unsigned char *buff, unsigned int buff_size, Chunk chunks[], int *num_chunks)
 cdc(buff, buff_size, chunks, &num_chunks);  
 //Step 3: Deduplication
 

 for (size_t i = 0; i < num_chunks; ++i) {
    unsigned char *chunk_data = chunks[i].data;
    int chunk_size = chunks[i].size;

    int new_chunk = deduplicate_chunks(chunk_data, chunk_size, &hash_table);
    
    if (new_chunk) {
      int encoded_data[INPUT_SIZE];
      int encoded_size;
      int encode_success = encoding((const char *) chunk_data, encoded_data, &encoded_size);
      if (encode_success == 0) {
        printf("Chunk %d successfully encoded. Encoded data: ", i+1);
        for (int j = 0; j < encoded_size; ++j) {
          printf("%d", encoded_data[j]);
        }
        printf("\n");
       } 
       else {
        printf("Encoding failed for chunk %d\n", i + 1);	
       }  
    } else {
      printf("Chunk %d is a duplicate and was not re-encoded.\n", i + 1);
    }
 }
  
 printf("Encoding Complete");
 return 0;
}
