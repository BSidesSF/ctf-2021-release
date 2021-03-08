#ifndef __ENCOUNTER_C__
#define __ENCOUNTER_C__

#ifndef ENCOUNTER_PATH
#define ENCOUNTER_PATH "/home/ctf/encounters/"
#endif

#include "character.h"
#include "asciimon.h"
#include "world.h"

#define MAX_ENCOUNTER_TABLE 8

typedef struct {
  int count;
  int encounter_chance;
  asciimon_t *enemies[MAX_ENCOUNTER_TABLE];
} encounter_table_t;

encounter_table_t *encounter_table_load(char *filename);
void encounter_table_print(encounter_table_t *encounter_table);
void encounter_table_run(encounter_table_t *encounter_table, character_t *character, world_t *world, int force);
void encounter_table_free(encounter_table_t *encounter_table);

#endif
