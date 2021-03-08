#ifndef __HELPERS_H__
#define __HELPESR_H__

typedef enum {
  NO,
  YES,
} yes_no_t;

void wait();
void clear();
void wait_then_clear();
yes_no_t yes_no(char *prompt, yes_no_t def);

char *read_file(char *filename);

#endif
