// See https://wiki.osdev.org/GDT_Tutorial for details

#include <assert.h>
#include <types.h>

#include "gdt.h"

typedef enum {
    CODE_SEGMENT       = 0b1010,
    DATA_SEGMENT       = 0b0010,
    TASK_STATE_SEGMENT = 0b1001,
} GdtSegmentType;

typedef struct {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;

    GdtSegmentType type: 4;
    bool non_system: 1;
    u8 privilege_level: 2;
    bool present: 1;

    u8 limit_high: 4;

    bool reserved: 1;
    bool long_mode: 1;
    bool bits32: 1;
    bool page_granularity: 1;

    u8 base_high;
} __attribute__((packed)) GdtEntry;

static_assert(sizeof(GdtEntry) == GDT_ENTRY_SIZE, "Unexpected GDT entry size");

typedef struct {
    u16 size;
    void* base;
} __attribute__((packed)) GdtDescriptor;

static_assert(sizeof(GdtDescriptor) == 6, "Unexpected GDTR size");

static GdtEntry GDT[] = {
    {},
    [GDT_KERNEL_CODE_INDEX] = {
        .limit_low = 0xFFFF,
        .base_low  = 0x0000,
        .base_middle = 0x00,

        .type = CODE_SEGMENT,
        .non_system = true,
        .privilege_level = 0,
        .present = true,

        .limit_high = 0xF,
        .bits32 = true,
        .page_granularity = true,
        .base_high = 0x00,
    },
    [GDT_KERNEL_DATA_INDEX] = {
        .limit_low = 0xFFFF,
        .base_low  = 0x0000,
        .base_middle = 0x00,

        .type = DATA_SEGMENT,
        .non_system = true,
        .privilege_level = 0,
        .present = true,

        .limit_high = 0xF,
        .bits32 = true,
        .page_granularity = true,
        .base_high = 0x00,
    },
    [GDT_USER_CODE_INDEX] = {
        .limit_low = 0xFFFF,
        .base_low  = 0x0000,
        .base_middle = 0x00,

        .type = CODE_SEGMENT,
        .non_system = true,
        .privilege_level = 3,
        .present = true,

        .limit_high = 0xF,
        .bits32 = true,
        .page_granularity = true,
        .base_high = 0x00,
    },
    [GDT_USER_DATA_INDEX] = {
        .limit_low = 0xFFFF,
        .base_low  = 0x0000,
        .base_middle = 0x00,

        .type = DATA_SEGMENT,
        .non_system = true,
        .privilege_level = 3,
        .present = true,

        .limit_high = 0xF,
        .bits32 = true,
        .page_granularity = true,
        .base_high = 0x00,
    },
#if 0
    [GDT_TSS_INDEX] = {
        .limit_low = 0x0000,
        .base_low  = 0x0000,
        .base_middle = 0x00,

        .type = TASK_STATE_SEGMENT,
        .non_system = false,
        .privilege_level = 0,
        .present = true,

        .limit_high = 0x0,
        .bits32 = true,
        .page_granularity = false,
        .base_high = 0x00,
    },
#endif
};

static_assert(sizeof GDT / sizeof GDT[0] == 5, "Unexpected GDT size");

static GdtDescriptor GDTR = {
    .base = &GDT,
    .size = sizeof GDT - 1,
};

// Please note that:
// * Real mode knows nothing about GDT, so it doesn't affect it
// * Changing GDT actually doesn't apply the changes: they will be applied only on segment register change.
void configure_gdt() {
    asm volatile (
        "lgdt (%[gdtr])"
        :: [gdtr] "m"(GDTR)
    );
}