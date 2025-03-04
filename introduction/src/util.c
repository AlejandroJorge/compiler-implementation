#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

string String(const char *str) {
  string new_str = checked_malloc(strlen(str) + 1);
  strcpy(new_str, str);
  new_str[strlen(str)] = 0;
  return new_str;
}

void *checked_malloc(size_t size) {
  void *p = malloc(size);
  assert(p);

  return p;
}
