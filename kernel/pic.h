#ifndef PIC_H
#define PIC_H

#include <types.h>

typedef struct {
    bool master;
    u8 interrupt;
    bool spurious;
} PicIrq;

void configure_pic();
bool classify_irq(u8 irq, PicIrq* pic_irq);
void ack_irq(PicIrq irq);

#endif