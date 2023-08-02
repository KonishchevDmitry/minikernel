.code16
#define ARCH_BITS 16

header:
kernel_size:
    .word 0

_start:
    push $_greeting_message
    call printlnf
    add $2, %sp

    push $_stop_execution_message
    call printlnf
    jmp halt

    _greeting_message: .string "Kernel is running..."
    _stop_execution_message: .string "Stopping the execution."

// void printc(int c)
printc:
    enter $0, $0

    cmpw $'\n', 4(%bp)
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