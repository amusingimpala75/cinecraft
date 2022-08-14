#include "system.h"
#include "io.h"
#include <stdlib.h>

static const char *run_dir = NULL;

__attribute__((destructor)) static void free_run_dir(void) {
  free((void *)run_dir);
}

void set_run_dir(const char *val) { run_dir = val; }

const char *get_run_dir(void) { return run_dir; }
