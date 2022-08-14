#pragma once

#include <stdio.h>

void create_directory(const char *);
FILE **open_file(const char *, const char *);
void close_file(FILE **);
