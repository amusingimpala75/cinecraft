#include "io.h"
#include "log.h"
#include "memory.h"
#include "system.h"
#include "types.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_directory(const char *path) {
  const char *cmd;
#ifdef _WIN32
  cmd = "md ";
#else
  cmd = "mkdir -p ";
#endif
  const char *root_path = get_run_dir();
  char *val = safe_malloc(strlen(cmd) + strlen(root_path) + strlen(path) + 1);
  val[0] = '\0';
  strcat(val, cmd);
  strcat(val, root_path);
  strcat(val, path);
  system(val);
  free(val);
}

static struct open_files {
  i64 count;
  i64 max_capacity;
  FILE **files;
  i64 extra_spaces;
} open_files = {
    .count = 0, .max_capacity = 8, .files = NULL, .extra_spaces = 0};

FILE **open_file(const char *path, const char *mode) {
  const char *root = get_run_dir();
  char *total_path = safe_malloc(strlen(root) + strlen(path) + 1);
  total_path[0] = '\0';
  strcat(total_path, root);
  strcat(total_path, path);
  FILE *file = fopen(total_path, mode);
  if (!file) {
    ERROR("could not open file %s with mode %s: ", total_path, mode,
          strerror(errno));
    return NULL;
  }
  free(total_path);
  if (!open_files.files) {
    open_files.files = safe_malloc(sizeof(FILE *) * 8);
  } else if (open_files.extra_spaces > 0) {
    for (size_t i = 0; i < open_files.count; i++) {
      if (!(open_files.files[i])) {
        open_files.extra_spaces--;
        open_files.files[i] = file;
        return &(open_files.files[i]);
      }
    }
  } else if (open_files.count + 1 > open_files.max_capacity) {
    open_files.max_capacity *= 2;
    open_files.files = safe_realloc(open_files.files, open_files.max_capacity);
  }
  open_files.files[open_files.count++] = file;
  return &(open_files.files[open_files.count - 1]);
}

__attribute__((destructor)) static void close_all_files(void) {
  for (size_t i = 0; i < open_files.count; i++) {
    FILE *file = open_files.files[i];
    if (file) {
      fclose(open_files.files[i]);
    }
  }
  free(open_files.files);
}

void close_file(FILE **file) {
  *file = NULL;
  fclose(*file);
  open_files.extra_spaces++;
}
