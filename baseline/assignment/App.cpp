#include "App.h"
#include <stdio.h>
#include <stdlib.h>
#include "stopwatch.h"
#include <chrono>
#include <iostream>

#define DATA_SIZE (12000 * 8000)
#define STAGES (4)

unsigned char *Data[STAGES + 1]; // allocate array or size 5
stopwatch time_scale;
stopwatch time_differentiation;
stopwatch time_compression;
stopwatch Avg_time;
void Exit_with_error(void)
{
  perror(NULL);
  exit(EXIT_FAILURE);
}

void Load_data(void)
{
  FILE *File = fopen("Input.bin", "rb");
  if (File == NULL)
    Exit_with_error();

  // read in the binary into data[0]
  if (fread(Data[0], 1, DATA_SIZE, File) != DATA_SIZE)
    Exit_with_error();

  if (fclose(File) != 0)
    Exit_with_error();
}

void Store_data(const char *Filename, int Stage, unsigned int Size)
{
  FILE *File = fopen(Filename, "wb");
  if (File == NULL)
    Exit_with_error();

  // write the stage's binary to an output file
  if (fwrite(Data[Stage], 1, Size, File) != Size)
    Exit_with_error();

  if (fclose(File) != 0)
    Exit_with_error();
}

int main()
{
  for (int i = 0; i <= STAGES; i++)
  {
    // We could strictly allocate less memory for some of these buffers, but
    // that is irrelevant here.
    Data[i] = (unsigned char *)malloc(DATA_SIZE); // malloc the size of the raw file here
    if (Data[i] == NULL)
      Exit_with_error();
  }
  Load_data(); // load in the data
  Avg_time.start();

  time_scale.start();
  Scale(Data[0], Data[1]); // outputs the scaled data into data[1]
  time_scale.stop();
  std::cout << "Avg latency of scale is: " << time_scale.avg_latency() << " ns." << std::endl;

  Filter(Data[1], Data[2]);
  time_differentiation.start();
  Differentiate(Data[2], Data[3]);
  time_differentiation.stop();
  std::cout << "Avg latency of Differentiate is: " << time_differentiation.avg_latency() << " ns." << std::endl;

  time_compression.start();
  int Size = Compress(Data[3], Data[4]);
  time_compression.stop();
  std::cout << "Avg latency of Compression is: " << time_compression.avg_latency() << " ns." << std::endl;

  Store_data("Output.bin", 4, Size);

  for (int i = 0; i <= STAGES; i++)
    free(Data[i]);

  puts("Application completed successfully.");

  return EXIT_SUCCESS;
}
