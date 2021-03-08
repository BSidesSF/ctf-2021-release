#include <stdlib.h>

#include "character.h"

character_t *character_new(char *name, asciimon_t *companion) {
  character_t *character = (character_t*) malloc(sizeof(character_t));
  character->name = name;
  character->companion = companion;

  return character;
}
