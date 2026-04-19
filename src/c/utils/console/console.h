#ifndef UTIL_CONSOLE_H
#define UTIL_CONSOLE_H

#include <stdint.h>

void print(char* text, char color);
void clear(char color);

uint16_t get_cur_pos();
void set_cur_pos(uint16_t pos);

#endif

