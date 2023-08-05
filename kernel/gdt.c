// FIXME(konishchev): Read:
// * https://wiki.osdev.org/Global_Descriptor_Table
// * https://habr.com/ru/articles/467289/

#include <assert.h>
#include <types.h>

typedef enum {
    CODE_SEGMENT  = 0b1010,
    DATA_SEGMENT  = 0b0010,
    STACK_SEGMENT = 0b0110,
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

static_assert(sizeof(GdtEntry) == 8, "Unexpected GDT entry size");

typedef struct {
    u16 size;
    void* base;
} __attribute__((packed)) GdtDescriptor;

static_assert(sizeof(GdtDescriptor) == 6, "Unexpected GDTR size");

static GdtEntry GDT[] = {
    {},
    {
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
    {
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
    }
};

GdtDescriptor GDTR = {
    .base = &GDT,
    .size = sizeof GDT - 1,
};