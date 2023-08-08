#include <types.h>

void halt() {
    asm volatile ("cli");

    while(true) {
        asm volatile ("hlt");
    }
}