#include "utils/console.h"

int cursor_x = 0;
int cursor_y = 0;

void print(char* text, char color) {
    char* video_memory = (char*) 0xb8000;
    int i = 0;
    
    while (text[i] != 0) {
        if (text[i] == '\n') {
            cursor_x = 0;
            cursor_y++;
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
}

void clear(char color) {
    char* video_memory = (char*) 0xb8000;
    int i = 0;
    while (i < 80 * 25 * 2) {
        video_memory[i] = ' ';
        i++;
        video_memory[i] = color;
        i++;
    }
    cursor_x = 0;
    cursor_y = 0;
}

