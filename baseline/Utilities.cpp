#include "Utilities.h"
void Check_data(unsigned char *Data, unsigned int Size)
{
    int error_code = 0;
    unsigned char *Data_golden = (unsigned char *)malloc(MAX_OUTPUT_SIZE);
    FILE *File = fopen("../data/Golden.bin", "rb");
    if (File == NULL)
        Exit_with_error("fopen for Check_data failed");

    if (Size == 0)
        Exit_with_error("fclose for Check_data failed, Size==0");

    if (fread(Data_golden, 1, Size, File) != Size)
        Exit_with_error("fread for Check_data failed, Different Size");

    if (fclose(File) != 0)
        Exit_with_error("fclose for Check_data failed");

    for (unsigned int i = 0; i < Size; i++)
    {
        if (Data_golden[i] != Data[i])
        {
            free(Data_golden);
            error_code = i + 1;
        }
    }

    free(Data_golden);

    if (error_code != 0)
    {
        printf("You got errors in data %d\n", error_code);
        Exit_with_error("Input.bin and Golden.bin doesn't match");
    }
    else
    {
        printf("Application completed successfully.\n");
    }
}