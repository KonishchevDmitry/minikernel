#include <types.h>

#include "textio.h"

void main() {
    printlnf("The system is ready.");

    while(true) {
        asm volatile ("hlt");
    }
}