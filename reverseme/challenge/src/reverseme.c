#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define LENGTH 4096
#define disable_buffering(_fd) setvbuf(_fd, NULL, _IONBF, 0)

int main(int argc, char *argv[])
{
  unsigned char *buffer = mmap(NULL, LENGTH, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
  ssize_t len;

  alarm(10);

  disable_buffering(stdout);
  disable_buffering(stderr);

  printf("Send me (encoded) x64!!\n");
  len = read(0, buffer, LENGTH);

  if(len < 0) {
    printf("Error reading!\n");
    exit(1);
  }

  int i;
  for(i = 0; i < len; i++) {
    buffer[i] ^= 0x41;
  }

  asm("call *%0\n" : :"r"(buffer));

  return 0;
}
