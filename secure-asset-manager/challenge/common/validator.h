#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_

#include <stdint.h>

typedef struct {
  uint8_t *data;
  size_t length;
} data_block_t;

uint32_t checksum(data_block_t *code, data_block_t *binary);

#endif
