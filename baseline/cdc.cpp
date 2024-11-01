#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <iostream>

#define WIN_SIZE 16
#define PRIME 3
#define MODULUS 256
#define TARGET 0

uint64_t hash_func(unsigned char *input, unsigned int pos)
{
    // put your hash function implementation here
    uint64_t hash = 0;
    for (int i = 0; i < WIN_SIZE; ++i) {
        int input_char = input[pos + WIN_SIZE - i];
        int power = pow(PRIME, i + 1);
        hash += input_char * power;
    }

    return hash;
}

//void cdc(unsigned char *buff, unsigned int buff_size)
//{
    // put your cdc implementation here
  //  for (unsigned int i = WIN_SIZE; i < (buff_size - WIN_SIZE); i = i + 2) {
  //      int hash = hash_func(buff, i);
     //   if (((hash % MODULUS)) == TARGET) {
       //     std::cout<<i + 1 <<std::endl;
       // }
       // int char_1 = buff[i];
       // int char_2 = buff[i + WIN_SIZE];
       // int next_hash = hash*PRIME - char_1*pow(PRIME, WIN_SIZE + 1) + char_2*PRIME;
       // if (((next_hash % MODULUS)) == TARGET) {
        //    std::cout<<i + 2<<std::endl;
       // }


//    }
//void cdc(unsigned char *buff, unsigned int buff_size)
//{
    // put your cdc implementation here
  //  for (unsigned int i = WIN_SIZE; i < (buff_size - WIN_SIZE); i = i + 2) {
    //    int hash = hash_func(buff, i);
      //  if (((hash % MODULUS)) == TARGET) {
        //    std::cout<<i + 1 <<std::endl;
        //}
        //int char_1 = buff[i];
        //int char_2 = buff[i + WIN_SIZE];
        //int power_1 = pow(PRIME, WIN_SIZE + 1);
        //int next_hash = hash*PRIME - char_1*power_1 + char_2*PRIME;
        //if (((next_hash % MODULUS)) == TARGET) {
      //      std::cout<<i + 2<<std::endl;
    //    }


  //
//}

//int update_hash (unsigned char *buff, unsigned int i, int hash) {
    //int char_1 = buff[i]*pow(PRIME, WIN_SIZE + 1);
    //int char_2 = buff[i + WIN_SIZE]*PRIME;
    //int next_hash = hash*PRIME - char_1* + char_2;

  //  return next_hash;
//}

uint64_t update_hash (unsigned char *buff, unsigned int i, uint64_t hash) {
    int char_1 = buff[i];
    int char_2 = buff[i + WIN_SIZE];
    uint64_t next_hash = hash*PRIME - char_1*pow(PRIME, WIN_SIZE + 1) + char_2 *PRIME;

    return next_hash;
}
void cdc(unsigned char *buff, unsigned int buff_size)
{
    // put your cdc implementation here
    uint64_t hash = hash_func(buff, WIN_SIZE);
    for (unsigned int i = WIN_SIZE + 1; i < (buff_size - WIN_SIZE) + 1; i++) {

        if (((hash % MODULUS)) == TARGET) {
            std::cout<<i <<std::endl;
        }
        hash = update_hash(buff, i, hash);

    }

}

void test_cdc( const char* file )
{
    FILE* fp = fopen(file,"r" );
    if(fp == NULL ){
        perror("fopen error");
        return;
    }

    fseek(fp, 0, SEEK_END); // seek to end of file
    int file_size = ftell(fp); // get current file pointer
    fseek(fp, 0, SEEK_SET); // seek back to beginning of file

    unsigned char* buff = (unsigned char *)malloc((sizeof(unsigned char) * file_size ));
    if(buff == NULL)
    {
        perror("not enough space");
        fclose(fp);
        return;
    }

    int bytes_read = fread(&buff[0],sizeof(unsigned char),file_size,fp);

    cdc(buff, file_size);

    free(buff);
    return;
}

int main()
{
    test_cdc("prince.txt");
    return 0;

}
