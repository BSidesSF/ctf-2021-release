#define DATA_START 0x4000-5
#define DATA_START_S "0x4000-5"

#define FILENAME_MAX_LEN 16

typedef struct {
  int active_jobs;
  int total_bytes_queued;
} queue_t;

void _start(queue_t *queues);

__asm__(
    "call bottom;"
    "bottom:"
    "pop eax;"
    "add eax, "DATA_START_S";"
    "push eax;"
    "call _start;"
);

