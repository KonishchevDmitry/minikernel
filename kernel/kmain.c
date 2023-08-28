#include <types.h>

#include "interrupts.h"
#include "misc.h"
#include "multiboot.h"
#include "pm.h"
#include "textio.h"
#include "vm.h"

static error __must_check configure(const MultibootInfo* multiboot_info) {
    error err;
    size_t multiboot_offset = 0;
    const MultibootTagMmap* mmap = NULL;

    while(true) {
        const MultibootTagHeader* tag = multiboot_get_data(multiboot_info, multiboot_offset, sizeof(MultibootTagHeader));
        if(!tag) {
            return "Unexpected end of multiboot info";
        } else if(tag->type == MULTIBOOT_TAG_TYPE_END) {
            break;
        }

        if(tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            if(tag->size < sizeof(MultibootTagMmap) || mmap) {
                return "Got an invalid mmap";
            }

            if(!(mmap = multiboot_get_data(multiboot_info, multiboot_offset, tag->size))) {
                return "Unexpected end of multiboot info";
            }
        }

        multiboot_offset += tag->size;
    }

    if(!mmap) {
        return "Multiboot info has no memory map";
    }

    if((err = pm_configure(mmap))) {
        return err;
    }

    vm_configure();

    return NULL;
}

void kmain(const MultibootInfo* multiboot_info) {
    error err;

    if((err = configure(multiboot_info))) {
        panic("Kernel configuration error: %s.", err);
    }

    printlnf("The system is ready.");

    while(true) {
        interrupts_health_check();
        asm volatile ("hlt");
    }
}