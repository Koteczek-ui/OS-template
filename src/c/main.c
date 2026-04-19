#include "utils/console/console.h"
#include "utils/keyboard/keyboard.h"
#include "utils/io/io.h"
#include "verinfo.h"
#include "intr/intr.h"

void print_welcome() {
    kprint("Hello, world!\n", 0x0F);
    kprint("Kernel works!", 0x02);
    kprint(" ", 0x0F);
    kprint(VER, 0x0F);
    kprint("\n", 0x00);
}

void setup() {
    interrupts_init();
    kclear(0x00);
    print_welcome();
}

void input() {
    while (1) {
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);
            char c = keyboard_process_byte(scancode);

            if (c != 0) {
                if (c == '\b') {
                    uint16_t pos = get_cur_pos();
                    
                    if (pos > 0) {
                        pos--;
                        set_cur_pos(pos);

                        kprint(" ", 0x0F);

                        set_cur_pos(pos);
                    }
                } else {
                    char str[2];
                    str[0] = c;
                    str[1] = '\0';
                    kprint(str, 0x0F);
                }
            }
        }
    }
}

__attribute__((section(".entry")))
void main() {
    setup();
    input();
}

