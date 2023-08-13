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

void (*INTERRUPT_HANDLERS[IDT_MAX_ENTRIES])(int irq);

#pragma GCC push_options
#pragma GCC target("general-regs-only")
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