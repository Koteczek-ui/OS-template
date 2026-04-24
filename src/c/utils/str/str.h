#ifndef UTIL_STR_H
#define UTIL_STR_H

#include <stdint.h>

void* memcpy(void* dest, const void* src, uint32_t n);
void* memset(void* s, int c, uint32_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, uint32_t n);
uint32_t strlen(const char* s);
char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);
char* strrchr(const char* s, int c);

#endif

