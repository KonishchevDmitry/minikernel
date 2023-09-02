#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <types.h>

typedef u8 irq_t;
typedef void (*InterruptHandler)(irq_t irq);

error __must_check set_interrupt_handler(irq_t irq, InterruptHandler handler);
void interrupts_health_check();

void enable_interrupts();
void disable_interrupts();
bool interrupts_enabled();

#endif