// See for details:
// * http://www.brokenthorn.com/Resources/OSDevPit.html
// * https://wiki.osdev.org/PIT

#include <types.h>

#include "interrupts.h"
#include "misc.h"
#include "pic.h"
#include "pit.h"

enum Port {
    COMMAND_PORT   = 0x43,
    CHANNEL0_PORT = 0x40,
};

enum Command {
    CMD_CHANNEL0              =     0,
    CMD_LOBYTE_HIBYTE         =  0b11 << 4,
    CMD_SQUARE_WAVE_GENERATOR = 0b011 << 1,
};

static const float PIT_HZ = 1193181.6666;
static const int IRQ_HZ = 100;
static volatile register_t TICKS = 0;

#include "isr_start.h"
    static void interrupt_handler(irq_t) {
        TICKS++;
    }
#include "isr_end.h"

error __must_check configure_pit() {
    bool interrupts_were_enabled = interrupts_enabled();

    if(interrupts_were_enabled) {
        disable_interrupts();
    }

    outb(COMMAND_PORT, CMD_CHANNEL0 | CMD_LOBYTE_HIBYTE | CMD_SQUARE_WAVE_GENERATOR);

    u16 divider = PIT_HZ / IRQ_HZ;
    outb(CHANNEL0_PORT, divider & 0xFF);
    outb(CHANNEL0_PORT, divider >> 8);

    if(interrupts_were_enabled) {
        enable_interrupts();
    }

    return set_interrupt_handler(PIC_TIMER_IRQ, interrupt_handler);
}

time_ms_t uptime() {
    return TICKS * (1000 / IRQ_HZ);
}