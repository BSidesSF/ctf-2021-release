#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "character.h"
#include "colors.h"
#include "helpers.h"
#include "world.h"

#include "object.h"

#ifndef FLAG_PATH
#define FLAG_PATH "/home/ctf/flag.txt"
#endif

#define PAUSE "\x01"

static void npc_talk(char **dialogue) {
  clear();

  int i;
  for(i = 0; dialogue[i]; i++) {
    if(dialogue[i][0] == PAUSE[0]) {
      printf("\n");
      wait();
    } else {
      printf("%s\n", dialogue[i]);
    }
  }
  printf("\n");
  wait_then_clear();
}


static void handle_wall(char *map, int top, int left) {
  char *dialogue[] = {
      "You bonk into the wall",
      0,
  };
  npc_talk(dialogue);
}

static void handle_door(char *map, int top, int left) {
  char *dialogue[] = {
      "You open the door",
      0,
  };
  npc_talk(dialogue);
}

static void handle_pc(world_t *world) {
  clear();

  printf(" ****************************************\n");
  printf(" ************ ACCESSING PC **************\n");
  printf(" ****************************************\n");
  printf("\n");
  printf("Loaded list of logged Asciimon; press <enter> to view...\n");

  int i;
  for(i = 1; i < LOGGED_COUNT; i++) {
    printf("[%3d] %32s: %s"RESET"\n", i, world->logged[i].asciimon, world->logged[i].logged ? HGRN"LOGGED" : HRED"NOT LOGGED");

    if((i % 31) == 0) {
      wait();
    }
  }
  printf("\n");
  wait_then_clear();
}

static void handle_tree(char *map, int top, int left) {
  char *dialogue[] = {
      "You bonk into the tree",
      0,
  };
  npc_talk(dialogue);
}

static void handle_mountain(char *map, int top, int left) {
  char *dialogue[] = {
      "You start climbing, but mountains are big",
      0,
  };
  npc_talk(dialogue);
}

static void handle_cave_wall(char *map, int top, int left) {
  char *dialogue[] = {
      "You start climbing the cave wall, but the ceiling is low!",
      0,
  };
  npc_talk(dialogue);
}

static void handle_barricade(char *map, int top, int left) {
  if(!strcasecmp(map, "Teal Plateau")) {
    char *dialogue[] = {
        "There is a barricade, with a door behind it. You hear exciting fighting",
        "from behind it, and wonder what it'd be like to be playing THAT game!",
        0,
    };
  npc_talk(dialogue);
  } else {
    char *dialogue[] = {
        "A barricade is blocking your way!",
        0,
    };
    npc_talk(dialogue);
  }
}

static object_result_t handle_water(char *map, int top, int left) {
  char *dialogue[] = {
      "Splash!",
      0,
  };
  npc_talk(dialogue);

  // Allow them to enter the pool
  if(!strcasecmp(map, "DeepBlue City")) {
    return NO_BONK;
  } else {
    return BONK;
  }
}

static void handle_professor(character_t *character, world_t *world) {
  static int seen_professor = 0;
  int count = seen_count(world);

  clear();

  if(seen_professor) {
    // Can't use npc_talk() here since it's not static
    printf(
        "The professor looks happy to see you again!\n"
        "\n"
        "\"Welcome back, %s!\" he says, \"Let me have a look at your Asciimon collection!\"\n", character->name);
        "\n",

    wait();

    printf(
        "\n"
        "\"1.. 2.... 3....... wow! You have logged %d out of %d Asciimon so far!\"\n"
        "\n",
        count, LOGGED_COUNT - 1
    );
  } else {
    printf("A man stands before you. His beard is as long as it is\n");
    printf("glorious, and he's wearing a festive Hawaiian shirt!\n");
    printf("\n");
    wait();

    printf("\"Well hi there %s!\" says the man, \"my name is Professor Jack.\n", character->name);
    printf("welcome to my Lab.\"\n");
    printf("\n");
    wait();

    printf("\"So, I'm working on a project, but I need you to log all %d\n", LOGGED_COUNT - 1);
    printf("Asciimon found in the wild. Well, some are in the wild. I'm not sure\n");
    printf("where you'll find the others. But I'm sure you can do it!\n");
    printf("\n");
    wait();

    printf("You agree, of course. Who could say no to Professor Jack?\n");
    printf("\n");
    wait();

    printf("\"Come back when you log 'em all!\n");
    printf("\n");
    seen_professor = 1;
  }

  if(count >= LOGGED_COUNT - 1) {
    char flag[64];
    FILE *f = fopen(FLAG_PATH, "r");
    if(!f) {
      fprintf(stderr, "Couldn't open the flag file: %s\n", FLAG_PATH);
      exit(1);
    }

    fgets(flag, 64, f);
    fclose(f);

    printf("\"Wow!!! That's all of them!!\" he exclaims, and reaches into his\n"
        "pack. \"This is for you: %s\"", flag);
  }

  wait_then_clear();
}

static void handle_mom(char *map, int top, int left) {
  char *dialogue[] = {
      "\"Oh hi!\" says the lady, \"I'm Red's Mom. He's not home now, he went",
      "to fulfill his destiny!\"",
      0,
  };
  npc_talk(dialogue);
}

static void handle_rival(char *map, int top, int left) {
  char *dialogue[] = {
      "\"Oh hi!\" says the lady, \"I'm Blue's Mom. He's not home now, he went",
      "to fulfill his destiny\"",
      0,
  };
  npc_talk(dialogue);
}

static void handle_champion(char *map, int top, int left) {
  // 1 14
  // 1 16
  if(left == 14) {
    char *dialogue[] = {
        "There's a man here. He says:",
        "\n",
        "\"They see that Teal Plateau ahead is where the Asciimon Champion",
        "has their final battle.\"",
        PAUSE,
        "He continues:",
        PAUSE,
        "\"Not you, though. You look like you'd be better off cheating.\"",
        0,
    };
    npc_talk(dialogue);
  } else {
    char *dialogue[] = {
        "There's a man here. He says:",
        "\n",
        "\"We thought we'd be champions once, but the 1337 4 were too good!\"",
        0,
    };
    npc_talk(dialogue);
  }
}

static void handle_elite4(char *map, int top, int left) {
  if(top == 3) {
    char *dialogue[] = {
        "The former 1337 says:",
        "\n"
        "\"We used to be the champions.\"",
        0,
    };
    npc_talk(dialogue);
  } else if(top == 4) {
    char *dialogue[] = {
        "The former 1337 says:",
        "\n"
        "\"Some kid came by. We weren't prepared for youthful strategies.\"",
        0,
    };
    npc_talk(dialogue);
  } else if(top == 5) {
    char *dialogue[] = {
        "The former 1337 says:",
        "\n"
        "\"In our day, we could work hard and get ahead. These days, you need",
        "to be born lucky!\"",
        0,
    };
    npc_talk(dialogue);
  } else if(top == 6) {
    char *dialogue[] = {
        "The former 1337 says:",
        "\n"
        "\"I wish the world revolved around us the way it revolves around Red",
        "and Blue\"",
        0,
    };
    npc_talk(dialogue);
  }
}

static void handle_gym_leader(char *map, int top, int left) {
  char *dialogue[] = {
      "\"Are you the gym leader?\" you ask, hesitantly.",
      PAUSE,
      "\"I was\", he says, dejectedly. \"but some kid came through and kicked",
      "all our butts! Now I'm just cleaning up.\"",
      PAUSE,
      "Poor Brock...",
      0,
  };
  npc_talk(dialogue);
}

static void handle_museum(world_t *world, char *map, int top, int left) {
  clear();
  printf("The museum curator says:\n");
  printf("\n");
  printf("\"Welcome to the AsciiMon museum! Would you like to see our exhibits?\"");
  printf("\n");

  yes_no_t answer = yes_no("What do you say?", YES);

  if(answer == YES) {
    int i;

    clear();

    printf("Oct   Dec   Hex   Char                       Asciimon\n");
    printf("─────────────────────────────────────────────────────────────────\n");
    for(i = 1; i < 152; i++) {
      // Use a writeable buffer for the default
      char buf[32];
      char *c = buf;

      if(i == 0)       { c = "NUL '\\0' (null character) "; }
      else if(i == 1)  { c = "SOH (start of heading)     "; }
      else if(i == 2)  { c = "STX (start of text)        "; }
      else if(i == 3)  { c = "ETX (end of text)          "; }
      else if(i == 4)  { c = "EOT (end of transmission)  "; }
      else if(i == 5)  { c = "ENQ (enquiry)              "; }
      else if(i == 6)  { c = "ACK (acknowledge)          "; }
      else if(i == 7)  { c = "BEL '\\a' (bell)            "; }
      else if(i == 8)  { c = "BS  '\\b' (backspace)       "; }
      else if(i == 9)  { c = "HT  '\\t' (horizontal tab)  "; }
      else if(i == 10) { c = "LF  '\\n' (new line)        "; }
      else if(i == 11) { c = "VT  '\\v' (vertical tab)    "; }
      else if(i == 12) { c = "FF  '\\f' (form feed)       "; }
      else if(i == 13) { c = "CR  '\\r' (carriage ret)    "; }
      else if(i == 14) { c = "SO  (shift out)            "; }
      else if(i == 15) { c = "SI  (shift in)             "; }
      else if(i == 16) { c = "DLE (data link escape)     "; }
      else if(i == 17) { c = "DC1 (device control 1)     "; }
      else if(i == 18) { c = "DC2 (device control 2)     "; }
      else if(i == 19) { c = "DC3 (device control 3)     "; }
      else if(i == 20) { c = "DC4 (device control 4)     "; }
      else if(i == 21) { c = "NAK (negative ack.)        "; }
      else if(i == 22) { c = "SYN (synchronous idle)     "; }
      else if(i == 23) { c = "ETB (end of trans. blk)    "; }
      else if(i == 24) { c = "CAN (cancel)               "; }
      else if(i == 25) { c = "EM  (end of medium)        "; }
      else if(i == 26) { c = "SUB (substitute)           "; }
      else if(i == 27) { c = "ESC (escape)               "; }
      else if(i == 28) { c = "FS  (file separator)       "; }
      else if(i == 29) { c = "GS  (group separator)      "; }
      else if(i == 30) { c = "RS  (record separator)     "; }
      else if(i == 31) { c = "US  (unit separator)       "; }
      else if(i == 32) { c = "SPACE                      "; }
      else if(i > 127) { c = "?                          "; }
      else             { sprintf(buf, "%c                          ", i); }

      printf("%03o   %3d   %02x   %s %s\n", i, i, i, c, world->logged[i].asciimon);

      if(i > 0 && (i % 32) == 0) {
        printf("\n");
        wait();
      }
    }
    wait();

    char *dialogue[] = {
        "\"How cool was that?! You can get it yourself by typing 'man ascii',",
        "and that's your lesson of the day!\"",
        0,
    };
    npc_talk(dialogue);
  } else {
    char *dialogue[] = {
        "\"Have a nice day, then!\"",
        0,
    };
    npc_talk(dialogue);
  }
}

static void handle_trainer(world_t *world, char *map, int top, int left) {
  clear();
  printf("A bored looking woman stands here...\n");
  printf("\n");
  printf("\"Hi, I'm what they call a trainer!\" she says.\n");
  printf("\n");
  printf("\"In all the excitement and everything, I guess Red forgot to fight\n");
  printf("me. It's kinda dark in here, so I don't blame him.\n");
  printf("\n");
  printf("Do you want to fight me?\"\n");
  printf("\n");

  yes_no_t response = yes_no("Would you like to fight?", YES);
  if(response == YES) {
    char *dialogue[] = {
        "She starts to get ready, then heaves a sigh",
        "\n"
        "\"You know what? This is just too much. Just go.\"",
        PAUSE,
        "You kinda wonder what happened. You have a feeling that a lazy",
        "programmer can probably be blamed for this one.",
        0,
    };
    npc_talk(dialogue);
  } else {
    char *dialogue[] = {
        "She replies",
        "\n"
        "\"That's okay. I wasn't in the mood anyways.\"",
        0,
    };
  }
}

static void handle_name_rater(character_t *character) {
  clear();
  printf("A weird man stands here...\n");
  printf("\n");
  printf("\"Hello, hello! I am the official Name Rater! Want me to rate your nickname?\"\n");
  printf("\n");
  printf("(You have a weird feeling this isn't how it normally works...\n");
  printf("\n");
  yes_no_t response = yes_no("Would you like him to rate your name?", YES);

  clear();
  if(response == NO) {
    printf("He replies:\n");
    printf("\n");
    printf("\"Fine! Come anytime you like!\"\n");
  } else {
    printf("\"%s, is it? That is a decent nickname! But, would you like me to\n", character->name);
    printf("give you a nicer nickname? How about it?\"\n");
    printf("\n");
    response = yes_no("What do you answer?", YES);

    clear();
    if(response == NO) {
      printf("\"Fine! Come anytime you like!\"\n");
    } else {
      printf("\"Fine! What would you like your nickname to be?\"\n");
      printf("\n");
      printf("> ");

      // Get the new name - allocate exactly as much memory as the asciimon type
      // takes (this is a bit of a cheat to make the exploit work more reliably)
      char *new_name = (char*) malloc(sizeof(asciimon_t));
      memset(new_name, 0, sizeof(asciimon_t));
      if(!fgets(new_name, sizeof(asciimon_t) - 1, stdin)) {
        printf("Could not read from stdin\n");
        exit(1);
      }

      if(new_name[strlen(new_name) - 1] == '\n') {
        new_name[strlen(new_name) - 1] = '\0';
      }

      printf("\"So you want to change '%s' to '%s'?\"\n", character->name, new_name);
      response = yes_no("Is that right?", YES);
      clear();

      if(response == NO) {
        printf("\"OK! You're still %s!\"\n", character->name);
        free(new_name);
      } else{
        // Free the old name
        free(character->name);
        character->name = new_name;

        printf("\"OK! From now on, you'll be called %s! That's a better name than before!\"\n", character->name);
      }
    }
  }
}


static void handle_bike_shop(char *map, int top, int left) {
  char *dialogue[] = {
      "You enter the bike shop. The proprietor says:",
      "",
      "\"Hi, we rent bikes here! The last kid who rented one never brought",
      "it back, though, so we're currently outta stock.\"",
      0,
  };
  npc_talk(dialogue);
}

static void handle_pool(char *map, int top, int left) {
  char *dialogue[] = {
      "You approach the pool. It's wet.",
      0,
  };
  npc_talk(dialogue);
}

static void handle_ron(char *map, int top, int left) {
  char *dialogue[] = {
      "The group looks up.",
      PAUSE,
      "\"Hi, we're the Rons. Welcome to Our house!",
      PAUSE,
      "You're probably looking for advice. What you need to do is, collect all",
      "of the Asciimon. Since you're already peering through the fourth wall,",
      "I can tell you: they aren't all available in the wild. You're gonna have",
      "to find another way.",
      PAUSE,
      "Once you have all 151 (or 152 if you're REALLY good), head back to",
      "Professor Jack and he'll give you your reward!",
      PAUSE,
      "You're welcome to check the PC in the corner to see how many you have",
      "It's the same interface as Professor Jack's!",
      PAUSE,
      "By the way, did you notice the secret 'fight' command? There are other",
      "secret quality of life commands, too!\"",
      0,
  };
  npc_talk(dialogue);
}

static void handle_secretary(char *map, int top, int left) {
  char *dialogue[] = {
      "There's a lady behind the reception desk",
      PAUSE,
      "\"Welcome to the Big Evil Corporation! Some kid just saved us from the",
      "bad guys. We're currently closed to clean up the mess!\"",
      PAUSE,
      "\"Have a great day!\"",
      0,
  };
  npc_talk(dialogue);
}

static void handle_elevator(char *map, int top, int left) {
  char *dialogue[] = {
      "The elevator is disabled",
      0,
  };
  npc_talk(dialogue);
}

static void handle_mall_worker(char *map, int top, int left) {
  char *dialogue[] = {
      "You meet a bored looking worker.",
      PAUSE,
      "\"Welcome to the Ascii Mall. Please don't break anything.\"",
      0,
  };
  npc_talk(dialogue);
}

static void handle_weirdness(char *map, int top, int left) {
  char *dialogue[] = {
      "There's something weird about the east side of this island, but you",
      "can't put your finger on it",
      PAUSE,
      "You decide it's probably from a different game.",
      0,
  };
  npc_talk(dialogue);
}

static void handle_volcano(char *map, int top, int left) {
  char *dialogue[] = {
      "This island used to be a volcano, and probably still is!",
      0,
  };
  npc_talk(dialogue);
}

static void handle_hidden(char *map, int top, int left) {
  char *dialogue[] = {
      "Hi, I'm a hidden NPC. I don't actually do anything, and I'm not in the",
      "game. I'm just something for Reverse Engineers to find!",
      0,
  };
  npc_talk(dialogue);
}

static object_result_t handle_final(char o, int top, int left) {
  if(o == ' ') {
    return NO_BONK;
  }

  if(o == '#') {
    handle_wall("Final Fight", top, left);
    return BONK;
  }

  if(o == 'D') {
    handle_door("Final Fight", top, left);
    return NO_BONK;
  }

  if(top == 2) {
    char *dialogue[] = {
        "Red huffs,",
        "",
        "\"Can't you see I'm busy fighting Blue?\"",
        0,
    };
    npc_talk(dialogue);
  npc_talk(dialogue);
  } else if(top == 4) {
    char *dialogue[] = {
        "Watching Red and Blue fight is all fun and games, but you have a",
        "feeling you've come to the wrong place.",
        "",
        "But since you made it here, have you found all of the hidden quality-",
        "-of-life commands? They aren't required, but they'll help!",
        0,
    };
    npc_talk(dialogue);
  } else if(top == 6) {
    char *dialogue[] = {
        "Blue puffs,",
        "",
        "\"Can't you see I'm busy fighting Red?\"",
        0,
    };
    npc_talk(dialogue);
  }
}

object_result_t check_bonk(char o, int top, int left, character_t *character, map_t *map, world_t *world) {
  if(!strcasecmp(map->name, "Final Fight")) {
    return handle_final(o, top, left);
  }

  switch(o) {
    // Static / generic objects
    case WALL:
      handle_wall(map->name, top, left);
      return BONK;

    case DOOR:
      handle_door(map->name, top, left);
      return NO_BONK;

    case PC:
      handle_pc(world);
      return BONK;

    case TREE:
      handle_tree(map->name, top, left);
      return BONK;

    case MOUNTAIN:
      handle_mountain(map->name, top, left);
      return BONK;

    case CAVE_WALL:
      handle_cave_wall(map->name, top, left);
      return BONK;

    case BARRICADE:
      handle_barricade(map->name, top, left);
      return BONK;

    case WATER:
      return handle_water(map->name, top, left);

    // Lab
    case PROFESSOR:
      handle_professor(character, world);
      return BONK;

    // Colour
    case MOM:
      handle_mom(map->name, top, left);
      return BONK;

    case RIVAL:
      handle_rival(map->name, top, left);
      return BONK;

    // Victory Lane
    case CHAMPION:
      handle_champion(map->name, top, left);
      return BONK;

    // Teal Plateau
    case ELITE1:
    case ELITE2:
    case ELITE4:
      handle_elite4(map->name, top, left);
      return BONK;

    // Gray
    case GYM_LEADER:
      handle_gym_leader(map->name, top, left);
      return BONK;

    case MUSEUM:
      handle_museum(world, map->name, top, left);
      return BONK;

    // Mount Earth
    case TRAINER:
      handle_trainer(world, map->name, top, left);
      return BONK;

    // DeepBlue
    case NAME_RATER:
      handle_name_rater(character);
      return BONK;

    case BIKE_SHOP:
      handle_bike_shop(map->name, top, left);
      return BONK;

    case POOL:
      handle_pool(map->name, top, left);
      return NO_BONK;

    // Ron's house
    case RON:
      handle_ron(map->name, top, left);
      return BONK;

    // Orange
    case SECRETARY:
      handle_secretary(map->name, top, left);
      return BONK;

    case ELEVATOR:
      handle_elevator(map->name, top, left);
      return BONK;

    // Jade
    case MALL_WORKER:
      handle_mall_worker(map->name, top, left);
      return BONK;

    // Scarlet
    case WEIRDNESS:
      handle_weirdness(map->name, top, left);
      return BONK;

    case VOLCANO:
      handle_volcano(map->name, top, left);
      return BONK;

    case HIDDEN:
      handle_hidden(map->name, top, left);
      return BONK;

    default:
      return NO_BONK;
  }
}
