#ifndef PM_H
#define PM_H

#include <types.h>

#include "multiboot.h"

#define PAGE_SIZE 4096

typedef size_t physaddr_t;
typedef size_t pageframe_t;

error __must_check pm_configure(const MultibootTagMmap* mmap, pageframe_t* vm_start_page, pageframe_t* pm_end_page);
pageframe_t __must_check pm_allocate_page();

inline physaddr_t pm_page_to_addr(pageframe_t page) {
    return page * PAGE_SIZE;
}

#endif