.code16

header:
kernel_size:
    .word 0

.global _start
_start:
    push $_greeting_message
    call printlnf
    add $2, %sp

    pushl $2
    pushl $3
    calll c_test
    add $8, %sp

    push %ax
    push $_c_call_message
    call printlnf
    add $4, %sp

    push $_stop_execution_message
    call printlnf
    jmp halt

    _c_call_message: .string "C call result: %d"

    _greeting_message: .string "Kernel is running..."
    _stop_execution_message: .string "Stopping the execution."

// void printc(int c)
.global printc
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