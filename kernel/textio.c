#include <stdarg.h>
#include <types.h>

#include "textio.h"
#include "vga.h"

static void printc(char c) {
    printc_vga(c);
}

static void print_string(const char* s) {
    while(*s) {
        printc(*s++);
    }
}

static void print_digit(int value) {
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

static void print_binary(u32 value, int bits) {
    for(u32 mask = 1 << (bits - 1); mask != 0; mask >>= 1) {
        printc('0' + ((value & mask) != 0));
    }
}

void printf_args(const char* s, va_list args) {
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
                print_digit(va_arg(args, int));
                break;

            case 'b':
                int bits;

                switch((c = *s++)) {
                    case 'b':
                        bits = 8;
                        break;

                    case 'w':
                        bits = 16;
                        break;

                    case 'd':
                        bits = 32;
                        break;

                    default:
                        print_string(s - 3);
                        return;
                }

                print_binary(va_arg(args, int), bits);
                break;

            case 's':
                print_string(va_arg(args, const char*));
                break;

            case '%':
                printc(c);
                break;

            default:
                print_string(s - 2);
                return;
        }
    }
}

void printf(const char* s, ...) {
    va_list args;
    va_start(args, s);
    printf_args(s, args);
    va_end(args);
}

void printlnf(const char* s, ...) {
    va_list args;
    va_start(args, s);
    printf_args(s, args);
    va_end(args);
    printc('\n');
}