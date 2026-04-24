#include "miniterminal.h"
#include "../../utils/calc/calc.h"
#include "../../utils/console/console.h"
#include "../../utils/console/cur.h"
#include "../../utils/keyboard/keyboard.h"
#include "../../utils/io/io.h"
#include "../../utils/str/str.h"

#define MAX_HISTORY 25
char history[MAX_HISTORY][256];
int history_count = 0;
int history_index = -1;

int ctrl_pressed = 0;
int is_cmd_running = 0;

uint8_t hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

void help_cmd_helper(char* name, char* desc) {
    kprint("\n  - ", 0x0F);
    kprint(name, 0x09);
    kprint(" - ", 0x01);
    kprint(desc, 0x09);
}

int should_interrupt() {
    if (inb(0x64) & 1) {
        uint8_t sc = inb(0x60);
        
        if (sc == 0x1D) ctrl_pressed = 1;
        else if (sc == 0x9D) ctrl_pressed = 0;
        
        if (ctrl_pressed && sc == 0x2E) {
            kprint("\n[", 0x04);
            kprint("Interuppted", 0x0C);
            kprint("]\n", 0x04);
            return 1;
        }
    }
    return 0;
}

void handle_all_matrix_cmd() {
    char buf[2] = {0, 0};
    for (int col = 0; col < 256; col++) {
        for (int ch = 0; ch < 256; ch++) {
            if (should_interrupt()) return;

            buf[0] = (char)ch;
            if (buf[0] == '\n' || buf[0] == '\r' || buf[0] == '\b' || buf[0] == '\t' || buf[0] == '\v' || buf[0] == '\f') buf[0] = ' ';
            kprint(buf, (uint8_t)col);
        }
        kprint("\n", 0x0F);
    }
}

void handle_print_logic(char* ptr) {
    char output[2] = {0, 0};
    uint8_t color = 0x0F;
    char* last_quote = 0;
    char* tmp = ptr;

    while (*tmp) {
        if (*tmp == '\"') if (tmp == ptr || *(tmp - 1) != '\\') last_quote = tmp;
        tmp++;
    }

    if (last_quote) {
        char* search_ptr = last_quote + 1;
        while (*search_ptr == ' ' || *search_ptr == '\"') search_ptr++;
        if (*search_ptr == '0' && (*(search_ptr + 1) == 'x' || *(search_ptr + 1) == 'X')) {
            search_ptr += 2;
            uint8_t high = hex_to_int(*search_ptr);
            search_ptr++;
            uint8_t low = hex_to_int(*search_ptr);
            color = (high << 4) | low;
        }
    }

    while (*ptr && ptr != last_quote) {
        if (*ptr == '\\') {
            ptr++;
            if (*ptr == '\\') output[0] = '\\';
            else if (*ptr == '\"') output[0] = '\"';
            else if (*ptr == 'n') output[0] = '\n';
            else {
                output[0] = '\\'; 
                kprint(output, color);
                output[0] = *ptr;
            }
        } else output[0] = *ptr;
        kprint(output, color);
        if (*ptr) ptr++;
    }
    kprint("\n", 0x0F);
}

void parse_cmd(char* cmd) {
    is_cmd_running = 1;
    
    if (strcmp(cmd, "hlt") == 0 || strcmp(cmd, "halt") == 0) asm volatile ("hlt");
    else if (strcmp(cmd, "cls") == 0 || strcmp(cmd, "clear") == 0) kclear(0x00);
    else if (strncmp(cmd, "calc ", 5) == 0) handle_calc(cmd + 5);
    else if (strncmp(cmd, "kprint \"", 8) == 0) handle_print_logic(cmd + 8);
    else if (strncmp(cmd, "print \"", 7) == 0) handle_print_logic(cmd + 7);
    else if (strncmp(cmd, "mkstr \"", 7) == 0) handle_print_logic(cmd + 7);
    else if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
        kprint("============================== Help ==============================", 0x09);
        help_cmd_helper("hlt/halt", "Stop kernel");
        help_cmd_helper("cls/clear", "Clear screen");
        help_cmd_helper("calc {expr}", "Calculate math expression");
        help_cmd_helper("kprint/print/mkstr \"text\" 0xCOLORCODE", "Print text with specified color");
        help_cmd_helper("colormatrix", "Show color matrix (EPILSEPSY WARNING!)");
        help_cmd_helper("?/help", "This help");
        kprint("\n==================================================================\n", 0x09);
    }
    else if (strcmp(cmd, "colormatrix") == 0) handle_all_matrix_cmd();
    else {
        kprint("Unknown command: ", 0x04);
        kprint(cmd, 0x0C);
        kprint("\n", 0x00);
    }
    
    is_cmd_running = 0;
}

void input() { kprint("> ", 0x01); }
void cmd_out(char* cmd_buffer) { kprint(cmd_buffer, 0x03); }

void start_terminal() {
    char cmd_buffer[256];
    int cmd_index = 0;
    uint16_t prompt_pos;

    kprint("For help, type '", 0x07);
    kprint("help", 0x03);
    kprint("' or '", 0x07);
    kprint("?", 0x03);
    kprint("'.\n", 0x07);
    input();
    prompt_pos = get_cur_pos();

    while (1) {
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);

            if (scancode == 0x1D) { ctrl_pressed = 1; continue; }
            if (scancode == 0x9D) { ctrl_pressed = 0; continue; }

            if (ctrl_pressed && scancode == 0x2E) {
                if (is_cmd_running) {
                    kprint("\n[", 0x04);
                    kprint("Interuppted", 0x0C);
                    kprint("]\n", 0x04);
                } else {
                    kprint("\nError: ", 0x04);
                    kprint("There is no running command now!\n", 0x0C);
                }
                
                cmd_index = 0;
                cmd_buffer[0] = '\0';
                history_index = -1;
                input();
                prompt_pos = get_cur_pos();
                
                ctrl_pressed = 0; 
                continue;
            }

            if (scancode == 0x48) {
                if (history_count > 0 && history_index < history_count - 1) {
                    history_index++;
                    set_cur_pos(prompt_pos);
                    for(int i = 0; i < cmd_index; i++) kprint(" ", 0x0F);
                    set_cur_pos(prompt_pos);
                    strcpy(cmd_buffer, history[history_index]);
                    cmd_index = 0;
                    while(cmd_buffer[cmd_index]) cmd_index++;
                    cmd_out(cmd_buffer);
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
                    cmd_out(cmd_buffer);
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

            if (scancode == 0x49) { scroll_view_up(); continue; }
            if (scancode == 0x51) { scroll_view_down(); continue; }

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
                    input();
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
                        cmd_out(str);
                        cmd_buffer[cmd_index++] = c;
                    }
                }
            }
        }
    }
}

