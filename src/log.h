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
