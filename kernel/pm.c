// See https://wiki.osdev.org/Memory_Map_(x86) for details

#include <constants.h>

#include "kernel.h"
#include "misc.h"
#include "pm.h"
#include "textio.h"
#include "vm.h"

typedef struct {
    pageframe_t page;
} PhysicalMemoryTableEntry;

typedef struct {
    size_t size;
    pageframe_t min_page;
    pageframe_t end_page;
    PhysicalMemoryTableEntry free[MAX_PM_PAGES];
} PhysicalMemoryTable;

PhysicalMemoryTable* PM_TABLE = (PhysicalMemoryTable*) 0x00100000;

static bool pm_has_page(pageframe_t page) {
    if(page < PM_TABLE->min_page) {
        return true;
    }

    for(size_t i = 0; i < PM_TABLE->size; i++) {
        if(PM_TABLE->free[i].page == page) {
            return true;
        }
    }

    return false;
}

static void pm_add_memory(u64 addr, u64 size) {
    u64 page = addr / PAGE_SIZE;

    u64 odd_size = addr % PAGE_SIZE;
    if(odd_size != 0) {
        u64 offset = PAGE_SIZE - odd_size;
        if(size < offset) {
            return;
        }
        page++;
        size -= offset;
    }

    for(u64 end_page = page + size / PAGE_SIZE; page < end_page && page < MAX_PM_PAGES; page++) {
        if(!pm_has_page(page)) {
            if(PM_TABLE->size >= MAX_PM_PAGES) {
                panic("Physical memory table overflow.");
            }

            PM_TABLE->free[PM_TABLE->size++] = (PhysicalMemoryTableEntry){
                .page = page
            };

            if(page >= PM_TABLE->end_page) {
                PM_TABLE->end_page = page + 1;
            }
        }
    }
}

error __must_check pm_configure(const MultibootTagMmap* mmap, pageframe_t* vm_start_page, pageframe_t* pm_end_page) {
    physaddr_t kernel_start = (physaddr_t) &KERNEL_HEADER;
    physaddr_t kernel_end = kernel_start + KERNEL_HEADER.kernel_size;

    physaddr_t pm_table_start = (physaddr_t) PM_TABLE;
    physaddr_t pm_table_end = pm_table_start + sizeof *PM_TABLE;

    printlnf("Kernel image: 0x%x - 0x%x", kernel_start, kernel_end);
    if(kernel_end > pm_table_start) {
        return "The kernel is loaded at an unexpected address";
    }

    const void* entries = mmap->entries;
    const void* entries_end = entries - sizeof(MultibootTagMmap) + mmap->tag.size;

    if(
        mmap->tag.type != MULTIBOOT_TAG_TYPE_MMAP ||
        mmap->entry_size < sizeof(MultibootMmapEntry) ||
        entries == entries_end || (entries_end - entries) % mmap->entry_size != 0
    ) {
        return "Got an invalid mmap";
    }

    bool huge = false;
    bool configured = false;

    printlnf("Memory map:");

    while(entries < entries_end) {
        const MultibootMmapEntry* mmap_entry = entries;
        entries += mmap->entry_size;

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

        if(mmap_entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            continue;
        }

        if(!configured) {
            if(mmap_entry->addr > pm_table_start || mmap_entry->addr + mmap_entry->size < pm_table_end) {
                continue;
            }

            PM_TABLE->size = 0;

            PM_TABLE->min_page = pm_table_end / PAGE_SIZE;
            if(pm_table_end % PAGE_SIZE != 0) {
                PM_TABLE->min_page++;
            }
            PM_TABLE->end_page = PM_TABLE->min_page;

            configured = true;
        }

        pm_add_memory(mmap_entry->addr, mmap_entry->size);
    }

    if(!configured) {
        return "Got an unsupported memory map";
    }

    printlnf("Available memory: %u KB", PM_TABLE->size * PAGE_SIZE / KB);

    *vm_start_page = PM_TABLE->min_page;
    *pm_end_page = PM_TABLE->end_page;

    return NULL;
}

pageframe_t __must_check pm_allocate_page() {
    if(PM_TABLE->size == 0) {
        return 0;
    }
    return PM_TABLE->free[--PM_TABLE->size].page;
}