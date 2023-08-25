// See https://wiki.osdev.org/Memory_Map_(x86) for details

#include "kernel.h"
#include "misc.h"
#include "pm.h"
#include "textio.h"
#include "vm.h"

#define MAX_ADDR UINT32_MAX
#define MAX_PAGES (((u64) MAX_ADDR + 1) / PAGE_SIZE)

typedef size_t physaddr_t;

typedef struct {
    size_t page_id;
} PhysicalMemoryTableEntry;

typedef struct {
    size_t size;
    size_t min_page_id;
    PhysicalMemoryTableEntry free[MAX_PAGES];
} PhysicalMemoryTable;

PhysicalMemoryTable* PM_TABLE = (PhysicalMemoryTable*) 0x00100000;

// FIXME(konishchev): static or drop
size_t pm_addr_to_page(physaddr_t addr) {
    if(addr % PAGE_SIZE != 0) {
        panic("An attempt to translate an invalid address to page number: %x", addr);
    }
    return addr / PAGE_SIZE;
}

static bool pm_has_page(size_t page_id) {
    if(page_id < PM_TABLE->min_page_id) {
        return true;
    }

    for(size_t i = 0; i < PM_TABLE->size; i++) {
        if(PM_TABLE->free[i].page_id == page_id) {
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

    for(u64 end_page = page + size / PAGE_SIZE; page < end_page && page < MAX_PAGES; page++) {
        if(!pm_has_page(page)) {
            if(PM_TABLE->size >= MAX_PAGES) {
                panic("Physical memory table overflow.");
            }
            PM_TABLE->free[PM_TABLE->size++] = (PhysicalMemoryTableEntry){.page_id = page};
        }
    }
}

error __must_check pm_configure(const MultibootTagMmap* mmap) {
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

            PM_TABLE->min_page_id = pm_table_end / PAGE_SIZE;
            if(pm_table_end % PAGE_SIZE != 0) {
                PM_TABLE->min_page_id++;
            }

            configured = true;
        }

        pm_add_memory(mmap_entry->addr, mmap_entry->size);
    }

    if(!configured) {
        return "Got an unsupported memory map";
    }

    printlnf("Available memory: %u", PM_TABLE->size * PAGE_SIZE);
    return NULL;
}