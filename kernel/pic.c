// 8259 Programmable Interrupt Controller support
//
// See for details:
// * https://wiki.osdev.org/PIC
// * http://www.brokenthorn.com/Resources/OSDevPic.html

#include <types.h>

#include "misc.h"
#include "pic.h"
#include "textio.h"

static const u8 SLAVE_PIN = 2;

static const u16 MASTER_COMMAND_PORT = 0x20;
static const u16 MASTER_DATA_PORT = 0x21;

static const u16 SLAVE_COMMAND_PORT = 0xA0;
static const u16 SLAVE_PIC_DATA_PORT = 0xA1;

// Initialization Command Words (ICW)
static const u8 ICW1_INIT = 0x10; // Initialization
static const u8 ICW1_ICW4 = 0x01; // Indicates that ICW4 will be present
static const u8 ICW4_8086 = 0x01; // 8086/88 (MCS-80/85) mode

// Operation Command Words (OCW)
static const u8 OCW2_EOI      = 0x20; // Non-specific End-of-interrupt
static const u8 OCW3_READ_ISR = 0x0B; // Read In-Service Register (sent IRQ which hasn't been acked yet)

static u8 get_status_register(bool master, u8 ocw3) {
    u16 port = master ? MASTER_COMMAND_PORT : SLAVE_COMMAND_PORT;
    outb(port, ocw3);
    return inb(port);
}

// Reinitializes the PIC giving them new vector offsets rather than 0x8 and 0x70 as configured by BIOS.
//
// The initialization command makes PIC to wait for three extra initialization words (ICW) on data port:
// * Its vector offset (ICW2)
// * How it's wired to master/slaves (ICW3)
// * Additional information about the environment (ICW4)
void configure_pic() {
    u8 master_mask = inb(MASTER_DATA_PORT);
    u8 slave_mask = inb(SLAVE_PIC_DATA_PORT);

    printlnf("Remapping PIC (master_mask=%bB, slave_mask=%bB)...", master_mask, slave_mask);

    outb(MASTER_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);
    outb(SLAVE_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);

    outb(MASTER_DATA_PORT, PIC_MASTER_OFFSET);
    outb(SLAVE_PIC_DATA_PORT, PIC_SLAVE_OFFSET);

    outb(MASTER_DATA_PORT, 1 << SLAVE_PIN);
    outb(SLAVE_PIC_DATA_PORT, SLAVE_PIN);

    outb(MASTER_DATA_PORT, ICW4_8086);
    outb(SLAVE_PIC_DATA_PORT, ICW4_8086);

    outb(MASTER_DATA_PORT, master_mask);
    outb(SLAVE_PIC_DATA_PORT, slave_mask);
}

bool classify_irq(u8 irq, PicIrq* pic_irq) {
    if(irq < PIC_MASTER_OFFSET || irq >= PIC_SLAVE_OFFSET + PIC_PORTS) {
        return false;
    }

    bool master = irq < PIC_SLAVE_OFFSET;
    u8 interrupt = irq % PIC_PORTS;

    // See https://wiki.osdev.org/PIC#Spurious_IRQs for details
    bool spurious = interrupt == 7 && !(get_status_register(master, OCW3_READ_ISR) & (1 << interrupt));

    *pic_irq = (PicIrq){
        .master = master,
        .interrupt = interrupt,
        .spurious = spurious,
    };
    return true;
}

void ack_irq(PicIrq irq) {
    if(!irq.master && !irq.spurious) {
        outb(SLAVE_COMMAND_PORT, OCW2_EOI);
    }

    if(!irq.spurious || !irq.master) {
        outb(MASTER_COMMAND_PORT, OCW2_EOI);
    }
}