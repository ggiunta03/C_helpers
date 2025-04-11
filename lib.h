#ifdef LIB_H
#define LIB_H

void zero_free(void*, size_t);
void free_all(char **);
void *safe_realloc(void *, size_t *, size_t)

#endif