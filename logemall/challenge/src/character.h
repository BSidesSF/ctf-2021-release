#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "asciimon.h"

typedef struct {
  char *name;
  asciimon_t *companion;
} character_t;

character_t *character_new(char *name, asciimon_t *companion);

#endif
