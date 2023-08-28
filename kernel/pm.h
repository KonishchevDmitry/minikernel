#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>

#include "multiboot.h"

typedef size_t physaddr_t;

error __must_check pm_configure(const MultibootTagMmap* mmap);

#endif