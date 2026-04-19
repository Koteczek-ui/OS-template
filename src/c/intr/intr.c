#include "intr.h"
#include "idt.h"
#include "isr.h"
#include "../utils/console/console.h"

char *ex_msgs[] = {
    ISR_0,  ISR_1,  ISR_2,  ISR_3,  ISR_4,  ISR_5,  ISR_6,  ISR_7,
    ISR_8,  ISR_9,  ISR_10, ISR_11, ISR_12, ISR_13, ISR_14, ISR_15,
    ISR_16, ISR_17, ISR_18, ISR_19, ISR_20, ISR_21, ISR_22, ISR_23,
    ISR_24, ISR_25, ISR_26, ISR_27, ISR_28, ISR_29, ISR_30, ISR_31
};

void interrupts_init() {
    idt_install();
}

void kernel_panic_msg(char* msg) {
    kprint("\n\nKERNEL PANIC\n", 0xF4);
    kprint("CRITICAL kernel ERROR\n", 0x04);
    kprint("error code: ", 0x05);
    kprint(msg, 0x06);
    kprint("\n", 0x00);
}

void isr_handler(struct registers regs) {
    if (regs.int_no < 32) {
        char* msg = ex_msgs[regs.int_no];

        kernel_panic_msg(msg);
        
        for (;;);
    }
}

