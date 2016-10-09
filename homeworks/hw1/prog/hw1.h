#include <stdio.h>

int str_manip(char *, char *);

#define printf(...) myprintf(__VA_ARGS__)
int myprintf(const char *, ...);

#define MYMSG(format, ...) \
  printf("Original message --> %s:%d: " format, __FILE__, __LINE__, __VA_ARGS__)
