// See https://wiki.osdev.org/Paging for details
//
// Don't try to be Higher Half Kernel (https://wiki.osdev.org/Higher_Half_Kernel) yet to simplify kernel bootstrapping

#include <assert.h>
#include <types.h>

#include "misc.h"
#include "pm.h"
#include "textio.h"
#include "vm.h"

#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024

typedef struct {
    bool present:1;
    bool writable:1;
    bool user:1;
    bool write_through:1;
    bool disable_cache:1;
    bool accessed:1;
    u8 available_1:1;
    bool page_size:1;
    u8 available_2:4;
    u32 page_table:20;
} __attribute__((packed)) PageDirectoryEntry;

static_assert(sizeof(PageDirectoryEntry) == 4, "Unexpected page directory entry size");

typedef struct {
    bool present:1;
    bool writable:1;
    bool user:1;
    bool write_through:1;
    bool disable_cache:1;
    bool accessed:1;
    bool dirty:1;
    bool page_attribute_table:1;
    bool global:1;
    u8 available:3;
    u32 page_frame:20;
} __attribute__((packed)) PageTableEntry;

static_assert(sizeof(PageTableEntry) == 4, "Unexpected page table entry size");

static physaddr_t PAGE_DIRECTORY;
const char* const NO_MEMORY_ERROR = "Not enough memory";

PageTableEntry new_page_table_entry(pageframe_t page, bool user) {
    return (PageTableEntry) {
        .present = true,
        .writable = true,
        .user = user,
        .page_frame = page,
    };
}

PageDirectoryEntry new_page_directory_entry(pageframe_t table, bool user) {
    return (PageDirectoryEntry) {
        .present = true,
        .writable = true,
        .user = user,
        .page_table = table,
    };
}

error __must_check map_physical_addresses(
    PageDirectoryEntry* directory, pageframe_t pm_start_page, pageframe_t pm_end_page, size_t vm_offset
) {
    for(pageframe_t pm_page = pm_start_page; pm_page < pm_end_page; pm_page++) {
        pageframe_t vm_page = pm_page + vm_offset;

        size_t directory_index = vm_page / PAGE_TABLE_SIZE;
        size_t table_index = vm_page % PAGE_TABLE_SIZE;

        PageDirectoryEntry* directory_entry = directory + directory_index;
        PageTableEntry* page_table;

        if(directory_entry->present) {
            page_table = (PageTableEntry*) pm_page_to_addr(directory_entry->page_table);
        } else {
            pageframe_t page_table_page = pm_allocate_page();
            if(!page_table_page) {
                return NO_MEMORY_ERROR;
            }

            page_table = (PageTableEntry*) pm_page_to_addr(page_table_page);
            memzero(page_table, PAGE_SIZE);

            *directory_entry = new_page_directory_entry(page_table_page, false);
        }

        page_table[table_index] = new_page_table_entry(pm_page, false);
    }

    return NULL;
}

error __must_check vm_configure(pageframe_t vm_start_page, pageframe_t pm_end_page) {
    error err;

    pageframe_t directory_page = pm_allocate_page();
    if(!directory_page) {
        return NO_MEMORY_ERROR;
    }

    PageDirectoryEntry* directory = (PageDirectoryEntry*) pm_page_to_addr(directory_page);
    memzero(directory, PAGE_SIZE);

    if((err = map_physical_addresses(directory, 0, vm_start_page, 0))) {
        return err;
    }

    if((err = map_physical_addresses(directory, 0, pm_end_page, VM2PM_OFFSET / PAGE_SIZE))) {
        return err;
    }

    PAGE_DIRECTORY = (physaddr_t) directory;
    asm volatile (
        "mov %[page_directory], %%cr3\n\t"
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0"
        :: [page_directory] "r"(directory) : "eax"
    );

    physaddr_t mapped_value = *((physaddr_t*) physaddr_to_virtual((physaddr_t) &PAGE_DIRECTORY));
    if(mapped_value != (physaddr_t) directory) {
        return "Paging configuration error";
    }

    printlnf("Paging is enabled.");
    return NULL;
}