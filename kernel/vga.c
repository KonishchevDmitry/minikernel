// See for details:
// * https://wiki.osdev.org/Text_mode
// * https://wiki.osdev.org/Text_Mode_Cursor
// * http://www.brokenthorn.com/Resources/OSDev10.html

#include <types.h>

#include "misc.h"
#include "vga.h"

static struct {
    volatile u16* memory;
    u8 width;
    u8 height;
    u16 pos;
} VGA = {
    .memory = (volatile u16*) 0xB8000,
    .width = 80,
    .height = 25,
    .pos = 0,
};

const u16 COLOR = 0x0700;

const u16 CRT_INDEX_REGISTER = 0x03D4;
const u16 CRT_DATA_REGISTER = 0x03D5;

void configure_vga() {
    // Getting current cursor position (which is left from BIOS)

    outb(CRT_INDEX_REGISTER, 0x0F);
    u8 low_pos = inb(CRT_DATA_REGISTER);

    outb(CRT_INDEX_REGISTER, 0x0E);
    u8 high_pos = inb(CRT_DATA_REGISTER);

    VGA.pos = ((u16) high_pos) << 8 | low_pos;
}

void move_cursor() {
    u16 pos = VGA.pos;

    outb(CRT_INDEX_REGISTER, 0x0F);
    outb(CRT_DATA_REGISTER, pos & 0xFF);

    outb(CRT_INDEX_REGISTER, 0x0E);
    outb(CRT_DATA_REGISTER, pos >> 8);
}

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
    move_cursor();
}

void printc_vga(char c) {
    bool need_cursor_update = false;

    if(c == '\n') {
        VGA.pos += VGA.width - VGA.pos % VGA.width;
        need_cursor_update = true;
    } else {
        VGA.memory[VGA.pos++] = COLOR | c;
    }

    if(VGA.pos >= VGA.width * VGA.height) {
        scroll_screen();
        need_cursor_update = false;
    }

    if(need_cursor_update) {
        move_cursor();
    }
}