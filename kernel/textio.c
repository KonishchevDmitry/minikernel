#include <stdarg.h>

#include "vga.h"

static void printc(char c) {
    printc_vga(c);
}

static void prints(const char* s) {
    while(*s) {
        printc(*s++);
    }
}

static void printd(int value) {
    if(value < 0) {
        printc('-');
    } else {
        value = -value;
    }

    int mul = 1;
    while(value / mul <= -10) {
        mul *= 10;
    }

    while(mul > 0) {
        int digit = -(value / mul);
        printc('0' + digit);
        value += digit * mul;
        mul /= 10;
    }
}

static void print_args(const char* s, va_list args) {
    char c;

    while((c = *s++)) {
        if(c != '%') {
            printc(c);
            continue;
        }

        switch((c = *s++)) {
            case 'c':
                printc(va_arg(args, int));
                break;

            case 'd':
                printd(va_arg(args, int));
                break;

            case 's':
                prints(va_arg(args, const char*));
                break;

            case '%':
                printc(c);
                break;

            default:
                printc('%');
                prints(s - 1);
                return;
        }
    }
}

void printf(const char* s, ...) {
    va_list args;
    va_start(args, s);
    print_args(s, args);
    va_end(args);
}

void printlnf(const char* s, ...) {
    va_list args;
    va_start(args, s);
    print_args(s, args);
    va_end(args);
    printc('\n');
}