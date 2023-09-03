#include <stdarg.h>
#include <types.h>

#include "interrupts.h"
#include "misc.h"
#include "pit.h"
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

void memzero(void* ptr, size_t size) {
    for(size_t i = 0; i < size; i++) {
        ((u8*) ptr)[i] = 0;
    }
}

void halt() {
    asm volatile ("hlt");
}

void sleep(time_ms_t duration) {
    time_ms_t deadline = uptime() + duration;
    while(uptime() < deadline) {
        halt();
    }
}

void panic(const char* s, ...) {
    printf("Panic: ");

    va_list args;
    va_start(args, s);
    printf_args(s, args);
    va_end(args);

    printf("\n");

    disable_interrupts();
    while(true) {
        halt();
    }
}