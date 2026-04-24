#ifndef INITS_H
#define INITS_H

#include "utils/console/console.h"
#include "intr/intr.h"
#include "verinfo.h"
#include "utils/mem/mem.h"
#include "utils/io/vfs.h"

static inline void print_welcome() {
    kprint("Hello, world!\n", 0x0F);
    kprint("Success: ", 0x0A);
    kprint("Kernel works!", 0x02);
    kprint(" ", 0x0F);
    kprint(KVER, 0x09);
    kprint("\n", 0x00);
}

static inline void console_init() {
    kclear(0x00);
    print_welcome();
}

static inline void fs_init() {
    heap_init();
    vfs_init();
}

static inline void basic_init() {
    interrupts_init();
    fs_init();
}

static inline void init() {
    basic_init();
    console_init();
}

#endif

