#ifndef INITS_H
#define INITS_H

#include "utils/console/console.h"
#include "intr/intr.h"
#include "verinfo.h"
#include "utils/mem/mem.h"
#include "utils/vfs/vfs.h"

static inline void print_welcome() {
    if (KLOAD_SHOW_MSG) kprint("Kernel loaded!");
    if (KALOAD_SHOW_HELLO_MSG) kprint("Hello, world!\n", 0x0F);
    if (KALOAD_SHOW_SUCCESS_MSG) {
        kprint("Success: ", 0x0A);
        kprint("Kernel works!", 0x02);
        kprint(" ", 0x0F);
        kprint(KVER, 0x09);
    } else {
        if (KALOAD_SHOW_VER) {
            if (!KALOAD_SHOW_ONLY_VER) kprint("Kernel ", 0x02);
            kprint(KVER, 0x09);
        }
    }
    kprint("\n", 0x00);
}

static inline void console_init() {
    if (KINIT_CLS_SCR) kcls(0x00);
    if (KINIT_PRINT_WELCOME) print_welcome();
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
    console_init();
    basic_init();
}

#endif
