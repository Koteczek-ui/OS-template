#include "miniterminal.h"
#include "../../utils/console/console.h"
#include "../../utils/keyboard/keyboard.h"
#include "../../utils/io/io.h"
#include <stdint.h>

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void help_cmd_helper(char* name, char* desc) {
    kprint("\n  - ", 0x0F);
    kprint(name, 0x09);
    kprint(" - ", 0x01);
    kprint(desc, 0x09);
}

void parse_cmd(char* cmd) {
    if (strcmp(cmd, "hlt") == 0 || strcmp(cmd, "halt") == 0) asm volatile ("hlt");
    else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
        const char* start_banner_part = "====================";
        const char* end_banner = "==============================================";
        kprint(start_banner_part, 0x09);
        kprint(" Help ", 0x07);
        kprint(start_banner_part, 0x09);
        
        help_cmd_helper("hlt/halt", "Stop kernel");
        help_cmd_helper("help/?", "This help");
        kprint("\n", 0x00);
        kprint(end_banner, 0x09);
        kprint("\n", 0x00);
    }
    else kprint("Unknown command.\n", 0x04);
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
            char c = keyboard_process_byte(scancode);

            if (c != 0) {
                if (c == '\n') {
                    cmd_buffer[cmd_index] = '\0';
                    kprint("\n", 0x0F);
                    
                    if (cmd_index > 0) parse_cmd(cmd_buffer);

                    cmd_index = 0;
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
                        char str[2];
                        str[0] = c;
                        str[1] = '\0';
                        kprint(str, 0x0F);
                        
                        cmd_buffer[cmd_index] = c;
                        cmd_index++;
                    }
                }
            }
        }
    }
}

