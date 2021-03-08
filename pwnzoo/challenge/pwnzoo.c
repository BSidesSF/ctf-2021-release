#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

typedef enum {
  DOG,
  CAT,
} species_t;

typedef struct st_animal_t {
  species_t species;
  char name[36];
  void (*speak)(struct st_animal_t *);
} animal_t;


char tmpbuf[128];


void speak_dog(animal_t *dog) {
  printf("Woof!  My name is %s!\n", dog->name);
}

void speak_cat(animal_t *cat) {
  printf("Meow! My name is %s!\n", cat->name);
}

void print_flag() {
  char *flag_path = "/home/ctf/flag.txt";
  if (getenv("FLAG_PATH"))
    flag_path = getenv("FLAG_PATH");
  FILE *fp = fopen(flag_path, "r");
  if (!fp) {
    printf("Unable to open flag!\n");
    exit(1);
  }
  fgets(tmpbuf, sizeof(tmpbuf), fp);
  printf("%s\n", tmpbuf);
  exit(0);
}

static void read_stdin_tmpbuf() {
  if(!fgets(tmpbuf, sizeof(tmpbuf), stdin)) {
    printf("Error reading!\n");
    exit(1);
  }
}

animal_t *construct_animal() {
  animal_t *animal = (animal_t *)malloc(sizeof(animal_t));
  if (!animal) {
    printf("Could not allocate!\n");
    exit(1);
  }
  memset(animal, 0, sizeof(animal_t));
  memset(&animal->name, ' ', sizeof(animal->name));
  animal->name[sizeof(animal->name)-1] = '\0';
  while(1) {
    printf("Play as cat or dog? ");
    read_stdin_tmpbuf();
    switch(tmpbuf[0]) {
      case 'd':
      case 'D':
        animal->species = DOG;
        animal->speak = speak_dog;
        return animal;
      case 'c':
      case 'C':
        animal->species = CAT;
        animal->speak = speak_cat;
        return animal;
    }
  }
}

void change_name(animal_t *animal) {
  printf("New name: ");
  read_stdin_tmpbuf();
  tmpbuf[strcspn(tmpbuf, "\n")] = '\0';
  if (!strlen(tmpbuf))
    return;
  strncpy(animal->name, tmpbuf, strlen(animal->name)+1);
  printf("Name changed!\n");
}

void print_banner() {
  puts("Pwn Zoo!");
  puts("");
}

void print_debug() {
  printf("Sizeof animal: %lu\n", sizeof(animal_t));
  printf("Offset of name: %lu\n", offsetof(animal_t, name));
  printf("Offset of speak: %lu\n", offsetof(animal_t, speak));
}

void menu(animal_t *animal) {
  puts("");
  puts("Menu:");
  puts("1. Speak");
  puts("2. Change name");
  puts("3. Exit");
  read_stdin_tmpbuf();
  switch(tmpbuf[0]) {
    case '1':
    case 'S':
    case 's':
      animal->speak(animal);
      break;
    case '2':
    case 'C':
    case 'c':
    case 'n':
      change_name(animal);
      break;
    case '3':
    case 'x':
    case 'X':
      exit(0);
  }
}

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IONBF, 0);
  print_banner();
  //print_debug();
  animal_t *animal = construct_animal();
  change_name(animal);
  while(1) {
    menu(animal);
  }
  return 0;
}
