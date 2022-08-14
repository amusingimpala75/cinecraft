#pragma once

typedef enum log_level {
  log_level_debug,
  log_level_info,
  log_level_warn,
  log_level_error
} log_level;

void log_msg(log_level, const char *, const int, const char *, ...);
void open_log(void);

#define LOG(level, fmt, ...)                                                   \
  log_msg(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define DEBUG(fmt, ...) LOG(log_level_debug, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) LOG(log_level_info, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) LOG(log_level_warn, fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) LOG(log_level_error, fmt, ##__VA_ARGS__)
