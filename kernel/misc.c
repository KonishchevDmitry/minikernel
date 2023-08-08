#include <types.h>

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