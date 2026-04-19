#include "console.h"
#include "../io/io.h"

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
    cur_x = pos % 80;
    cur_y = pos / 80;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void kprint(char* text, char color) {
    char* video_memory = (char*) 0xb8000;
    int i = 0;
    
    while (text[i] != 0) {
        if (text[i] == '\n') {
            cur_x = 0;
            cur_y++;
        } else if (text[i] == '\b') {
            if (cur_x > 0) {
                cur_x--;
            } else if (cur_y > 0) {
                cur_y--;
                cur_x = (80 - 1);
            }
            
            int offset = (cur_y * 80 + cur_x) * 2;
            video_memory[offset] = ' ';
            video_memory[offset + 1] = color;
        } else {
            int offset = (cur_y * 80 + cur_x) * 2;
            video_memory[offset] = text[i];
            video_memory[offset + 1] = color;
            cur_x++;
        }

        if (cur_x >= 80) {
            cur_x = 0;
            cur_y++;
        }

        i++;
    }

    set_cur_pos(cur_y * 80 + cur_x);
}

void kclear(char color) {
    char* video_memory = (char*) 0xb8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = color;
    }
    
    set_cur_pos(0);
}

