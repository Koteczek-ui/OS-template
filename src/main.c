#include "utils/console.h"

void main() {
    clear(0x00);
    
    print("Hello, world!\n", 0x0F);
    print("Kernel works!", 0x02);
}

