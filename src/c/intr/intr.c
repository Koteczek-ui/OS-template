#include "intr.h"
#include "idt.h"
#include "isr.h"
#include "../utils/console/console.h"
#include "../verinfo.h"
#include "../utils/str/conv.h"

void interrupts_init() { idt_install(); }

void kernel_panic_msg(int int_no) {
    char buf[12];
    static const char ex_msgs[(MAX_ISR + 1)][40] = {
        ISR_0,  ISR_1,  ISR_2,  ISR_3,  ISR_4,  ISR_5,  ISR_6,  ISR_7,
        ISR_8,  ISR_9,  ISR_10, ISR_11, ISR_12, ISR_13, ISR_14, ISR_15,
        ISR_16, ISR_17, ISR_18, ISR_19, ISR_20, ISR_21, ISR_22, ISR_23,
        ISR_24, ISR_25, ISR_26, ISR_27, ISR_28, ISR_29, ISR_30, ISR_31
    };
    
    kprint("\n\nKERNEL PANIC\n", 0xFC);
    kprint("kernel CRASHED!\n", 0x04);
    kprint("error code: ", 0x05);
    kprint(itoa(int_no, buf, 10), 0x06);
    kprint(" (", 0x08);
    
    if (int_no <= MAX_ISR) kprint(ex_msgs[int_no], 0x06);
    else kprint("<Unknown>", 0x06);
    
    kprint(")\n", 0x08);
}

void isr_handler(struct regs *r) { int no = r->int_no; if (no <= MAX_ISR) { kernel_panic_msg(no); for (;;); } }

