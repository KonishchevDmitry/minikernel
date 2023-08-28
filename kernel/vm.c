// See https://wiki.osdev.org/Paging for details
//
// Don't try to be Higher Half Kernel (https://wiki.osdev.org/Higher_Half_Kernel) yet to simplify kernel bootstrapping

#include <assert.h>
#include <constants.h> // FIXME(konishchev): Drop it
#include <types.h>

#include "textio.h"
#include "pm.h"
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

// FIXME(konishchev): Make dynamic
PageDirectoryEntry page_directory[PAGE_DIRECTORY_SIZE] __attribute__ ((aligned (PAGE_SIZE)));
PageTableEntry system_page_table[PAGE_TABLE_SIZE] __attribute__ ((aligned (PAGE_SIZE)));
PageTableEntry test_page_table[PAGE_TABLE_SIZE] __attribute__ ((aligned (PAGE_SIZE)));

// FIXME(konishchev): HERE
void vm_configure() {
    for(size_t i = 0; i < PAGE_TABLE_SIZE; i++) {
        system_page_table[i] = (PageTableEntry) {
            .present = true,
            .writable = true,
            .user = false,
            .page_frame = i,
        };
    }

    page_directory[0] = (PageDirectoryEntry) {
        .present = true,
        .writable = true,
        .user = false,
        .page_table = ((physaddr_t) system_page_table) >> 12,
    };

    int* source = (int*) (4 * MB);
    *source = 111;

    int* target = (int*) (8 * MB);
    *target = 222;

    printlnf("Before paging: %d", *target);

    test_page_table[0] = (PageTableEntry) {
        .present = true,
        .writable = true,
        .user = false,
        .page_frame = (4 * MB) >> 12,
    };
    page_directory[2] = (PageDirectoryEntry) {
        .present = true,
        .writable = true,
        .user = false,
        .page_table = ((physaddr_t) test_page_table) >> 12,
    };

    asm volatile (
        "mov %[page_directory], %%cr3\n\t"
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0"
        :: [page_directory] "r"(page_directory) : "eax"
    );
    printlnf("Paging is enabled.");

    printlnf("After paging: %d", *target);
}