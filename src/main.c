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
#include "log.h"
#include "memory.h"
#include "network.h"
#include "system.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define getcwd _getcwd
#include <direct.h>
#else
#include <unistd.h>
#endif

const char license_notice[] =
  "    Cinecraft  Copyright (C) 2022  AmusingImpala75\n"
  "    This program comes with ABSOLUTELY NO WARRANTY; for details type see the GPLv3.\n"
  "    This is free software, and you are welcome to redistribute it\n"
  "    under certain conditions; see the GPLv3 for details.\n"
;

int main(int argc, char *argv[]) {
  printf("%s", license_notice);
  printf("Cinecraft Server version %d.%d.%d targeting version %s\n",
         server_version.major, server_version.minor, server_version.patch, target_version_string);
  const int max_file_path = 1024;
  char *run_dir = safe_malloc(max_file_path); // todo: un-hardcode
  getcwd(run_dir, sizeof(char *) * max_file_path);
  if (argc == 2) {
    const char *dir_change = argv[1]; // todo: non relative path
    if (strlen(run_dir) + strlen(dir_change) + 2 >
        sizeof(char *) * max_file_path) {
      ERROR("file length too long");
      exit(1);
    }
    strcat(run_dir, "/");
    strcat(run_dir, dir_change);
  } else {
    ERROR("Usage: cinecraft path-to-directory-for-data");
    return 0;
  }
  set_run_dir(run_dir);
  open_log();
  load_config();
  initialize_socket();
  while (1) {
    sleep(1);
  }
  return 0;
}
