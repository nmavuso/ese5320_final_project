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
    uint64_t hash = 0;
    for (int i = 0; i < WIN_SIZE; ++i) {
        int input_char = input[pos + WIN_SIZE - i];
        int power = pow(PRIME, i + 1);
        hash += input_char * power;
    }

    return hash;
}

void cdc(unsigned char *buff, unsigned int buff_size)
{

    for (unsigned int i = WIN_SIZE; i < (buff_size - WIN_SIZE); ++i) {
        if (((hash_func(buff, i) % MODULUS)) == TARGET) {
            std::cout<<i + 1<<std::endl;
        }
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
