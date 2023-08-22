#include <types.h>

#include "multiboot.h"

struct {
    MultibootInfo header;
    char buffer[200];
} __attribute__((packed)) MULTIBOOT_INFO = {
    .header = {
        .size = sizeof MULTIBOOT_INFO.header,
    },
};

error MULTIBOOT_BUFFER_EXHAUSTED_ERROR = "multiboot buffer is exhausted";

static void* multiboot_allocate(size_t size) {
    size_t pos = MULTIBOOT_INFO.header.size - sizeof MULTIBOOT_INFO.header;
    if(pos + size > sizeof MULTIBOOT_INFO.buffer) {
        return NULL;
    }

    void* memory = MULTIBOOT_INFO.buffer + pos;
    MULTIBOOT_INFO.header.size += size;

    return memory;
}

typedef struct {
    u64 addr;
    u64 size;
    u32 type;
    u32 acpi_flags;
} __attribute__((packed)) SmapEntry;

enum AcpiExtendedAttributes {
    ACPI_EXTENDED_ATTRIBUTE_ACTUAL       = 1 << 0,
    ACPI_EXTENDED_ATTRIBUTE_NON_VOLATILE = 1 << 1,
};

static error __must_check get_memory_mapping() {
    // See https://wiki.osdev.org/Detecting_Memory_(x86) for details

    MultibootTagMmap* mmap = multiboot_allocate(sizeof(MultibootTagMmap));
    if(!mmap) {
        return MULTIBOOT_BUFFER_EXHAUSTED_ERROR;
    }

    *mmap = (MultibootTagMmap) {
        .tag = {
            .type = MULTIBOOT_TAG_TYPE_MMAP,
            .size = sizeof(MultibootTagMmap),
        },
        .entry_size = sizeof(MultibootMmapEntry),
        .entry_version = 0,
    };

    int cursor = 0;
    const int smap_magic = 0x534D4150;

    do {
        int magic;
        bool smap_error;

        SmapEntry smap;
        int smap_size;

        asm volatile (
            "int $0x15"
            : "=a"(magic), "=@ccc"(smap_error), "=m"(smap), "=c"(smap_size), "=b"(cursor)
            : "a"(0xE820), "d"(smap_magic), "b"(cursor), "D"(&smap), "c"(sizeof smap)
        );
        smap_size &= 0xFF;

        if(magic != smap_magic || smap_error || smap_size < 20) {
            return "Failed to get SMAP entry";
        }

        if(smap_size >= 24) {
            if(!(smap.acpi_flags & ACPI_EXTENDED_ATTRIBUTE_ACTUAL)) {
                continue;
            }

            if(smap.acpi_flags & ACPI_EXTENDED_ATTRIBUTE_NON_VOLATILE && smap.type == MULTIBOOT_MEMORY_AVAILABLE) {
                smap.type = MULTIBOOT_MEMORY_RESERVED;
            }
        }

        MultibootMmapEntry* mmap_entry = multiboot_allocate(sizeof(MultibootMmapEntry));
        if(!mmap_entry) {
            return MULTIBOOT_BUFFER_EXHAUSTED_ERROR;
        }

        mmap->tag.size += sizeof *mmap_entry;
        *mmap_entry = (MultibootMmapEntry) {
            .addr = smap.addr,
            .size = smap.size,
            .type = smap.type,
        };
    } while(cursor);

    return NULL;
}

error __must_check collect_multiboot_info() {
    error err;

    if((err = get_memory_mapping())) {
        return err;
    }

    MultibootTagHeader* end_tag = multiboot_allocate(sizeof(MultibootTagHeader));
    if(!end_tag) {
        return MULTIBOOT_BUFFER_EXHAUSTED_ERROR;
    }

    *end_tag = (MultibootTagHeader) {
        .type = MULTIBOOT_TAG_TYPE_END,
        .size = sizeof(*end_tag),
    };

    return NULL;
}