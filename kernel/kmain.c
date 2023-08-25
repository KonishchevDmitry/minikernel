#include <types.h>

#include "interrupts.h"
#include "misc.h"
#include "multiboot.h"
#include "pm.h"
#include "textio.h"

static error __must_check configure(const MultibootInfo* multiboot_info) {
    error err;
    size_t multiboot_offset = 0;
    bool memory_configured = 0;

    while(true) {
        const MultibootTagHeader* tag = multiboot_get_data(multiboot_info, multiboot_offset, sizeof(MultibootTagHeader));
        if(!tag) {
            return "Unexpected end of multiboot info";
        } else if(tag->type == MULTIBOOT_TAG_TYPE_END) {
            break;
        }

        if(tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            if(tag->size < sizeof(MultibootTagMmap) || memory_configured) {
                return "Got an invalid mmap";
            }

            const MultibootTagMmap* mmap = multiboot_get_data(multiboot_info, multiboot_offset, tag->size);
            if(!mmap) {
                return "Unexpected end of multiboot info";
            }

            if((err = pm_configure(mmap))) {
                return err;
            }
            memory_configured = true;
        }

        multiboot_offset += tag->size;
    }

    if(!memory_configured) {
        return "Multiboot info has no memory map";
    }

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