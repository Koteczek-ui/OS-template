#ifndef UTIL_CONSOLE_H
#define UTIL_CONSOLE_H

#include <stdint.h>

#define GET_KPRINT_MACRO(_1, _2, NAME, ...) NAME
#define kprint(...) GET_KPRINT_MACRO(__VA_ARGS__, kprint_color, kprint_default)(__VA_ARGS__)

#define kprint_default(text) kprint_raw(text, 0x07)
#define kprint_color(text, color) kprint_raw(text, color)

void kprint_raw(const char* text, char color);

void kcls(char color);
void scroll();
void scroll_view_up();
void scroll_view_down();

#endif
