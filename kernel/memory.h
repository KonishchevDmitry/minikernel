#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>

#include "multiboot.h"

error __must_check configure_memory(MultibootTagMmap* mmap);

#endif