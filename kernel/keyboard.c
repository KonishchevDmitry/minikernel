#include "interrupts.h"
#include "keyboard.h"
#include "pic.h"
#include "textio.h"

#include "isr_start.h"
    static void interrupt_handler(irq_t) {
        printlnf("A key pressed.");
    }
#include "isr_end.h"

error __must_check configure_keyboard() {
    return set_interrupt_handler(PIC_KEYBOARD_IRQ, interrupt_handler);
}