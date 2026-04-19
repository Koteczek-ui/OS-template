#include "console.h"
#include "../io/io.h"

int cursor_x = 0;
int cursor_y = 0;

uint16_t get_cur_pos() {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

void set_cur_pos(uint16_t pos) {
    cursor_x = pos % 80;
    cursor_y = pos / 80;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void print(char* text, char color) {
    char* video_memory = (char*) 0xb8000;
    int i = 0;
    
    while (text[i] != 0) {
        if (text[i] == '\n') {
            cursor_x = 0;
            cursor_y++;
        } else if (text[i] == '\b') {
            if (cursor_x > 0) {
                cursor_x--;
            } else if (cursor_y > 0) {
                cursor_y--;
                cursor_x = 79;
            }
            
            int offset = (cursor_y * 80 + cursor_x) * 2;
            video_memory[offset] = ' ';
            video_memory[offset + 1] = color;
        } else {
            int offset = (cursor_y * 80 + cursor_x) * 2;
            video_memory[offset] = text[i];
            video_memory[offset + 1] = color;
            cursor_x++;
        }

        if (cursor_x >= 80) {
            cursor_x = 0;
            cursor_y++;
        }

        i++;
    }

    set_cur_pos(cursor_y * 80 + cursor_x);
}

void clear(char color) {
    char* video_memory = (char*) 0xb8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = color;
    }
    
    set_cur_pos(0);
}

