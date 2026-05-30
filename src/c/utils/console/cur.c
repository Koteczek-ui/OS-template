#include "../io/io.h"
#include "cur.h"
#include "console.h"

int cur_x = 0;
int cur_y = 0;

uint16_t get_cur_pos() {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

void set_cur_pos(uint16_t pos) {
    cur_x = pos % MAX_CUR_X;
    cur_y = pos / MAX_CUR_X;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

