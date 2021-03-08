#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "asciimon.h"
#include "helpers.h"
#include "state.h"

#include "encounters.h"

typedef enum {
  COMMAND_ATTACK,
  COMMAND_DEFEND,
  COMMAND_RUN,
  COMMAND_HELP,
  COMMAND_QUIT,
} command_t;

static command_t handle_input() {
  char buffer[128];
  static char previous_buffer[128] = "help";

  printf("Available commands: attack defend run help\n");
  printf("(Hint: press <enter> to re-run previous command)\n");
  printf("Combat Action > ");
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

  if(tolower(buffer[0]) == 'a') {
    return COMMAND_ATTACK;
  }

  if(tolower(buffer[0]) == 'd') {
    return COMMAND_DEFEND;
  }

  if(tolower(buffer[0]) == 'r') {
    return COMMAND_RUN;
  }

  if(tolower(buffer[0]) == 'h') {
    return COMMAND_HELP;
  }

  if(tolower(buffer[0]) == 'q') {
    return COMMAND_QUIT;
  }

  return -1;
}

typedef enum {
  SCARED,
  TIMID,
  NORMAL,
  ANGRY,
  ENRAGED,
  DISPOSITION_COUNT,
} disposition_t;

static char *disposition_to_string(disposition_t disposition) {
  switch(disposition) {
    case SCARED:  return "Scared";
    case TIMID:   return "Timid";
    case NORMAL:  return "Normal";
    case ANGRY:   return "Angry";
    case ENRAGED: return "Enraged";
    default:      return "Unknown";
  }
}

static int odds_of_attacking(disposition_t disposition) {
  switch(disposition) {
    case SCARED:  return 10;
    case TIMID:   return 25;
    case NORMAL:  return 50;
    case ANGRY:   return 75;
    case ENRAGED: return 90;
    default:      return 100;
  }
}

static char *read_line(FILE *f) {
  char *buf = malloc(256);

  for(;;) {
    // Read, make sure there's no error
    if(!fgets(buf, 256, f)) {
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

encounter_table_t *encounter_table_load(char *filename) {
  char path[128] = ENCOUNTER_PATH;
  strncat(path, filename, 128);

  FILE *f = fopen(path, "r");
  if(!f) {
    fprintf(stderr, "Encounter file not found: %s\n", filename);
    exit(1);
  }

  // Allocate memory
  encounter_table_t *encounters = (encounter_table_t *) malloc(sizeof(encounter_table_t));

  // No encounters yet
  encounters->count = 0;

  // Read the odds as an integer
  char *odds = read_line(f);
  encounters->encounter_chance = atoi(odds);
  free(odds);

  // Read encounters
  for(;;) {
    // Read the raw encounter
    char *name = read_line(f);

    // Exit if we are at the end of file
    if(!name) {
      break;
    }

    // Increment encounters
    asciimon_t *asciimon = asciimon_load(name);
    if(!asciimon) {
      printf("Asciimon failed to load.. bad encounter table?\n");
      exit(0);
    }
    encounters->enemies[encounters->count++] = asciimon_load(name);
    free(name);
  }

  fclose(f);

  return encounters;
}

// TODO: I can probably get rid of this
void encounter_table_print(encounter_table_t *encounter_table) {
  if(!encounter_table) {
    printf("No encounters here\n");
    return;
  }

  printf("Chance of encounter: %d%%\n", encounter_table->encounter_chance);

  int i;
  for(i = 0; i < encounter_table->count; i++) {
    printf("%s ", encounter_table->enemies[i]->name);
    asciimon_print(encounter_table->enemies[i], 0);
  }
  printf("\n");
}

static int calculate_damage(double attack, double defense) {
  double rand_value = ((rand() % 15) + 85.0) / 100.0;
  return (int)((((144.0 * attack / defense) / 50) + 2) * rand_value);
}

void encounter_table_run(encounter_table_t *encounter_table, character_t *character, world_t *world, int force) {
  if(!force && (rand() % 100 > encounter_table->encounter_chance)) {
    return;
  }

  asciimon_t *enemy = encounter_table->enemies[rand() % encounter_table->count];
  disposition_t disposition = rand() % DISPOSITION_COUNT;

  asciimon_t *companion = character->companion;

  clear();
  printf("A %s %s draws near!\n", disposition_to_string(disposition), enemy->name);

  if(!is_logged(world, enemy->number)) {
    printf("    Wow! You haven't seen a %s before! You make a note\n", enemy->name);
    printf("\n");
    log_seen(world, enemy->number);
  }

  printf("Your %s gets ready!\n", companion->name);
  printf("\n");

  int enemy_hp = enemy->hp;
  int companion_hp = companion->hp;
  int escape_attempts = 0;
  while(enemy_hp > 0) {
    wait_then_clear();
    printf("Enemy %s:\n", enemy->name);
    asciimon_print(enemy, 1);
    printf("%d/%d\n\n", enemy_hp, enemy->hp);

    printf("Your %s:\n", companion->name);
    asciimon_print(companion, 1);
    printf("%d/%d\n\n", companion_hp, companion->hp);

    int is_attacking = ((rand() % 100) < odds_of_attacking(disposition));
    int enemy_damage;
    int defense = enemy->defense;

    if(is_attacking) {
      enemy_damage = calculate_damage(enemy->attack, companion->defense);
      printf("The enemy %s starts winding up an attack for %d damage!\n", enemy->name, enemy_damage);
    } else {
      printf("The enemy %s steels itself to defend!\n", enemy->name);
      defense *= 2;
    }
    printf("\n");
    printf("What do you do? (a = attack, d = defend, r = run)\n\n> ");
    fflush(stdout);

    command_t action = handle_input();
    if(action == COMMAND_ATTACK) {
      if(rand() % 256 == 0) {
        printf("Whiff! You somehow missed!\n");
      } else {
        int player_damage = calculate_damage(companion->attack, defense);
        printf("You attack for %d damage!\n", player_damage);
        if(player_damage >= enemy_hp) {
          break;
        } else {
          enemy_hp -= player_damage;
        }
      }

      // Reset escape attempts
      escape_attempts = 0;
    } else if(action == COMMAND_DEFEND) {
      printf("You defend!\n");

      // Recalculate damage
      enemy_damage = calculate_damage(enemy->attack, companion->defense * 2);

      printf("The enemy ends up doing %d damage\n", enemy_damage);
    } else if(action == COMMAND_RUN) {
      if(enemy->speed < companion->speed) {
        printf("You got away!\n");
    wait_then_clear();
        return;
      }

      int escape_chance = ((int)(30 * ++escape_attempts)) + (((double)(companion->speed * 32)) / ((double) enemy->speed / 4));
      if(rand() % 256 < escape_chance) {
        printf("You got away!\n");
        wait_then_clear();
        return;
      }

      printf("You try to escape but fail!\n");
    } else if(action == COMMAND_HELP) {
      printf("This is the combat interface!\n");
      printf("\n");
      printf("(A)ttack: Your companion will attack the opponent using his attack trait against their defense\n");
      printf("(D)efend: Your companion will defend itself, effectively doubling its defense trait\n");
      printf("(R)un: You will attempt to escape; success rate is based on comparing your speed stat to theirs\n");
      printf("(Q)uit: Exit the game\n");
      printf("(H)elp: Hi\n");
      printf("\n");
    } else if(action == COMMAND_QUIT) {
      printf("Bye!\n");
      exit(0);
    } else {
      printf("You twiddle your thumbs\n");
      continue;
    }

    // Enemy attack
    if(is_attacking) {
      printf("The enemy hits you for %d damage!\n", enemy_damage);
      if(enemy_damage >= companion_hp) {
        printf("Your companion faints. Game over! :(\n\n");
        exit(0);
      }
      companion_hp -= enemy_damage;
    } else {
      printf("The %s is defending!\n", enemy->name);
    }
  }

  wait_then_clear();
  printf("The enemy's %s faints! You are victorious!\n\n", enemy->name);

  printf("Would you to replace your %s? [y/N]\n\n", companion->name);

  char response[8];
  fgets(response, 8, stdin);

  if(response[0] == 'y') {
    // VULN: This uses the literal enemy, and can free it
    character->companion = enemy;
    enemy->caught = 1;
    free(companion);
  }
}

void encounter_table_free(encounter_table_t *encounter_table) {
  int i;
  for(i = 0; i < encounter_table->count; i++) {
    // Only free if it hasn't been caught (this half fixes the vuln)
    if(!encounter_table->enemies[i]->caught) {
      asciimon_free(encounter_table->enemies[i]);
    }
  }
  free(encounter_table);
}
