#pragma once

#include "types.h"

struct semantic_version {
  i8 major;
  i8 minor;
  i8 patch;
};

struct snap_version {
  i8 year;
  i8 count;
  i8 iteration;
};

enum version_type { version_type_release, version_type_snapshot };

struct target_version {
  enum version_type type;
  union {
    struct semantic_version release;
    struct snap_version snapshot;
  } version;
};

const struct semantic_version server_version = {
    .major = 0, .minor = 1, .patch = 0};

const struct target_version target_version = {
    .type = version_type_release,
    .version = {.release = {.major = 1, .minor = 19, .patch = 1}}};
