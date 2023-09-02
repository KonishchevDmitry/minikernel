#ifndef PIC_H
#define PIC_H

#include <types.h>

#include "interrupts.h"

typedef struct {
    bool master;
    u8 interrupt;
    bool spurious;
} PicIrq;

#define PIC_PORTS 8
#define PIC_MASTER_OFFSET 0x20
#define PIC_SLAVE_OFFSET (PIC_MASTER_OFFSET + PIC_PORTS)

#define PIC_TIMER_IRQ    (PIC_MASTER_OFFSET + 0)
#define PIC_KEYBOARD_IRQ (PIC_MASTER_OFFSET + 1)

void configure_pic();
bool classify_irq(irq_t irq, PicIrq* pic_irq);
void ack_irq(PicIrq irq);

#endif