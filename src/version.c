#/*
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

#include "version.h"

const struct semantic_version server_version = {
    .major = 0, .minor = 1, .patch = 0};

const struct target_version target_version = {
    .type = version_type_release,
    .version = {.release = {.major = 1, .minor = 19, .patch = 2}}};

const char *target_version_string = "1.19.2";
