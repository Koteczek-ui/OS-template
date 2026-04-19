#ifndef ISR_H
#define ISR_H

#include <stdint.h>

struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#define ISR_0 "Division By Zero"
#define ISR_1 "Debug"
#define ISR_2 "Non Maskable Interrupt"
#define ISR_3 "Breakpoint"
#define ISR_4 "Into Detected Overflow"
#define ISR_5 "Out of Bounds"
#define ISR_6 "Invalid Opcode"
#define ISR_7 "No Coprocessor"
#define ISR_8 "Double Fault"
#define ISR_9 "Coprocessor Segment Overrun"
#define ISR_10 "Bad TSS"
#define ISR_11 "Segment Not Present"
#define ISR_12 "Stack Fault"
#define ISR_13 "General Protection Fault"
#define ISR_14 "Page Fault"
#define ISR_15 "Unknown Interrupt (Intel Reserved)"
#define ISR_16 "x87 FPU Floating-Point Error"
#define ISR_17 "Alignment Check"
#define ISR_18 "Machine Check"
#define ISR_19 "SIMD Floating-Point Exception"
#define ISR_20 "Virtualization Exception"
#define ISR_21 "Control Protection Exception"
#define ISR_22 "Reserved"
#define ISR_23 "Reserved"
#define ISR_24 "Reserved"
#define ISR_25 "Reserved"
#define ISR_26 "Reserved"
#define ISR_27 "Reserved"
#define ISR_28 "Hypervisor Injection Exception"
#define ISR_29 "VMM Communication Exception"
#define ISR_30 "Security Exception"
#define ISR_31 "Reserved"

void isr_handler(struct registers regs);

#endif

