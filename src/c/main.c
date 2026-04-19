#include "utils/console/console.h"
#include "utils/keyboard/keyboard.h"
#include "utils/io/io.h"
#include "verinfo.h"

void print_welcome() {
    print("Hello, world!\n", 0x0F);
    print("Kernel works!", 0x02);
    print(" ", 0x0F);
    print(VER, 0x0F);
    print("\n", 0x0F);
}

void setup() {
    clear(0x00);
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

                        print(" ", 0x0F);

                        set_cur_pos(pos);
                    }
                } else {
                    char str[2];
                    str[0] = c;
                    str[1] = '\0';
                    print(str, 0x0F);
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

