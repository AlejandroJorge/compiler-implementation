#include <assert.h>
#include <stddef.h>

typedef char *string;
string String(const char *);

void *checked_malloc(size_t size);
