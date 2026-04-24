#ifndef UTIL_CALC_H
#define UTIL_CALC_H

#include <stdint.h>

uint32_t strlen(const char* s);
void itoa(int n, char s[]);
int get_precedence(char op);
int apply_op(int a, int b, char op);
int calc_expr(char* str);
void handle_calc(char* cmd);

#endif

