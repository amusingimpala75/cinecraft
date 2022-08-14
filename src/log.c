#include "log.h"
#include "io.h"
#include "memory.h"
#include "system.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FILE **log_file = NULL;

static const char *level_as_string(log_level level) {
  switch (level) {
  case log_level_debug:
    return "debug";
  case log_level_info:
    return "info";
  case log_level_warn:
    return "warn";
  case log_level_error:
    return "error";
  default:
    return "";
  }
}

/*
Returns a pointer that must be manually deallocated
*/
static char *get_date_time_str(void) {
  time_t now = time(NULL);
  struct tm now_tm = *localtime(&now);
  /* year_month_day_hour_minute_second */
  const char *fmt = "%d_%d_%d_%d_%d_%d";
  char *ret;
  if (!asprintf(&ret, fmt, now_tm.tm_year + 1900, now_tm.tm_mon + 1,
                now_tm.tm_mday, now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec)) {
    ERROR("malloc error");
  }
  return ret;
}

void open_log(void) {
  char *log_file_path;
  char *time = get_date_time_str();
  log_file_path =
      safe_malloc(strlen("/log/") + strlen(time) + strlen(".log") + 1);
  log_file_path[0] = '\0';
  strcat(log_file_path, "/log/");
  create_directory(log_file_path);
  strcat(log_file_path, time);
  free(time);
  strcat(log_file_path, ".log");
  if (!*(log_file = open_file(log_file_path, "a"))) {
    WARN("could not open log file: %s", log_file_path);
  }
  free(log_file_path);
}

static void set_color(log_level level) {
  switch (level) {
  case log_level_debug:
    printf("\x1B[30;1m");
    return;
  case log_level_info:
    printf("\x1B[37;1m");
    return;
  case log_level_warn:
    printf("\x1B[33;1m");
    return;
  case log_level_error:
    printf("\x1B[31m");
  default:
    return;
  }
}

/*
Returns pointer that must be manually deallocated
*/
static char *get_time_str(void) {
  time_t now = time(NULL);
  struct tm now_tm = *localtime(&now);
  const char *fmt = "[%d:%d:%d] ";
  char *ret;
  if (!asprintf(&ret, fmt, now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec)) {
    ERROR("formatting error");
  }
  return ret;
}

void log_msg(log_level level, const char *file, const int line, const char *msg,
             ...) {
  // todo: time
  const char *level_str = level_as_string(level);
  const char *time = get_time_str();
  va_list args;
  va_start(args, msg);
  if (log_file) {
    fprintf(*log_file, "%s", time);
    fprintf(*log_file, "[%s %s:%d] ", level_str, file, line);
    vfprintf(*log_file, msg, args);
    fprintf(*log_file, "\n");
  }
  set_color(level);
  printf("%s", time);
  printf("[%s: %s:%d] ", level_str, file, line);
  vfprintf(stdout, msg, args);
  va_end(args);
  printf("\n");
  printf("\x1B[0m");
}
