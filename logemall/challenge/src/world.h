#ifndef __WORLD_H__
#define __WORLD_H__

#define LOGGED_COUNT 152

#include "asciimon.h"

typedef struct {
  // Asciimon's name
  char asciimon[MAX_ASCIIMON_NAME];

  // Has it been logged?
  int logged;
} logged_t;

typedef struct {
  logged_t logged[LOGGED_COUNT];
} world_t;

world_t *world_initialize();
void log_seen(world_t *world, int number);
int is_logged(world_t *world, int number);
int seen_count(world_t *world);

#endif
