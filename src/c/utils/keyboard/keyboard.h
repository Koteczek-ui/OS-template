#ifndef UTIL_KEYBOARD_H
#define UTIL_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

#define KBD_DATA_PORT 0x60

char keyboard_process_byte(uint8_t scancode);

#endif

