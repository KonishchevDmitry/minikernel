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

static void pad_number(bool is_signed, int bits, int len) {
    int max_len;

    switch(bits) {
        case 8:
            max_len = 3;
            break;
        case 16:
            max_len = 5;
            break;
        case 32:
            max_len = 10;
            break;
        case 64:
            max_len = 20;
            break;
        default:
            return;
    }

    if(is_signed && bits != 64) {
        max_len++;
    }

    for(int i = len; i < max_len; i++) {
        printc(' ');
    }
}

static void print_signed_number(i64 value, int bits, bool padded) {
    int len = 1;
    bool negative = value < 0;

    if(negative) {
        len++;
    } else {
        value = -value;
    }

    i64 mul = 1;
    while(value / mul <= -10) {
        mul *= 10;
        len++;
    }

    if(padded) {
        pad_number(true, bits, len);
    }

    if(negative) {
        printc('-');
    }

    while(mul > 0) {
        i64 digit = -(value / mul);
        printc('0' + digit);
        value += digit * mul;
        mul /= 10;
    }
}

static void print_unsigned_number(u64 value, int bits, bool padded) {
    u64 mul = 1;
    int len = 1;

    while(value / mul >= 10) {
        mul *= 10;
        len++;
    }

    if(padded) {
        pad_number(false, bits, len);
    }

    while(mul > 0) {
        u64 digit = value / mul;
        printc('0' + digit);
        value -= digit * mul;
        mul /= 10;
    }
}

static char hex_char(u8 value) {
    if(value < 10) {
        return '0' + value;
    } else {
        return 'A' + (value - 10);
    }
}

static void print_hex(u64 value, int bits) {
    while(bits > 0) {
        bits -= 8;

        u8 byte = (value >> bits) & 0xFF;
        printc(hex_char(byte >> 4));
        printc(hex_char(byte & 0xF));
    }
}

static void print_binary(u64 value, int bits) {
    for(u64 mask = (u64) 1 << (bits - 1); mask != 0; mask >>= 1) {
        printc('0' + ((value & mask) != 0));
    }
}

static bool printf_number(char format, int bits, u64 value) {
    switch(format) {
        case 'd':
        case 'D':
            print_signed_number(value, bits, format == 'D');
            return true;

        case 'u':
        case 'U':
            print_unsigned_number(value, bits, format == 'U');
            return true;

        case 'x':
            print_hex(value, bits);
            return true;

        case 'B':
            print_binary(value, bits);
            return true;

        default:
            return false;
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

            case 's':
                print_string(va_arg(args, const char*));
                break;

            case 'd':
            case 'D':
            case 'u':
            case 'U':
            case 'x':
            case 'B':
                if(!printf_number(c, sizeof(int) * 8, va_arg(args, int))) {
                    print_string(s - 2);
                    return;
                }
                break;

            case 'b':
                if(!printf_number(*s++, 8, va_arg(args, int))) {
                    print_string(s - 3);
                    return;
                }
                break;

            case 'w':
                if(!printf_number(*s++, 16, va_arg(args, int))) {
                    print_string(s - 3);
                    return;
                }
                break;

            case 'l':
                if(!printf_number(*s++, 32, va_arg(args, int))) {
                    print_string(s - 3);
                    return;
                }
                break;

            case 'q':
                if(!printf_number(*s++, 64, va_arg(args, u64))) {
                    print_string(s - 3);
                    return;
                }
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

    move_cursor();
}

void printlnf(const char* s, ...) {
    va_list args;

    va_start(args, s);
    printf_args(s, args);
    va_end(args);

    printc('\n');
}