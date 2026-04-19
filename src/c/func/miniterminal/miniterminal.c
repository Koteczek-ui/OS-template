#include "miniterminal.h"
#include "../../utils/console/console.h"
#include "../../utils/keyboard/keyboard.h"
#include "../../utils/io/io.h"
#include <stdint.h>

#define MAX_HISTORY 20
char history[MAX_HISTORY][256];
int history_count = 0;
int history_index = -1;

void strcpy(char* dest, const char* src) {
    while ((*dest++ = *src++));
}

void help_cmd_helper(char* name, char* desc) {
    kprint("\n  - ", 0x0F);
    kprint(name, 0x09);
    kprint(" - ", 0x01);
    kprint(desc, 0x09);
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, int n) {
    if (n == 0) return 0;
    while (n > 1 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

void handle_print_logic(char* ptr) {
    char output[2] = {0, 0};
    while (*ptr && *ptr != '\"') {
        if (*ptr == '\\') {
            ptr++;
            if (*ptr == '\\') output[0] = '\\';
            else if (*ptr == '\"') output[0] = '\"';
            else if (*ptr == 'n') output[0] = '\n';
            else {
                output[0] = '\\'; 
                kprint(output, 0x0F);
                output[0] = *ptr;
            }
        } else {
            output[0] = *ptr;
        }
        kprint(output, 0x0F);
        if (*ptr) ptr++;
    }
    kprint("\n", 0x0F);
}

void parse_cmd(char* cmd) {
    if (strcmp(cmd, "hlt") == 0 || strcmp(cmd, "halt") == 0) asm volatile ("hlt");
    else if (strcmp(cmd, "cls") == 0 || strcmp(cmd, "clear") == 0) kclear(0x00);
    else if (strncmp(cmd, "kprint \"", 8) == 0) handle_print_logic(cmd + 8);
    else if (strncmp(cmd, "print \"", 7) == 0) handle_print_logic(cmd + 7);
    else if (strncmp(cmd, "mkstr \"", 7) == 0) handle_print_logic(cmd + 7);
    else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
        kprint("==================== Help ====================", 0x09);
        help_cmd_helper("hlt/halt", "Stop kernel");
        help_cmd_helper("cls/clear", "Clear screen");
        help_cmd_helper("print/kprint/mkstr \"text\"", "Print text");
        help_cmd_helper("?/help", "This help");
        kprint("\n==============================================\n", 0x09);
    }
    else {
        kprint("Unknown command: ", 0x04);
        kprint(cmd, 0x04);
        kprint("\n", 0x04);
    }
}

void start_terminal() {
    char cmd_buffer[256];
    int cmd_index = 0;
    uint16_t prompt_pos;

    kprint("For help, type 'help' or '?'.\n", 0x08);
    kprint("> ", 0x05);
    prompt_pos = get_cur_pos();

    while (1) {
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);

            if (scancode == 0x49) { scroll_view_up(); continue; }
            if (scancode == 0x51) { scroll_view_down(); continue; }

            if (scancode == 0x48) {
                if (history_count > 0 && history_index < history_count - 1) {
                    history_index++;
                    set_cur_pos(prompt_pos);
                    for(int i = 0; i < cmd_index; i++) kprint(" ", 0x0F);
                    set_cur_pos(prompt_pos);
                    
                    strcpy(cmd_buffer, history[history_index]);
                    cmd_index = 0;
                    while(cmd_buffer[cmd_index]) cmd_index++;
                    kprint(cmd_buffer, 0x0F);
                }
                continue;
            }

            if (scancode == 0x50) {
                if (history_index > 0) {
                    history_index--;
                    set_cur_pos(prompt_pos);
                    for(int i = 0; i < cmd_index; i++) kprint(" ", 0x0F);
                    set_cur_pos(prompt_pos);

                    strcpy(cmd_buffer, history[history_index]);
                    cmd_index = 0;
                    while(cmd_buffer[cmd_index]) cmd_index++;
                    kprint(cmd_buffer, 0x0F);
                } else if (history_index == 0) {
                    history_index = -1;
                    set_cur_pos(prompt_pos);
                    for(int i = 0; i < cmd_index; i++) kprint(" ", 0x0F);
                    set_cur_pos(prompt_pos);
                    cmd_index = 0;
                    cmd_buffer[0] = '\0';
                }
                continue;
            }

            char c = keyboard_process_byte(scancode);

            if (c != 0) {
                if (c == '\n') {
                    cmd_buffer[cmd_index] = '\0';
                    kprint("\n", 0x0F);
                    
                    if (cmd_index > 0) {
                        for (int i = MAX_HISTORY - 1; i > 0; i--) strcpy(history[i], history[i-1]);
                        strcpy(history[0], cmd_buffer);
                        if (history_count < MAX_HISTORY) history_count++;
                        
                        parse_cmd(cmd_buffer);
                    }

                    cmd_index = 0;
                    history_index = -1;
                    kprint("> ", 0x05);
                    prompt_pos = get_cur_pos();
                } else if (c == '\b') {
                    uint16_t current_pos = get_cur_pos();
                    if (current_pos > prompt_pos) {
                        current_pos--;
                        set_cur_pos(current_pos);
                        kprint(" ", 0x0F);
                        set_cur_pos(current_pos);
                        if (cmd_index > 0) cmd_index--;
                    }
                } else {
                    if (cmd_index < 254) {
                        char str[2] = {c, 0};
                        kprint(str, 0x0F);
                        cmd_buffer[cmd_index++] = c;
                    }
                }
            }
        }
    }
}

