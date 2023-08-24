#ifndef KERNEL_H
#define KERNEL_H

#include <types.h>

typedef struct {
    u16 kernel_size;
} __attribute__((packed)) KernelHeader;

extern KernelHeader KERNEL_HEADER;

#endif