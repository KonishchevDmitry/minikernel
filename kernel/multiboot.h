// See for details:
// * https://wiki.osdev.org/Multiboot
// * https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <types.h>

typedef struct {
    u32 size;
    u32 reserved;
    char tags[0];
} __attribute__((packed)) MultibootInfo;

typedef struct {
    u32 type;
    u32 size;
} __attribute__((packed)) MultibootTagHeader;

typedef struct {
    MultibootTagHeader tag;
    u32 entry_size;
    u32 entry_version;
    char entries[0];
} __attribute__((packed)) MultibootTagMmap;

typedef struct {
    u64 addr;
    u64 size;
    u32 type;
    u32 reserved;
} __attribute__((packed)) MultibootMmapEntry;

enum MultibootInfoFlags {
    MULTIBOOT_TAG_TYPE_END  = 0,
    MULTIBOOT_TAG_TYPE_MMAP = 6,
};

enum MultibootMemoryType {
    MULTIBOOT_MEMORY_AVAILABLE        = 1,
    MULTIBOOT_MEMORY_RESERVED         = 2,
    MULTIBOOT_MEMORY_ACPI_RECLAIMABLE = 3,
    MULTIBOOT_MEMORY_NVS              = 4,
    MULTIBOOT_MEMORY_BADRAM           = 5,
};

void* multiboot_get_data(MultibootInfo* info, size_t offset, size_t size);

#endif
