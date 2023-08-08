#include "gdt.h"

.code16

header:
kernel_size:
    .word 0

.global _start
_start:
    push $_greeting_message
    call real_mode_printlnf
    add $2, %sp

    call ensure_a20_address_line

    cli
        calll configure_gdt

        call enter_protected_mode

        // After entering protected mode we've lost all BIOS services. GDT is not applied yet, but we are expected to
        // make a far jump to 32-bit code with proper segment selector + load the proper segment selector to all other
        // segment registers.

        ljmp $(GDT_KERNEL_CODE_INDEX * GDT_ENTRY_SIZE), $_start_32

    _greeting_message: .string "Kernel is running..."

// void real_mode_printc(int c)
.global real_mode_printc
real_mode_printc:
    jmp real_mode_printc_bios

.code32

_start_32:
    // FIXME(konishchev): IDT: https://habr.com/ru/articles/467289/
    //sti

    // We've made a far jump here which loaded the segment selector to CS register.
    // Now we need to update all other segment registers to apply GDT.
    mov $(GDT_KERNEL_DATA_INDEX * GDT_ENTRY_SIZE), %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    // Configure VGA to be able to print something
    call configure_vga

    push $_protected_mode_greeting_message
    call printlnf
    add $4, %esp

    push $_stop_execution_message
    call printlnf
    add $4, %esp
    call halt

    _protected_mode_greeting_message: .string "Entered protected mode."
    _stop_execution_message: .string "Stopping the execution."
