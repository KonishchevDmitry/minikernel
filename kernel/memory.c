#include "memory.h"
#include "textio.h"

error __must_check configure_memory(const MultibootTagMmap* mmap) {
    const void* start = mmap->entries;
    const void* end = start - sizeof(MultibootTagMmap) + mmap->tag.size;

    if(
        mmap->tag.type != MULTIBOOT_TAG_TYPE_MMAP ||
        mmap->entry_size < sizeof(MultibootMmapEntry) ||
        start == end || (end - start) % mmap->entry_size != 0
    ) {
        return "Got an invalid mmap";
    }

    printlnf("Memory map:");

    bool huge = false;
    while(start < end) {
        const MultibootMmapEntry* mmap_entry = start;
        huge |= mmap_entry->addr > 0xFFFFFFFF || mmap_entry->size > 0xFFFFFFFF;

        const char* type = "other";
        switch(mmap_entry->type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                type = "available";
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                type = "reserved";
                break;
        }

        if(huge) {
            printlnf(" 0x%qx %qU %s", mmap_entry->addr, mmap_entry->size, type);
        } else {
            printlnf(" 0x%lx %lU %s", (u32) mmap_entry->addr, (u32) mmap_entry->size, type);
        }

        start += mmap->entry_size;
    }

    return NULL;
}