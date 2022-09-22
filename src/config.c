/*
    Cinecraft - voxel game server written in C
    Copyright (C) 2022  AmusingImpala75

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "io.h"
#include "log.h"
#include "memory.h"
#include "system.h"

#include <stdlib.h>
#include <string.h>
#include <toml.h>

struct config config;

#define TOML_TABLE(parent, name)                                               \
  toml_table_t *name;                                                          \
  do {                                                                         \
    name = toml_table_in(parent, #name);                                       \
    if (!name) {                                                               \
      ERROR("no table '" #name "' in '" #parent "' in config.toml");           \
    }                                                                          \
  } while (false)
#define TOML_INT(parent, name)                                                 \
  toml_datum_t name;                                                           \
  do {                                                                         \
    name = toml_int_in(parent, #name);                                         \
    if (!name.ok) {                                                            \
      ERROR("no entry '" #name "' in"                                          \
            "'" #parent "' in config.toml");                                   \
    }                                                                          \
  } while (false)
#define TOML_STRING(parent, name)                                              \
  toml_datum_t name;                                                           \
  do {                                                                         \
    name = toml_string_in(parent, #name);                                      \
    if (!name.ok) {                                                            \
      ERROR("no entry '" #name "' in '" #parent "' in config.toml");           \
    }                                                                          \
  } while (false)
#define TOML_BOOL(parent, name)                                                \
  toml_datum_t name;                                                           \
  do {                                                                         \
    name = toml_bool_in(parent, #name);                                        \
    if (!name.ok) {                                                            \
      ERROR("no entry '" #name "' in '" #parent "' in config.toml");           \
    }                                                                          \
  } while (false)

void load_config(void) {
  FILE **file = open_file("/config.toml", "r");
  char errbuf[256];
  toml_table_t *conf = toml_parse_file(*file, errbuf, sizeof(errbuf));
  close_file(file);
  if (!conf) {
    ERROR("Error loading config: %s", errbuf);
  }

  TOML_TABLE(conf, server);
  TOML_INT(server, port);
  config.server.port = port.u.i;

  TOML_TABLE(conf, preview);
  TOML_STRING(preview, motd);
  config.preview.motd = motd.u.s;
  TOML_STRING(preview, icon);
  config.preview.icon = icon.u.s;
  TOML_STRING(preview, name);

  TOML_INT(conf, max_player_count);
  config.max_player_count = max_player_count.u.i;
  TOML_BOOL(conf, chat_preview);
  config.chat_preview = chat_preview.u.b;

  toml_free(conf);

  config.auth = false; //TODO
}

__attribute__((destructor)) static void cleanup_config(void) {
  free(config.preview.motd);
  free(config.preview.icon);
}
