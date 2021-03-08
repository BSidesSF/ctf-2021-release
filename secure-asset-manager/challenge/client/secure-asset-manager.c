#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifndef HOST
#define HOST "secure-asset-manager-4235c8a4.challenges.bsidessf.net"
#endif

#ifndef PORT
#define PORT 6112
#endif

#include "../common/validator.h"
#include "../common/proc_maps_parser/pmparser.h"

#define NAME "secure-asset-manager"
#define VERSION "secure-asset-manager client v1.00"
#define CHECK_UPDATES "CHECK_UPDATES"
#define I_AM "I_AM "
#define BYE "BYE"

data_block_t *get_memory() {
  procmaps_iterator* maps = pmparser_parse();
  if(maps==NULL){
    fprintf(stderr, "Couldn't parse /proc/self/maps!\n");
    exit(1);
  }

  // Loop through /proc/self/maps
  procmaps_struct* maps_tmp = NULL;
  while((maps_tmp = pmparser_next(maps)) != NULL){
    // Look for the first entry with the proper permissions
    if(!strcmp(maps_tmp->perm, "r-xp")) {
      if(!strcmp(maps_tmp->pathname + strlen(maps_tmp->pathname) - strlen(NAME), NAME)) {
        data_block_t *result = (data_block_t *) malloc(sizeof(data_block_t));
        result->data = maps_tmp->addr_start;
        result->length = maps_tmp->length;
        pmparser_free(maps);

        return result;
      }
    }
  }
  pmparser_free(maps);
  fprintf(stderr, "Couldn't find our memory mapping\n");
  exit(1);
}

void send_block(int s, data_block_t *block) {
  uint8_t size_buf[2];

  size_buf[0] = (block->length & 0x000000FF) >> 0;
  size_buf[1] = (block->length & 0x0000FF00) >> 8;

  write(s, size_buf, 2);
  write(s, block->data, block->length);
}

data_block_t *recv_block(int s) {
  uint8_t size_buf[2];

  // Read the length
  read(s, size_buf, 2);

  // Allocate memory
  data_block_t *result = (data_block_t *) malloc(sizeof(data_block_t));
  result->length = size_buf[0] | size_buf[1] << 8;
  result->data = malloc(result->length);

  // Read the body
  if(read(s, result->data, result->length) != result->length) {
    fprintf(stderr, "ERROR: Failed to receive data!\n");
    exit(1);
  }

  return result;
}

void free_block_and_data(data_block_t *b) {
  free(b->data);
  free(b);
}

void go(int s) {
  printf("Connected to %s:%d\n", HOST, PORT);

  data_block_t *in = NULL;

  // Get the server version
  in = recv_block(s);
  printf("Server version: %s\n", in->data);
  free_block_and_data(in);

  // Send our version
  data_block_t out;
  out.data = (uint8_t*)VERSION;
  out.length = strlen(VERSION) + 1;
  send_block(s, &out);

  // Receive the challenge
  in = recv_block(s);
  printf("Received server challenge (%ld bytes), calculating response\n", in->length);

  // Get a handle to memory
  data_block_t *memory = get_memory();

  // Calculate the checksum
  uint32_t result = checksum(in, memory);

  // Store the response as a little endian integer
  out.length = 4;
  out.data = (uint8_t *) malloc(4);
  out.data[0] = ((result >>  0) & 0x0FF);
  out.data[1] = ((result >>  8) & 0x0FF);
  out.data[2] = ((result >> 16) & 0x0FF);
  out.data[3] = ((result >> 24) & 0x0FF);

  // Done with server data
  free_block_and_data(in);

  // Send the response and free the memory
  printf("Sending challenge response\n");
  send_block(s, &out);
  free(out.data);

  // Get the server reply
  in = recv_block(s);
  if(memcmp(in->data, "PASS", 4)) {
    printf("Server challenge failed!\n");
    exit(0);
  }
  printf("Server challenge passed!\n");
  free_block_and_data(in);

  // Check for updates?
  printf("Checking for updates...\n");
  out.data = (uint8_t*)CHECK_UPDATES;
  out.length = strlen(CHECK_UPDATES) + 1;
  send_block(s, &out);

  // Get the response
  in = recv_block(s);
  if(in->data[0] != '0') {
    printf("The server says there's an update available! Please update your client before continuing\n");
    exit(0);
  }
  printf("No updates available!\n");
  free_block_and_data(in);

  // Check in?
  out.data = malloc(1024 + strlen(I_AM) + 1);
  memcpy(out.data, I_AM, 5);
  gethostname((char*)(out.data + 5), 1024);

  printf("Checking in with hostname...\n");
  out.length = strlen((char*)out.data) + 1;
  send_block(s, &out);

  // Get the response
  in = recv_block(s);
  if(memcmp(in->data, "OK", 2)) {
    printf("Something went wrong with checking in!\n");
    exit(0);
  }
  printf("Check-in successful! Thanks for using "VERSION"!!\n");
  free_block_and_data(in);

  // Disconnect
  printf("Disconnecting...\n");
  out.data = (uint8_t*)BYE;
  out.length = strlen(BYE) + 1;
  send_block(s, &out);
}

int main(int argc, char *argv[])
{
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    fprintf(stderr, "Failed to create socket\n");
    exit(1);
  }

  struct hostent *server = gethostbyname(HOST);
  if(!server) {
    fprintf(stderr, "Failed to resolve host %s\n", HOST);
    exit(1);
  }

  struct sockaddr_in servaddr;
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  bcopy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);

  if(connect(s, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
      fprintf(stderr, "Failed to connect to server %s:%d\n", HOST, PORT);
      exit(1);
  }

  go(s);
  close(s);

  return 0;
}
