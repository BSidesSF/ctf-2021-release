#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "helpers.h"

#include "asciimon.h"

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

asciimon_t *asciimon_load(char *name) {
  // Allocate for the asciimon
  asciimon_t *asciimon = (asciimon_t *) malloc(sizeof(asciimon_t));
  char portrait_path[48] = DATA_PATH;

  // Inefficiently find the asciimon name in the list
  FILE *f = fopen(DATA_PATH"asciimon.csv", "r");
  if(!f) {
    fprintf(stderr, "Couldn't load asciimon file!\n");
    exit(1);
  }

  char *raw;
  for(;;) {
    raw = read_line(f);
    if(!raw) {
      fprintf(stderr, "Could not load asciimon: %s\n", name);
      return NULL;
    }

    // Do ugly parsing using a simple strtok() with no error handling
    char *number = strtok(raw, ",");
    char *row_name = strtok(NULL, ",");

    if(!strcasecmp(name, row_name)) {
      // Close the file
      fclose(f);

      // Save the name and number (from the row)
      strcpy(asciimon->name, row_name);
      asciimon->number = atoi(number);

      // Get the portrait path
      strcat(portrait_path, number);
      strcat(portrait_path, ".txt");

      break;
    }

    // If it wasn't this line, free it and keep going
    free(raw);
  }

  strtok(NULL, ","); // Type 1
  strtok(NULL, ","); // Type 2
  strtok(NULL, ","); // Total stats
  asciimon->hp = atoi(strtok(NULL, ",")); // HP
  asciimon->attack = atoi(strtok(NULL, ",")); // Attack
  asciimon->defense = atoi(strtok(NULL, ",")); // Defense
  strtok(NULL, ","); // Sp. Attack
  strtok(NULL, ","); // Sp. Def
  asciimon->speed = atoi(strtok(NULL, ",")); // Speed
  strtok(NULL, ","); // Generation
  strtok(NULL, ","); // Legacy?

  // This will prevent it from being freed
  asciimon->caught = 0;

  // Read the portrait from a separate file
  asciimon->portrait = read_file(portrait_path);

  // Free the memory
  free(raw);

  return asciimon;
}

void asciimon_print(asciimon_t *asciimon, int display_portrait) {
  printf("[%d] Max HP = %d; Attack = %d; defense = %d; speed = %d\n", asciimon->number, asciimon->hp, asciimon->attack, asciimon->defense, asciimon->speed);

  if(display_portrait) {
    printf("%s\n", asciimon->portrait);
  }
}

void asciimon_free(asciimon_t *asciimon) {
  free(asciimon->portrait);
  free(asciimon);
}
