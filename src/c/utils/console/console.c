#include "console.h"
#include "../io/io.h"

int cur_x = 0;
int cur_y = 0;

uint16_t video_offset = 0;

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

void scroll() {
    char* video_mem = (char*) 0xb8000;
    for (int i = 0; i < 24 * 80 * 2; i++) {
        video_mem[i] = video_mem[i + 80 * 2];
    }
    for (int i = 24 * 80 * 2; i < 25 * 80 * 2; i += 2) {
        video_mem[i] = ' ';
        video_mem[i + 1] = 0x07;
    }
    cur_y = 24;
}

void update_video_offset() {
    outb(0x3D4, 0x0C);
    outb(0x3D5, (uint8_t)((video_offset >> 8) & 0xFF));
    outb(0x3D4, 0x0D);
    outb(0x3D5, (uint8_t)(video_offset & 0xFF));
}

void scroll_view_up() {
    if (video_offset >= 80) {
        video_offset -= 80;
        update_video_offset();
    }
}

void scroll_view_down() {
    video_offset += 80;
    update_video_offset();
}

void kprint(const char* text, char color) {
    char* video_mem = (char*) 0xb8000;
    int i = 0;
    
    while (text[i] != 0) {
        if (text[i] == '\n') {
            cur_x = 0;
            cur_y++;
        } else if (text[i] == '\b') {
            if (cur_x > 0) cur_x--;
            int offset = (cur_y * 80 + cur_x) * 2;
            video_mem[offset] = ' ';
            video_mem[offset + 1] = color;
        } else {
            int offset = (cur_y * 80 + cur_x) * 2;
            video_mem[offset] = text[i];
            video_mem[offset + 1] = color;
            cur_x++;
        }

        if (cur_x >= 80) {
            cur_x = 0;
            cur_y++;
        }

        if (cur_y >= 25) scroll();

        i++;
    }
    set_cur_pos(cur_y * 80 + cur_x);
}

void kclear(char color) {
    char* video_mem = (char*) 0xb8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_mem[i] = ' ';
        video_mem[i + 1] = color;
    }
    
    set_cur_pos(0);
}

