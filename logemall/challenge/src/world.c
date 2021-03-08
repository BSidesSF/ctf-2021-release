#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// For DATA_PATH
#include "asciimon.h"

#include "world.h"

static char *read_line(FILE *f) {
  char *buf = malloc(ASCIIMON_LINE_LENGTH);

  for(;;) {
    // Read, make sure there's no error
    if(!fgets(buf, ASCIIMON_LINE_LENGTH, f)) {
      return NULL;
    }

    // Ignore these
    if(strlen(buf) == 0 || buf[0] == '\n' || buf[0] == ';') {
      continue;
    }

    // Remove null
    buf[strlen(buf) - 1] = '\0';

    return buf;
  }
}

world_t *world_initialize() {
  world_t *world = (world_t*) malloc(sizeof(world_t));

  FILE *f = fopen(DATA_PATH"asciimon.csv", "r");
  if(!f) {
    printf("Couldn't load encounter file: "DATA_PATH"asciimon.csv");
    exit(1);
  }

  // Initialize the list
  int i;
  for(i = 0; i < LOGGED_COUNT; i++) {
    strcpy(world->logged[i].asciimon, "[Data Not Found]");
    world->logged[i].logged = 0;
  }

  for(;;) {
    // Read the line
    char *buf = read_line(f);
    if(!buf) {
      break;
    }

    // Tokenize it badly
    int num = atoi(strtok(buf, ","));
    strcpy(world->logged[num].asciimon, strtok(NULL, ","));

    // It has not been logged
    world->logged[num].logged = 0;

    free(buf);
  }

  fclose(f);

  return world;
}

void log_seen(world_t *world, int number) {
  if(number >= 0 && number < LOGGED_COUNT) {
    world->logged[number].logged = 1;
  }
}

int is_logged(world_t *world, int number) {
  if(number >= 0 && number < LOGGED_COUNT) {
    return world->logged[number].logged;
  }
  return 0;
}

int seen_count(world_t *world) {
  int count = 0;
  int i;
  for(i = 0; i < LOGGED_COUNT; i++) {
    if(world->logged[i].logged) {
      count++;
    }
  }

  return count;
}

