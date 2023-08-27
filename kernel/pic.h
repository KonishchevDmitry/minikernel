#ifndef PIC_H
#define PIC_H

#include <types.h>

typedef struct {
    bool master;
    u8 interrupt;
    bool spurious;
} PicIrq;

static const u8 PIC_PORTS = 8;
static const u8 PIC_MASTER_OFFSET = 0x20;
static const u8 PIC_SLAVE_OFFSET = PIC_MASTER_OFFSET + PIC_PORTS;

static const u8 PIC_TIMER_IRQ = PIC_MASTER_OFFSET + 0;

void configure_pic();
bool classify_irq(u8 irq, PicIrq* pic_irq);
void ack_irq(PicIrq irq);

#endif