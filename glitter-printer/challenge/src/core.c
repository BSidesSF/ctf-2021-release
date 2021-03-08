// *******************************
// * This MUST be first!!     // *
#include "start.h"            // *
// * END This MUST be first   // *
// *******************************

#include "syscalls.h"
#include "asmfuncs.h"

#define PATH_START "/spool/"
#define STDIN 0
#define STDOUT 1


char read_byte() {
  char buffer = 0;
  int result = read(STDIN, &buffer, 1);

  if(result != 1) {
    exit(1);
  }

  return buffer;
}

int read_number(char *terminator) {
  int result = 0;

  while(1) {
    // Read a single byte
    char buffer = read_byte();

    // If it's not a valid byte, we're done (and we consume the terminator)
    if(buffer < '0' || buffer > '9') {
      if(terminator) {
        *terminator = buffer;
      }
      return result;
    }

    // Add to the result and keep going (vulnerable to overflow!)
    result = (result * 10) + (buffer - '0');
  }

  return result;
}

void reverse(char *str) {
    int start = 0;
    int end = strlen(str) - 1;
    while(start < end) {
      int tmp = str[end];
      str[end] = str[start];
      str[start] = tmp;

      start++;
      end--;
    }
}

void number_to_string(unsigned int number, char *buffer) {
    int i = 0;

    // Don't do an empty string for 0
    if(number == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    while(number != 0) {
        buffer[i++] = (number % 10) + '0';
        number = number/10;
    }

    buffer[i] = '\0';

    reverse(buffer);
}

void read_filename(char *str, char terminator, int max_length) {
  int offset = 0;

  while(offset < max_length) {
    char b = read_byte();
    if(b == terminator) {
      str[offset] = '\0';
      return;
    }

    // Ignore slashes
    if(b == '/' || b == '\\') {
      continue;
    }

    str[offset] = b;
    offset++;
  }
}

void success() {
  char buf = 0;

  write(STDOUT, &buf, 1);
}

// Doesn't really do much - just writes a '1' to a file representing the queue
void print_waiting_jobs() {
  // 01 queue LF - Print any waiting jobs
  int queue = read_number(0);

  // Start with the queue path
  char path[32] = PATH_START"queues/";

  // Append
  number_to_string(queue, path + strlen(path));

  // Open
  int fd = open(path, 1 /* O_RWONLY */ | 64 /* O_CREAT */, 0644);
  char buf = 1;
  write(fd, &buf, 1);
  close(fd);

  // Confirm
  success();
}

void receive_job(queue_t *queues) {
  // 02 queue LF - Receive a printer job
  char terminator = 0;
  int queue = read_number(&terminator);
  if(queue > 1024) { // XXX: Integer underflow bug (this can be negative)
    exit(7);
  }
  if(terminator != ' ') {
    exit(9);
  }

  int subcommand = read_byte(STDIN);

  if(subcommand == 1) {
    // -> 01 LF - Abort job
    queues[queue].active_jobs--;
  } else if(subcommand == 2) {
    // -> 02 Count SP Name LF <bytes> - Receive control file
    // Count
    char terminator = 0;
    int size = read_number(&terminator);
    if(terminator != ' ') {
      exit(2);
    }
    if(size > 1024) {
      exit(3);
    }

    // Name
    char filename[128] = PATH_START"jobs/control/";
    read_filename(filename + strlen(filename), '\n', 127 - strlen(filename));

    // Data
    char data[1024];
    int size_read = 0;

    while(size_read < size) {
      int block_size = read(STDIN, data + size_read, size - size_read);
      size_read += block_size;
    }

    // Do the actual writing
    int fd = open(filename, 1 /* O_RWONLY */ | 64 /* O_CREAT */, 0644);
    write(fd, data, size_read);
    close(fd);
  } else if(subcommand == 3) {
    // -> 03 Count SP Name LF <bytes> - Receive data file

    // Count
    char terminator = 0;
    int size = read_number(&terminator);
    if(terminator != ' ') {
      exit(2);
    }
    if(size > 1024) {
      exit(3);
    }

    // Name
    char filename[128] = PATH_START"jobs/data/";
    read_filename(filename + strlen(filename), '\n', 127 - strlen(filename));

    // Data
    char data[1024];
    int size_read = 0;

    while(size_read < size) {
      int block_size = read(STDIN, data + size_read, size - size_read);
      size_read += block_size;
    }

    // Do the actual writing
    int fd = open(filename, 1 /* O_RWONLY */ | 64 /* O_CREAT */, 0644);
    write(fd, data, size_read);
    close(fd);

    // Update the queue (note that this 'size' isn't necessarily correct)
    queues[queue].active_jobs++;
    queues[queue].total_bytes_queued += size;
  } else {
    exit(5);
  }

  // Confirm
  success();
}

void queue_state_list(queue_t *queues, int long_output) {
  int queue_number = read_number(0);
  if(queue_number > 1024) { // XXX: Integer underflow bug (this can be negative)
    exit(10);
  }
  queue_t *queue = &queues[queue_number];

  if(long_output) {
    // 04 Queue SP List LF - Send queue state (long)
    // Dress up the queue a bit
    char active_jobs[64] = "Number of active jobs: ";
    number_to_string(queue->active_jobs, active_jobs + strlen(active_jobs));
    write(STDOUT, active_jobs, strlen(active_jobs) + 1);

    char total_bytes[64] = "Bytes queued: ";
    number_to_string(queue->total_bytes_queued, total_bytes + strlen(total_bytes));
    write(STDOUT, total_bytes, strlen(total_bytes) + 1);
  } else {
    // 03 Queue SP List LF - Send queue state (short)
    // Just send the literal queue
    write(STDOUT, (void*)queue, sizeof(queue_t));
  }
}

void _start(queue_t *queues) {
  while(1) {
    char command = read_byte(STDIN);

    if(command == 1) {
      print_waiting_jobs();
    } else if(command == 2) {
      receive_job(queues);
    } else if(command == 3) {
      queue_state_list(queues, 0);
    } else if(command == 4) {
      queue_state_list(queues, 1);
    } else if(command == 5) {
      // 05 Queue SP Agent SP List LF - Remove jobs
    } else {
      exit(6);
    }
  }
}
