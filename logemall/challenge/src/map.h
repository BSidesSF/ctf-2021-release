#ifndef __MAP_T__
#define __MAP_T__

#define MAP_WIDTH 20
#define MAP_HEIGHT 10

#ifndef MAP_PATH
#define MAP_PATH "/home/ctf/maps/"
#endif

#include "encounters.h"

#define ME         'O'

#define WALL       '#'
#define DOOR       'D'
#define PC         'P'
#define TREE       't'
#define MOUNTAIN   'M'
#define CAVE_WALL  '^'
#define BARRICADE  'B'
#define WATER      'W'

// Lab
#define PROFESSOR  'p'

// Colour
#define MOM        'm'
#define RIVAL      'i'

// Victory
#define CHAMPION   'c'

// Teal
#define ELITE1     '1'
#define ELITE2     '3'
#define ELITE4     '7'

// Gray
#define GYM_LEADER 'g'
#define MUSEUM     'u'

// Mount Earth
#define TRAINER    'T'

// DeepBlue
#define NAME_RATER 'r'
#define BIKE_SHOP  'b'
#define POOL       '+'

// Ron's house
#define RON 'o'

// Orange
#define SECRETARY  's'
#define ELEVATOR   'E'
#define DESK       '-'

// Jade
#define MALL_WORKER 'w'

// Scarlet
#define WEIRDNESS '?'
#define VOLCANO   'v'

// Misc
#define HIDDEN 'H'


typedef enum {
  UP,
  LEFT,
  DOWN,
  RIGHT,
} direction_t;

typedef struct {
  // Which map is in the other 4 directions?
  char *directions[4];

  // Name of the map
  char *name;

  // A short description
  char *description;

  // The actual construction
  char *map[MAP_HEIGHT];

  // Encounter table (optional)
  encounter_table_t *encounter_table;
} map_t;

map_t *map_load(char *filename);
void map_draw(map_t *map, int left, int top);
void map_free(map_t *map);

#endif
