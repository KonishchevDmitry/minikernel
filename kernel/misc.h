#ifndef MISC_H
#define MISC_H

#include <types.h>

u8 inb(u16 port);
void outb(u16 port, u8 value);

void halt();
void panic(const char* s, ...);

#endif