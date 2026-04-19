#ifndef UTIL_CONSOLE_H
#define UTIL_CONSOLE_H

#include <stdint.h>

void kprint(const char* text, char color);
void kclear(char color);

uint16_t get_cur_pos();
void set_cur_pos(uint16_t pos);

void scroll();
void scroll_view_up();
void scroll_view_down();

#endif

