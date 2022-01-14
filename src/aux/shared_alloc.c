#include "shared_alloc.h"
#include <sys/mman.h>
#include <stddef.h>

void *SA_malloc(size_t size)
{
    void *addr;
    addr = mmap(NULL, sizeof(size_t) + size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(addr == MAP_FAILED) addr = NULL;
    else ((size_t *) addr)[0] = size;
    return &((size_t *)addr)[1];
}


void SA_free(void *ptr)
{
    size_t size = ((size_t *)ptr)[-1];
    munmap(ptr, size);
}

void *SA_calloc(size_t nmemb, size_t size)
{
    void *addr;
    addr = mmap(NULL, size * nmemb, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(addr == MAP_FAILED) addr = NULL;
    else ((size_t *) addr)[0] = size * nmemb;
    return &((size_t *)addr)[1];
}
