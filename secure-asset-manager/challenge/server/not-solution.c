#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../common/validator.h"
#include "../common/proc_maps_parser/pmparser.h"

data_block_t *file_to_data_block(char *filename) {
  data_block_t *result = (data_block_t *) malloc(sizeof(data_block_t));

  struct stat statbuf;
  if(stat(filename, &statbuf) == -1) {
    printf("Couldn't open code file: %s\n", filename);
    exit(1);
  }
  result->length = statbuf.st_size;
  result->data = (uint8_t*) malloc(result->length);

  FILE *file = fopen(filename, "rb");
  if(read(fileno(file), result->data, result->length) != result->length)
  {
    fprintf(stderr, "Failed to read() the entire file!\n");
    exit(1);
  }

  return result;
}

uint8_t hex_to_c(char c) {
  if(c >= '0' && c <= '9') {
    return c - '0';
  }

  if(c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }

  if(c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }

  fprintf(stderr, "Bad hex character: 0x%02x\n", c);
  exit(1);
}

data_block_t *hex_to_data_block(char *hex) {
  // Allocate result
  data_block_t *result = (data_block_t *) malloc(sizeof(data_block_t));

  // Allocate memory
  result->data = malloc(strlen(hex));
  result->length = strlen(hex) / 2;

  // Loop across the string
  size_t i;
  for(i = 0; i < result->length; i++) {
    result->data[i] =
      (hex_to_c(hex[(i * 2)]) << 4) |
      (hex_to_c(hex[(i * 2) + 1]));
  }

  return result;
}

int main(int argc, char *argv[])
{
  if(argc != 3) {
    printf("Usage: %s <code_file> <memory_file>\n", argv[0]);
    exit(1);
  }

  data_block_t *code = file_to_data_block(argv[1]);
  data_block_t *data = file_to_data_block(argv[2]);

  uint32_t result = checksum(code, data);
  printf("%08x\n", result);

  exit(0);
}
