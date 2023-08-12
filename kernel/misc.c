#include <stdarg.h>
#include <types.h>

#include "misc.h"
#include "textio.h"

u8 inb(u16 port) {
    u8 result;
    asm volatile (
        "in %%dx, %%al"
        : "=a"(result)
        : "d"(port)
    );
    return result;
}

void outb(u16 port, u8 value) {
    asm volatile (
        "out %%al, %%dx"
        :: "a"(value), "d"(port)
    );
}

void halt() {
    asm volatile ("cli");

    while(true) {
        asm volatile ("hlt");
    }
}

void panic(const char* s, ...) {
    printf("Panic: ");

    va_list args;
    va_start(args, s);
    printf_args(s, args);
    va_end(args);

    printf("\n");
    halt();
}