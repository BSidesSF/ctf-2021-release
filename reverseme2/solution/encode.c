#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int i;
  srand(0x13371337);
  unsigned char buffer[4096];

  ssize_t len = read(0, buffer, 4096);

  if(len < 0) {
    printf("Error reading!\n");
    exit(1);
  }

  for(i = 0; i < len; i++) {
    buffer[i] ^= (rand() >> 3) & 0x0FF;
    printf("%c", buffer[i]);
  }

  return 0;
}
