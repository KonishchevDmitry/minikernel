#ifndef VM_H
#define VM_H

#include <types.h>

#include "pm.h"

#define MAX_VM_PAGES (((u64) UINT32_MAX + 1) / PAGE_SIZE)
#define MAX_PM_PAGES (MAX_VM_PAGES / 2)
#define VM2PM_OFFSET (MAX_PM_PAGES * PAGE_SIZE)

extern const char* const NO_MEMORY_ERROR;

error __must_check vm_configure(pageframe_t vm_start_page, pageframe_t pm_end_page);

inline void* physaddr_to_virtual(physaddr_t addr) {
    return (void*) addr + VM2PM_OFFSET;
}

#endif