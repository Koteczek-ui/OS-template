#ifndef UTIL_MEM_H
#define UTIL_MEM_H

#include <stdint.h>

void heap_init();
void* malloc(uint32_t size);
void free(void* ptr);

#endif

