#include "utils/console/console.h"
#include "verinfo.h"
#include "func/miniterminal/miniterminal.h"
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

__attribute__((section(".entry")))
void main() {
    setup();
    start_terminal();
}

