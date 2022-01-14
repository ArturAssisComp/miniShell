#include "shared_alloc.h"
#include <sys/mmap.h>

void *SA_malloc(size_t size)
{
    void *addr;
    addr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(addr == MAP_FAILED) addr = NULL;
    return addr;
}


void SA_free(void *ptr, size_t size)
{
    munmap(ptr, size);
}

void *SA_calloc(size_t nmemb, size_t size)
{
    void *addr;
    addr = mmap(NULL, size * nmemb, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(addr == MAP_FAILED) addr = NULL;
    return addr;
}
