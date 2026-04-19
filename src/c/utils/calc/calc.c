#include "calc.h"
#include "../../utils/console/console.h"

int strlen(const char* s) {
    int l = 0;
    while (s[l]) l++;
    return l;
}

void itoa(int n, char s[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0) s[i++] = '-';
    s[i] = '\0';
    
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char tmp = s[j];
        s[j] = s[k];
        s[k] = tmp;
    }
}

int power(int base, int exp) {
    int res = 1;
    for (int i = 0; i < exp; i++) res *= base;
    return res;
}

int get_precedence(char op) {
    if (op == '|' || op == '&') return 1;
    if (op == '+' || op == '-') return 2;
    if (op == '*' || op == '/' || op == '%') return 3;
    if (op == '^') return 4;
    return 0;
}

int apply_op(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '%': return a % b;
        case '^': return power(a, b);
        case '&': return a & b;
        case '|': return a | b;
    }
    return 0;
}

int calc_expr(char* str) {
    int values[64];
    char ops[64];
    int v_top = -1;
    int o_top = -1;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ' || str[i] == '\t') continue;

        if (str[i] >= '0' && str[i] <= '9') {
            int val = 0;
            while (str[i] >= '0' && str[i] <= '9') {
                val = (val * 10) + (str[i] - '0');
                i++;
            }
            values[++v_top] = val;
            i--;
        } 
        else if (str[i] == '(') {
            ops[++o_top] = str[i];
        } 
        else if (str[i] == ')') {
            while (o_top >= 0 && ops[o_top] != '(') {
                int val2 = values[v_top--];
                int val1 = values[v_top--];
                values[++v_top] = apply_op(val1, val2, ops[o_top--]);
            }
            if (o_top >= 0) o_top--; 
        } 
        else {
            while (o_top >= 0 && ops[o_top] != '(' && get_precedence(ops[o_top]) >= get_precedence(str[i])) {
                int val2 = values[v_top--];
                int val1 = values[v_top--];
                values[++v_top] = apply_op(val1, val2, ops[o_top--]);
            }
            ops[++o_top] = str[i];
        }
    }

    while (o_top >= 0) {
        int val2 = values[v_top--];
        int val1 = values[v_top--];
        values[++v_top] = apply_op(val1, val2, ops[o_top--]);
    }

    return (v_top >= 0) ? values[0] : 0;
}

void handle_calc(char* cmd) {
    int result = calc_expr(cmd);
    char res_str[32];
    
    kprint("Result: ", 0x07);
    itoa(result, res_str);
    kprint(res_str, 0x0A);
    kprint("\n", 0x0F);
}

