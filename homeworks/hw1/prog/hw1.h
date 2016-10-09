#include <stdio.h>

#define printf(...) myprintf(__VA_ARGS__)
int myprintf(const char*, ...);
