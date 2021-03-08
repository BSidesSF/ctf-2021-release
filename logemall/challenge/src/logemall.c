#define _DEFAULT_SOURCE

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "character.h"
#include "encounters.h"
#include "asciimon.h"
#include "helpers.h"
#include "map.h"
#include "object.h"
#include "state.h"
#include "world.h"

#define disable_buffering(_fd) setvbuf(_fd, NULL, _IONBF, 0)

typedef enum {
  COMMAND_UP,
  COMMAND_DOWN,
  COMMAND_LEFT,
  COMMAND_RIGHT,
  COMMAND_HELP,
  COMMAND_QUIT,
  COMMAND_FIGHT,

  COMMAND_FLY_JADE,
  COMMAND_FLY_DEEPBLUE,
  COMMAND_FLY_SCARLET,
  COMMAND_FLY_TEAL,
  COMMAND_FLY_COLOUR,
  COMMAND_FLY_GRAY,
  COMMAND_FLY_ORANGE,

  COMMAND_DISABLE_ENCOUNTERS,
  COMMAND_ENABLE_ENCOUNTERS,
  COMMAND_SHOW_ENCOUNTER_TABLES,
  COMMAND_HIDE_ENCOUNTER_TABLES,
} command_t;

static command_t handle_input() {
  char buffer[128];
  static char previous_buffer[128] = "help";

  printf("Available commands: w a s d help quit\n");
  printf("(Hint: press <enter> to re-run previous command)\n");
  printf("Action > ");
  memset(buffer, 0, 128);
  fflush(stdin);
  if(!fgets(buffer, 127, stdin)) {
    printf("stdin closed\n");
    exit(0);
  }

  // Zero-length buffers repeat
  if(buffer[0] == '\n') {
    strncpy(buffer, previous_buffer, 128);
  } else {
    strncpy(previous_buffer, buffer, 128);
  }

  int i;
  for(i = 0; i < strlen(buffer); i++) {
    if(buffer[i] == '\n') {
      buffer[i] = '\0';
    } else {
      buffer[i] = tolower(buffer[i]);
    }
  }

  if(!strcmp(buffer, "up") || !strcmp(buffer, "w")) {
    return COMMAND_UP;
  }

  if(!strcmp(buffer, "left") || !strcmp(buffer, "a")) {
    return COMMAND_LEFT;
  }

  if(!strcmp(buffer, "down") || !strcmp(buffer, "s")) {
    return COMMAND_DOWN;
  }

  if(!strcmp(buffer, "right") || !strcmp(buffer, "d")) {
    return COMMAND_RIGHT;
  }

  if(!strcmp(buffer, "fight")) {
    return COMMAND_FIGHT;
  }

  if(buffer[0] == 'h') {
    return COMMAND_HELP;
  }

  if(buffer[0] == 'q') {
    return COMMAND_QUIT;
  }

  if(!strcmp(buffer, "fly jade"))  { return COMMAND_FLY_JADE; }
  if(!strcmp(buffer, "fly deepblue")) { return COMMAND_FLY_DEEPBLUE; }
  if(!strcmp(buffer, "fly scarlet")) { return COMMAND_FLY_SCARLET; }
  if(!strcmp(buffer, "fly teal"))   { return COMMAND_FLY_TEAL; }
  if(!strcmp(buffer, "fly colour"))   { return COMMAND_FLY_COLOUR; }
  if(!strcmp(buffer, "fly gray"))   { return COMMAND_FLY_GRAY; }
  if(!strcmp(buffer, "fly orange"))  { return COMMAND_FLY_ORANGE; }

  if(!strcmp(buffer, "encounters off"))   { return COMMAND_DISABLE_ENCOUNTERS; }
  if(!strcmp(buffer, "encounters on"))    { return COMMAND_ENABLE_ENCOUNTERS; }
  if(!strcmp(buffer, "encounters show"))  { return COMMAND_SHOW_ENCOUNTER_TABLES; }
  if(!strcmp(buffer, "encounters hide"))  { return COMMAND_HIDE_ENCOUNTER_TABLES; }

  return -1;
}

void draw(state_t *state) {
  printf("%s\n\n", state->character->name);
  printf("Map: %s\n\n", state->map->name);
  printf("%s\n\n", state->map->description);
  printf("Companion: %s\n", state->character->companion->name);
  asciimon_print(state->character->companion, 0);
  printf("\n");

  if(state->show_encounter_table) {
    encounter_table_print(state->map->encounter_table);
  }

  map_draw(state->map, state->left, state->top);
}

void check_encounters(state_t *state) {
  if(state->disable_encounters) {
    return;
  }

  if(state->map->encounter_table) {
    encounter_table_run(state->map->encounter_table, state->character, state->world, 0);
  }
}

void go_to_map(state_t *state, char *name) {
  if(!name) {
    fprintf(stderr, "Uh oh, something went wrong! There's no map in that direction!\n");
    wait_then_clear();
  } else {
    printf("Loading map: %s\n", name);
    map_t *old_map = state->map;
    state->map = map_load(name);
    map_free(old_map);
  }
}

void handle_up(state_t *state) {
  if(state->top == 0) {
    state->top = MAP_HEIGHT;
    go_to_map(state, state->map->directions[UP]);
    state->top--;
  } else {
    int top = state->top - 1;
    int left = state->left;
    if(check_bonk(state->map->map[top][left], top, left, state->character, state->map, state->world) == NO_BONK) {
      check_encounters(state);
      state->top--;
    }
  }
}

void handle_left(state_t *state) {
  if(state->left == 0) {
    state->left = MAP_WIDTH;
    go_to_map(state, state->map->directions[LEFT]);
    state->left--;
  } else {
    int top = state->top;
    int left = state->left - 1;
    if(check_bonk(state->map->map[top][left], top, left, state->character, state->map, state->world) == NO_BONK) {
      check_encounters(state);
      state->left--;
    }
  }
}

void handle_down(state_t *state) {
  if(state->top + 1 == MAP_HEIGHT) {
    state->top = 0;
    go_to_map(state, state->map->directions[DOWN]);
    check_encounters(state);
  } else {
    int top = state->top + 1;
    int left = state->left;
    if(check_bonk(state->map->map[top][left], top, left, state->character, state->map, state->world) == NO_BONK) {
      check_encounters(state);
      state->top++;
    }
  }
}

void handle_right(state_t *state) {
  if(state->left + 1 == MAP_WIDTH) {
    state->left = 0;
    go_to_map(state, state->map->directions[RIGHT]);
    check_encounters(state);
  } else {
    int top = state->top;
    int left = state->left + 1;
    if(check_bonk(state->map->map[top][left], top, left, state->character, state->map, state->world) == NO_BONK) {
      check_encounters(state);
      state->left++;
    }
  }

}

void handle_help() {
  printf("Commands:\n");
  printf("\n");
  printf(" UP    (w) - move up\n");
  printf(" LEFT  (a) - move left\n");
  printf(" DOWN  (s) - move down\n");
  printf(" RIGHT (d) - move right\n");
  printf("\n");
  printf(" QUIT  (q) - quit\n");
  printf("\n");

  wait();
}

character_t *get_character() {
  printf("What would you like to be known as?\n");
  printf("> ");
  fflush(stdout);

  char *name = malloc(sizeof(asciimon_t));
  memset(name, 0, sizeof(asciimon_t));
  fgets(name, sizeof(asciimon_t) - 1, stdin);
  if(name[strlen(name) - 1] == '\n') {
    name[strlen(name) - 1] = '\0';
  }

  for(;;) {
    printf("Which starting Asciimon would you like?\n");
    printf("Choices: Plant Frog, Fire Lizard, Water Turtle\n");
    printf("\n");

    char companion_name[32];
    if(!fgets(companion_name, 31, stdin)) {
      printf("stdin closed\n");
      exit(0);
    }

    companion_name[strlen(companion_name) - 1] = '\0';

    asciimon_t *companion = asciimon_load(companion_name);
    if(companion) {
      asciimon_print(companion, 1);
      yes_no_t response = yes_no("Are you sure?", YES);

      if(response == YES) {
        return character_new(name, companion);
      }
      free(companion);
      continue;
    }

    printf("Oops! That's an invalid choice!\n");
  }
}

int main(int argc, char *argv[]) {
  srand(152);
  disable_buffering(stdout);
  disable_buffering(stderr);

  state_t *state = (state_t*) malloc(sizeof(state_t));
  state->top = 5;
  state->left = 7;
  state->map = map_load("lab.map");
  state->character = get_character();
  state->world = world_initialize();
  state->show_encounter_table = 0;
  state->disable_encounters = 0;

  log_seen(state->world, state->character->companion->number);

  for(;;) {
    clear();
    draw(state);

    command_t command = handle_input();
    while(command == -1) {
      printf("Unknown command\n");
      command = handle_input();
    }

    switch(command) {
      case COMMAND_UP:
        handle_up(state);
        break;

      case COMMAND_LEFT:
        handle_left(state);
        break;

      case COMMAND_DOWN:
        handle_down(state);
        break;

      case COMMAND_RIGHT:
        handle_right(state);
        break;

      case COMMAND_HELP:
        handle_help();
        break;

      case COMMAND_FIGHT:
        if(state->map->encounter_table) {
          encounter_table_run(state->map->encounter_table, state->character, state->world, 1);
        }
        break;

      case COMMAND_FLY_JADE:
        go_to_map(state, "jade.map");
        break;

      case COMMAND_FLY_DEEPBLUE:
        go_to_map(state, "deepblue.map");
        break;

      case COMMAND_FLY_SCARLET:
        go_to_map(state, "scarlet.map");
        break;

      case COMMAND_FLY_TEAL:
        go_to_map(state, "teal.map");
        break;

      case COMMAND_FLY_COLOUR:
        go_to_map(state, "colour.map");
        break;

      case COMMAND_FLY_GRAY:
        go_to_map(state, "gray.map");
        break;

      case COMMAND_FLY_ORANGE:
        go_to_map(state, "orange.map");
        break;

      case COMMAND_DISABLE_ENCOUNTERS:
        state->disable_encounters = 1;
        break;

      case COMMAND_ENABLE_ENCOUNTERS:
        state->disable_encounters = 0;
        break;

      case COMMAND_SHOW_ENCOUNTER_TABLES:
        state->show_encounter_table = 1;
        break;

      case COMMAND_HIDE_ENCOUNTER_TABLES:
        state->show_encounter_table = 0;
        break;

      case COMMAND_QUIT:
        return 0;

      default:
        fprintf(stderr, "Something went wrong in command handler, unknown command: %d\n", command);
        exit(1);
    }
  }
}
