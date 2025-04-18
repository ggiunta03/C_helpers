#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

void zero_free(void*, size_t);
void free_all(void **);
void *safe_realloc(void *, size_t *, size_t);
char *trim_ending_matches(char *haystack, char *needle);
char *build_str(char *str1, char *str2);

#endif