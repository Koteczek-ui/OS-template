#include "console.h"
#include "cur.h"
#include "../io/io.h"

uint16_t video_offset = 0;

void scroll() {
    char* video_mem = (char*) 0xb8000;
    for (int i = 0; i < (MAX_CUR_Y - 1) * MAX_CUR_X * 2; i++) video_mem[i] = video_mem[i + MAX_CUR_X * 2];
    for (int i = (MAX_CUR_Y - 1) * MAX_CUR_X * 2; i < MAX_CUR_Y * MAX_CUR_X * 2; i += 2) {
        video_mem[i] = ' ';
        video_mem[i + 1] = 0x07;
    }
    cur_y = (MAX_CUR_Y - 1);
}

void update_video_offset() {
    outb(0x3D4, 0x0C);
    outb(0x3D5, (uint8_t)((video_offset >> 8) & 0xFF));
    outb(0x3D4, 0x0D);
    outb(0x3D5, (uint8_t)(video_offset & 0xFF));
}

void scroll_view_up() {
    if (video_offset >= MAX_CUR_X) {
        video_offset -= MAX_CUR_X;
        update_video_offset();
    }
}

void scroll_view_down() {
    video_offset += MAX_CUR_X;
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
            int offset = (cur_y * MAX_CUR_X + cur_x) * 2;
            video_mem[offset] = ' ';
            video_mem[offset + 1] = color;
        } else {
            int offset = (cur_y * MAX_CUR_X + cur_x) * 2;
            video_mem[offset] = text[i];
            video_mem[offset + 1] = color;
            cur_x++;
        }

        if (cur_x >= MAX_CUR_X) {
            cur_x = 0;
            cur_y++;
        }

        if (cur_y >= MAX_CUR_Y) scroll();

        i++;
    }
    set_cur_pos(cur_y * MAX_CUR_X + cur_x);
}

void kcls(char color) {
    char* video_mem = (char*) 0xb8000;
    for (int i = 0; i < MAX_CUR_X * MAX_CUR_Y * 2; i += 2) {
        video_mem[i] = ' ';
        video_mem[i + 1] = color;
    }
    
    cur_x = 0;
    cur_y = 0;
    video_offset = 0;
    update_video_offset();
    set_cur_pos(0);
}

