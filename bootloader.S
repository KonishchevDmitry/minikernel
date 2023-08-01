// Kernel MBR bootloader
//
// The labels have absolute values with 0x7C00 shift, so we don't need to alter any segment registers

.code16
#define ARCH_BITS 16

_start:
    push $(_end - _start + 2)
    push $_greeting_message
    call printlnf
    add $4, %sp

    call read_disk

    mov kernel_size, %ax
    push %ax
    push $_kernel_loading_message
    call printlnf
    add $4, %sp

    push $_stop_execution_message
    call printlnf
    jmp halt

    _greeting_message: .string "Bootloader is running (size=%d)..."
    _kernel_loading_message: .string "Loading kernel (size=%d)..."

    // FIXME(konishchev): Drop it
    _stop_execution_message: .string "Stopping the execution."

// FIXME(konishchev): Rewrite
read_disk:
    // https://en.wikipedia.org/wiki/INT_13H
    mov $0x42, %ah # Extended Read Sectors From Drive
    mov $0x80, %dl # First HDD
    mov $_disk_address_packet, %si
    int $0x13
    jc _read_disk_error

    ret

    _read_disk_error:
        movzxb %ah, %ax
        push %ax
        push $_read_disk_error_message
        call printlnf
        jmp halt

        _read_disk_error_message: .string "Disk read error (code=%d)"

    .align 4
    _disk_address_packet:
        .byte 16     # size
        .byte 0      # padding
        .word 1      # count
        .word 0x7E00 # memory offset
        .word 0      # memory segment
        start_sector:
        .quad 1      # sector to start from

// void printc(int c)
.type printc, @function
printc:
    enter $0, $0

    cmp $'\n', 4(%bp)
    je _printc_cr

    _printc:
        push %bx

        // https://en.wikipedia.org/wiki/INT_10H
        mov $0x0E, %ah
        mov 4(%bp), %al
        mov $0, %bh
        int $0x10

        pop %bx
        leave
        ret

    _printc_cr:
        push $'\r'
        call printc
        add $2, %sp
        jmp _printc

#include "libasm.S"

_end:

# MBR magic number
.org 510
.word 0xAA55

// See https://wiki.osdev.org/Memory_Map_(x86) for usable memory details
kernel_start:
kernel_size:
