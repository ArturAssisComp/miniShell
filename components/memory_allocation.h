#ifndef MEMORY_ALLOCATION_H
#define MEMORY_ALLOCATION_H

#include <stdio.h>
#include <stdlib.h>

void *xcalloc(size_t num_of_elements, size_t size_of_element);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
#endif
