// Kernel MBR bootloader
//
// The labels have absolute values with 0x7C00 shift, so we don't need to alter any segment registers

.code16

_start:
    pushl $(_end - _start)
    pushl $_greeting_message
    calll printlnf
    addw $8, %sp

    calll stop_execution

    _greeting_message: .string "Bootloader is running (size=%d)..."

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
