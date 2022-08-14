#pragma once

#include "types.h"

extern struct config {
  struct server {
    i16 port;
  } server;
  struct preview {
    char *motd;
    char *icon;
  } preview;
  i64 max_player_count;
  bool chat_preview;
} config;

void load_config(void);
