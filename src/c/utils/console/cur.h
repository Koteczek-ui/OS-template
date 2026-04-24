#ifndef UTIL_CUR_H
#define UTIL_CUR_H

#include <stdint.h>

#define MAX_CUR_X 80
#define MAX_CUR_Y 25

extern int cur_x;
extern int cur_y;

uint16_t get_cur_pos();
void set_cur_pos(uint16_t pos);

#endif

