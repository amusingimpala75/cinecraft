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

int main(int argc, char *argv[]) {
  printf("Cinecraft Server version %d.%d.%d targeting version ",
         server_version.major, server_version.minor, server_version.patch);
  if (target_version.type == version_type_release) {
    printf("%d.%d.%d", target_version.version.release.major,
           target_version.version.release.minor,
           target_version.version.release.patch);
  } else {
    printf("%dw%d%c", target_version.version.snapshot.year,
           target_version.version.snapshot.count,
           'a' + target_version.version.snapshot.iteration);
  }
  printf("\n");
  const int max_file_path = 1024;
  char *run_dir = safe_malloc(max_file_path); // todo: un-hardcode
  getcwd(run_dir, sizeof(char *) * max_file_path);
  if (argc > 1) {
    const char *dir_change = argv[1]; // todo: non relative path
    if (strlen(run_dir) + strlen(dir_change) + 2 >
        sizeof(char *) * max_file_path) {
      ERROR("file length too long");
      exit(1);
    }
    strcat(run_dir, "/");
    strcat(run_dir, dir_change);
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
