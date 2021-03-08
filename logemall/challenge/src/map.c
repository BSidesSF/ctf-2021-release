#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "colors.h"
#include "encounters.h"

#include "map.h"

static char *read_line(FILE *f) {
  char *buf = malloc(256);

  for(;;) {
    // Read, make sure there's no error
    if(!fgets(buf, 256, f)) {
      fprintf(stderr, "Error reading map\n");
      exit(1);
    }

    // Ignore these
    if(strlen(buf) == 0 || buf[0] == '\n' || buf[0] == ';') {
      continue;
    }

    // Remove newline
    buf[strlen(buf) - 1] = '\0';

    // Have a null type
    if(!strcmp("none", buf)) {
      return NULL;
    }

    return buf;
  }
}

map_t *map_load(char *filename) {
  char path[128] = MAP_PATH;
  strncat(path, filename, 128);

  FILE *f = fopen(path, "r");
  if(!f) {
    fprintf(stderr, "Map file not found: %s\n", path);
    exit(1);
  }

  map_t *new_map = malloc(sizeof(map_t));
  new_map->name = read_line(f);
  new_map->description = read_line(f);
  new_map->directions[UP]    = read_line(f);
  new_map->directions[LEFT]  = read_line(f);
  new_map->directions[DOWN]  = read_line(f);
  new_map->directions[RIGHT] = read_line(f);

  int i;
  for(i = 0; i < MAP_HEIGHT; i++) {
    new_map->map[i] = read_line(f);
  }

  char *encounter_file = read_line(f);
  new_map->encounter_table = NULL;
  if(encounter_file) {
    printf("Encounters loaded:\n");
    new_map->encounter_table = encounter_table_load(encounter_file);
    encounter_table_print(new_map->encounter_table);
    free(encounter_file);
  }

  return new_map;
}

void map_draw(map_t *map, int left, int top) {
  int i;
  for(i = 0; i < MAP_HEIGHT; i++) {
    int j;
    for(j = 0; j < MAP_WIDTH; j++) {
      if(j == left && i == top) {
        printf(RESET YEL"%c", ME);
      } else {
        // Add fun colours
        switch(map->map[i][j]) {
          case TREE:      printf(RESET HGRN);      break;
          case WATER:     printf(RESET BLU BLUB);  break;
          case DOOR:      printf(RESET REDB BLU);  break;
          case WALL:      printf(RESET BLKHB);     break;
          case PROFESSOR: printf(RESET YEL);       break;
          case PC:        printf(RESET YEL);       break;
          case MOUNTAIN:  printf(RESET HBLK);      break;
          case CAVE_WALL: printf(RESET HBLK);      break;
          case BARRICADE: !strcmp(map->name, "Final Fight") ? printf(RESET) : printf(RESET YELB);      break;
          default:        printf(RESET);
        }

        printf("%c", map->map[i][j]);
      }
    }
    printf("\n");
  }
  printf(RESET"\n");
}

void map_free(map_t *map) {
  free(map->name);
  free(map->description);
  free(map->directions[UP]);
  free(map->directions[LEFT]);
  free(map->directions[DOWN]);
  free(map->directions[RIGHT]);

  int i;
  for(i = 0; i < MAP_HEIGHT; i++) {
    free(map->map[i]);
  }

  if(map->encounter_table) {
    encounter_table_free(map->encounter_table);
  }

  free(map);
}
