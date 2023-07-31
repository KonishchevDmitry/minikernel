// Kernel MBR bootloader
//
// The labels have absolute values with 0x7C00 shift, so we don't need to alter any segment registers

.code16

_start:
    pushl $(_end - _start + 2)
    pushl $_greeting_message
    calll printlnf
    addw $8, %sp

    call read_disk

    mov $_kernel_start, %eax
    movb (%eax), %al
    movzxb %al, %eax
    pushl %eax
    pushl $_greeting_message
    calll printlnf

    calll stop_execution

    _greeting_message: .string "Bootloader is running (size=%d)..."

// FIXME(konishchev): Rewrite
read_disk:
    // https://en.wikipedia.org/wiki/INT_13H
    movb $0x42, %ah # Extended Read Sectors From Drive
    movb $0x80, %dl # First HDD
    movw $_disk_address_packet, %si
    int $0x13
    jc _read_disk_error

    ret

    _read_disk_error:
        movzxb %ah, %eax
        pushl %eax
        pushl $_read_disk_error_message
        calll printlnf

        calll stop_execution

        _read_disk_error_message: .string "Disk read err (code=%d)"

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
    cmpl $'\n', 4(%esp)
    je _printc_cr

    _printc:
        pushw %bx

        // https://en.wikipedia.org/wiki/INT_10H
        movb $0x0E, %ah
        movb 6(%esp), %al
        movb $0, %bh
        int $0x10

        popw %bx
        retl

    _printc_cr:
        pushl $'\r'
        calll printc
        addw $4, %sp
        jmp _printc

.include "libasm.s"
_end:

# MBR magic number
.org 510
.word 0xAA55

// See https://wiki.osdev.org/Memory_Map_(x86) for usable memory details
_kernel_start:
