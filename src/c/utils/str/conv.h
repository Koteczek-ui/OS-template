#ifndef UTIL_CONV_H
#define UTIL_CONV_H

#include <stdint.h>
#include <stddef.h>
#include "../io/io.h"

static inline char* itoa(int val, char* s, int radix) {
    char *rc, *ptr, *low;
    if (radix < 2 || radix > 94) { *s = '\0'; return s; }
    rc = ptr = s;
    if (val < 0 && radix == 10) *ptr++ = '-';
    low = ptr;
    do {
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()_+-={}|\\][:\"';<>?/.,"[(val < 0 ? -val : val) % radix];
    } while ((val = (val < 0 ? -val : val) / radix));
    *ptr-- = '\0';
    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

static inline void outoa(uint16_t port, int val, int radix) {
    char buf[33];
    itoa(val, buf, radix);
    for (int i = 0; buf[i] != '\0'; i++) outb(port, buf[i]);
}

#endif

