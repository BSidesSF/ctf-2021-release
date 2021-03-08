#ifndef __ASCIIMON_H__
#define __ASCIIMON_H__

#ifndef DATA_PATH
#define DATA_PATH "/home/ctf/data/"
#endif

#define ASCIIMON_LINE_LENGTH 256
#define MAX_ASCIIMON_NAME 32

typedef struct {
  int   number;
  char  name[MAX_ASCIIMON_NAME];
  int   hp;
  int   attack;
  int   defense;
  int   speed;
  int   caught;
  char *portrait;
} asciimon_t;

#endif

asciimon_t *asciimon_load(char *name);
void asciimon_print(asciimon_t *asciimon, int display_portrait);
void asciimon_free(asciimon_t *asciimon);
