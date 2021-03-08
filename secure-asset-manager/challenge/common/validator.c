#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "validator.h"

uint32_t checksum(data_block_t *code, data_block_t *binary) {
  // Allocate +rwx memory
  uint32_t (*rwx)(uint8_t*, uint8_t*) = mmap(0, code->length, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

  // Populate it with the code
  memcpy(rwx, code->data, code->length);

  // Run the code
  uint32_t result = rwx(binary->data, binary->data + binary->length);

  // Wipe and unmap the memory
  memset(rwx, 0, code->length);
  munmap(rwx, code->length);

  return result;
}
