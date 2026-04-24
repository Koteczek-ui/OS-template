#include "inits.h"
#include "func/miniterminal/miniterminal.h"

extern uint32_t next_free_lba;

void setup() {
    init();
    start_terminal();
}

__attribute__((section(".entry")))
void kmain() {
    setup();
}

