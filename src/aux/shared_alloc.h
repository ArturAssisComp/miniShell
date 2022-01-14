#ifndef SHARED_ALLOC_H
#define SHARED_ALLOC_H

void *SA_malloc(size_t size);
void SA_free(void *ptr);
void *SA_calloc(size_t nmemb, size_t size);

#endif
