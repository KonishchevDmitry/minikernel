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

    calll collect_multiboot_info
    test %eax, %eax
    jnz _multiboot_info_error

    cli
        calll configure_gdt

        call enter_protected_mode

        // After entering protected mode we've lost all BIOS services. GDT is not applied yet, but we are expected to
        // make a far jump to 32-bit code with proper segment selector + load the proper segment selector to all other
        // segment registers.

        ljmp $(GDT_KERNEL_CODE_INDEX * GDT_ENTRY_SIZE), $_start_32

    _multiboot_info_error:
        push %ax
        push $_multiboot_info_error_message
        call real_mode_panic
        _multiboot_info_error_message: .string "Failed to collect multiboot info: %s."

    _greeting_message: .string "Kernel is running..."

// void real_mode_printc(int c)
.global real_mode_printc
real_mode_printc:
    jmp real_mode_printc_bios

.code32

_start_32:
        // We've made a far jump here which loaded the segment selector to CS register.
        // Now we need to update all other segment registers to apply GDT.
        mov $(GDT_KERNEL_DATA_INDEX * GDT_ENTRY_SIZE), %ax
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs
        mov %ax, %ss

        // Configure VGA as soon as possible to be able to print something
        call configure_vga

        call configure_interrupts

        push $_protected_mode_greeting_message
        call printlnf
        add $4, %esp
    sti

    push $MULTIBOOT_INFO
    call kmain

    _protected_mode_greeting_message: .string "Entered protected mode."

// void isr(int irq)
.align 4
.global isr
isr:
    // See https://wiki.osdev.org/Interrupt_Service_Routines for details

    push %eax
    push %ecx
    push %edx

    cld # C code following the System V ABI requires DF to be clear on function entry

    mov 12(%esp), %eax
    push %eax
    call handle_interrupt
    add $4, %esp

    pop %edx
    pop %ecx
    pop %eax
    add $4, %esp

    iret
