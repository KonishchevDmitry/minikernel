// See for details:
// * https://wiki.osdev.org/IDT
// * https://wiki.osdev.org/Interrupt
// * https://wiki.osdev.org/Exceptions

#include <assert.h>
#include <types.h>

#include "idt.h"
#include "misc.h"
#include "pic.h"
#include "textio.h"

typedef enum {
    INTERRUPT_GATE = 0xE,
    TRAP_GATE = 0xF,
} GateType;

typedef struct {
    u16 isr_low;
    u16 code_selector;
    u8  reserved_1;
    GateType type: 4;
    u8 reserved_2: 1;
    u8 privilege_level: 2;
    bool present: 1;
    u16 isr_high;
} __attribute__((packed)) IdtEntry;

static_assert(sizeof(IdtEntry) == IDT_ENTRY_SIZE, "Unexpected IDT entry size");

typedef struct {
    u16 limit;
    void* base;
} __attribute__((packed)) IdtDescriptor;

static_assert(sizeof(IdtDescriptor) == 6, "Unexpected IDTR size");

extern IdtEntry IDT[IDT_MAX_ENTRIES];

static IdtDescriptor IDTR = {
    .base = IDT,
    .limit = sizeof IDT - 1,
};

static volatile size_t TOTAL_SPURIOUS_INTERRUPTS = 0;
static void (*INTERRUPT_HANDLERS[IDT_MAX_ENTRIES])(int irq);

#pragma GCC push_options
#pragma GCC target("general-regs-only")
    void handle_interrupt(int irq) {
        PicIrq pic_irq;
        bool is_pic_irq = classify_irq(irq, &pic_irq);

        if(is_pic_irq && pic_irq.spurious) {
            TOTAL_SPURIOUS_INTERRUPTS++;
        } else {
            INTERRUPT_HANDLERS[irq](irq);
        }

        if(is_pic_irq) {
            ack_irq(pic_irq);
        }
    }

    static void default_interrupt_handler(int irq) {
        panic("Unsupported interrupt received: #%d.", irq);
    }
#pragma GCC pop_options

void configure_interrupts() {
    configure_pic();

    for(size_t irq = 0; irq < IDT_MAX_ENTRIES; irq++) {
        INTERRUPT_HANDLERS[irq] = default_interrupt_handler;
    }

    asm volatile (
        "lidt (%[idtr])"
        :: [idtr] "m"(IDTR)
    );
}

static size_t REPORTED_SPURIOUS_INTERRUPTS = 0;

void interrupts_health_check() {
    size_t spurious_interrupts = TOTAL_SPURIOUS_INTERRUPTS;
    if(REPORTED_SPURIOUS_INTERRUPTS == spurious_interrupts) {
        return;
    }

    printlnf("Got %d spurious interrupts.", spurious_interrupts - REPORTED_SPURIOUS_INTERRUPTS);
    REPORTED_SPURIOUS_INTERRUPTS = spurious_interrupts;
}