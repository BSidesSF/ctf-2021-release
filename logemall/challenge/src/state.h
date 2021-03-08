#ifndef __STATE_H__
#define __STATE_H__

#include "character.h"
#include "map.h"
#include "world.h"

typedef struct {
  map_t *map;
  int top;
  int left;
  character_t *character;
  world_t *world;

  int show_encounter_table;
  int disable_encounters;
} state_t;

#endif
