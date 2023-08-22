#include "multiboot.h"

void* multiboot_get_data(MultibootInfo* info, size_t offset, size_t size) {
    if(sizeof(MultibootInfo) + offset + size > info->size) {
        return NULL;
    }
    return info->tags + offset;
}