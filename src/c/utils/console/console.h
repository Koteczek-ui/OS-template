#ifndef UTIL_CONSOLE_H
#define UTIL_CONSOLE_H

#include <stdint.h>

void kprint(const char* text, char color);
void kclear(char color);

void scroll();
void scroll_view_up();
void scroll_view_down();

#endif

