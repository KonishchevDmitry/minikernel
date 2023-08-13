// See https://wiki.osdev.org/PIC for details

#include <types.h>

#include "misc.h"
#include "pic.h"
#include "textio.h"

const u8 PIC_PORTS = 8;

const u16 MASTER_COMMAND_PORT = 0x20;
const u16 MASTER_DATA_PORT = 0x21;
const u8 MASTER_OFFSET = 0x20;

const u16 SLAVE_COMMAND_PORT = 0xA0;
const u16 SLAVE_PIC_DATA_PORT = 0xA1;
const u8 SLAVE_OFFSET = MASTER_OFFSET + PIC_PORTS;

// Reinitializes the PIC giving them new vector offsets rather than 0x8 and 0x70 as configured by BIOS.
//
// The initialization command makes PIC to wait for three extra initialization words (ICW) on data port:
// * Its vector offset (ICW2)
// * How it's wired to master/slaves (ICW3)
// * Additional information about the environment (ICW4)
void configure_pic() {
    const u8 ICW1_INIT = 0x10; // Initialization
    const u8 ICW1_ICW4 = 0x01; // Indicates that ICW4 will be present
    const u8 ICW4_8086 = 0x01; // 8086/88 (MCS-80/85) mode

    u8 master_mask = inb(MASTER_DATA_PORT);
    u8 slave_mask = inb(SLAVE_PIC_DATA_PORT);

    printlnf("Remapping PIC (master_mask=%bb, slave_mask=%bb)...", master_mask, slave_mask);

    outb(MASTER_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);
    outb(SLAVE_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);

    outb(MASTER_DATA_PORT, MASTER_OFFSET);
    outb(SLAVE_PIC_DATA_PORT, SLAVE_OFFSET);

    outb(MASTER_DATA_PORT,    1 << 2); // Where slave is connected
    outb(SLAVE_PIC_DATA_PORT, 1 << 1); // Where master is connected

    outb(MASTER_DATA_PORT, ICW4_8086);
    outb(SLAVE_PIC_DATA_PORT, ICW4_8086);

    outb(MASTER_DATA_PORT, master_mask);
    outb(SLAVE_PIC_DATA_PORT, slave_mask);
}