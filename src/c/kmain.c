#include "inits.h"
#include "func/miniterminal/miniterminal.h"

void setup() {
    init();
    start_terminal();
}

__attribute__((section(".entry")))
void kmain() {
    setup();
}
