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
