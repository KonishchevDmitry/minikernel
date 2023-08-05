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
    jmp printc_bios
