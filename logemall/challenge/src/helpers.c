#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "helpers.h"

void wait() {
  printf("<Press enter to continue>\n");

  for(;;) {
    char c = getchar();
    if(c == EOF) {
      printf("stdin closed\n");
      exit(0);
    }

    if(c == '\n') {
      break;
    }
  }
}

void clear() {
  printf("(If you see this instead of a screen clear, you might need a better terminal)\n");
  printf("\x1b\x5b\x48\x1b\x5b\x32\x4a");
  printf("\n");
  fflush(stdout);
}

void wait_then_clear() {
  wait();
  clear();
}

yes_no_t yes_no(char *prompt, yes_no_t def) {
  if(def == YES) {
    printf("%s [Y/n] > ", prompt);
  } else {
    printf("%s [y/N] > ", prompt);
  }
  fflush(stdout);

  char buf[8];
  fgets(buf, 7, stdin);

  if(tolower(buf[0]) == 'y') {
    return YES;
  } else if(tolower(buf[0]) == 'n') {
    return NO;
  } else {
    return def;
  }
}

char *read_file(char *filename) {
  struct stat statbuf;
  if(stat(filename, &statbuf) == -1) {
    fprintf(stderr, "Failed to open file: %s\n", filename);
    exit(1);
  }

  char *data = malloc(statbuf.st_size + 1);
  memset(data, 0, statbuf.st_size + 1);

  FILE *f = fopen(filename, "rb");
  fread(data, 1, statbuf.st_size, f);
  fclose(f);

  return data;
}
