// See https://wiki.osdev.org/Text_mode for details
// FIXME(konishchev): https://wiki.osdev.org/Text_Mode_Cursor

#include <types.h>

#include "vga.h"

static struct {
    volatile u16* memory;
    u16 width;
    u16 height;
    u32 pos;
} VGA = {
    .memory = (volatile u16*) 0xB8000,
    .width = 80,
    .height = 25,
    .pos = 0,
};

const u16 COLOR = 0x0700;

static void scroll_screen() {
    volatile u16* dst = VGA.memory;
    volatile u16* src = dst + VGA.width;
    volatile u16* end = dst + VGA.width * VGA.height;

    while(src != end) {
        *dst++ = *src++;
    }

    while(dst != end) {
        *dst++ = COLOR | ' ';
    }

    VGA.pos -= VGA.width;
}

void printc_vga(char c) {
    if(c == '\n') {
        VGA.pos += VGA.width - VGA.pos % VGA.width;
    } else {
        VGA.memory[VGA.pos++] = COLOR | c;
    }

    if(VGA.pos >= VGA.width * VGA.height) {
        scroll_screen();
    }
}