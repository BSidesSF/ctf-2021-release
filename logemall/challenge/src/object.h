#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "map.h"
#include "world.h"

typedef enum {
  BONK,
  NO_BONK,
} object_result_t;

object_result_t check_bonk(char o, int top, int left, character_t *character, map_t *map, world_t *world);

#endif
