#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"

#define KBD_DATA_PORT 0x60

static bool shift_l = false;
static bool shift_r = false;
static bool ctrl_l  = false;
static bool alt_l   = false;
static bool caps_lock = false;
static bool extended = false;

static const char kbd_map[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static const char kbd_map_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

char keyboard_process_byte(uint8_t scancode) {
    if (scancode == 0xE0) {
        extended = true;
        return 0;
    }

    bool released = (scancode & 0x80);
    uint8_t code = scancode & 0x7F;

    if (released) {
        switch (code) {
            case 0x2A: shift_l = false; break;
            case 0x36: shift_r = false; break;
            case 0x1D: ctrl_l  = false; break;
            case 0x38: alt_l   = false; break;
        }
        extended = false;
        return 0;
    } else {
        switch (code) {
            case 0x2A: shift_l = true; return 0;
            case 0x36: shift_r = true; return 0;
            case 0x1D: ctrl_l  = true; return 0;
            case 0x38: alt_l   = true; return 0;
            case 0x3A: caps_lock = !caps_lock; return 0;

            default:
                if (extended) {
                    extended = false;
                    return 0;
                } else if (code < sizeof(kbd_map)) {
                    bool upper = (shift_l || shift_r) ^ caps_lock;
                    
                    if (code == 0x0E) {
                        return '\b';
                    }

                    char ascii = upper ? kbd_map_shift[code] : kbd_map[code];
                    return ascii;
                }
                return 0;
        }
    }
}

