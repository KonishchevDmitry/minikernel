#include <types.h>

#include "interrupts.h"
#include "textio.h"

void main() {
    printlnf("The system is ready.");

    while(true) {
        interrupts_health_check();
        asm volatile ("hlt");
    }
}