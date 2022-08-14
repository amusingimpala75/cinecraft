#include "memory.h"
#include "log.h"
#include <stdlib.h>

void *safe_malloc(size_t size) {
  void *val = malloc(size);
  if (!val) {
    ERROR("malloc error");
  }
  return val;
}

void *safe_realloc(void *ptr, size_t size) {
  void *val = realloc(ptr, size);
  if (!val) {
    ERROR("realloc error");
  }
  return val;
}
