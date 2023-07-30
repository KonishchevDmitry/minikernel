// Core assembly library
//
// Attention: Must be written to work as:
// * Standalone library
// * .code16; .include "libasm.s"
// * .code32; .include "libasm.s"

// void prints(const char* s)
.global prints
.type prints, @function
prints:
    pushl %esi
    movl 8(%esp), %esi # current data pointer

    subl $4, %esp # current symbol

    _prints_loop:
        lodsb

        cmpb $0, %al
        je _prints_return

        movzxb %al, %eax
        movl %eax, (%esp)
        calll printc

        jmp _prints_loop

    _prints_return:
        addl $4, %esp
        popl %esi
        retl

// void printf(const char* format, ...)
.global printf
.type printf, @function
printf:
    calll printfw
    retl

// void printlnf(const char* format, ...)
.global printlnf
.type printlnf, @function
printlnf:
    calll printfw

    pushl $'\n'
    calll printc
    addl $4, %esp

    retl

// void printfw(void* _, const char* format, ...)
.global printfw
.type printfw, @function
printfw:
    pushl %esi
    pushl %ebx

    movl 16(%esp), %esi # current format string pointer
    leal 20(%esp), %ebx # current argument pointer

    subl $4, %esp # current symbol

    _printfw_loop:
        cld
        lodsb

        cmpb $'%', %al
        je _printfw_format

        cmpb $0, %al
        je _printfw_return

    _printfw_char:
        movzxb %al, %eax
        movl %eax, (%esp)
        calll printc
        jmp _printfw_loop

    _printfw_format:
        cld
        lodsb

        cmpb $'s', %al
        je _printfw_string

        cmpb $'d', %al
        je _printfw_decimal

        cmpb $'%', %al
        je _printfw_char

        jmp _printfw_error

    _printfw_string:
        movl (%ebx), %eax
        addl $4, %ebx
        movl %eax, (%esp)
        calll prints
        jmp _printfw_loop

    _printfw_decimal:
        movl (%ebx), %eax
        addl $4, %ebx
        movl %eax, (%esp)
        calll printd
        jmp _printfw_loop

    _printfw_error:
        subl $2, %esi
        movl %esi, (%esp)
        calll prints

    _printfw_return:
        addl $4, %esp
        popl %ebx
        popl %esi
        retl

// void printd(int value)
.type printd, @function
printd:
    enterl $0, $0

    movl 8(%esp), %eax # value
    movl $10, %ecx     # divisor

    _printd_backward_loop:
        movl $0, %edx
        divl %ecx

        pushl %edx

        cmpl $0, %eax
        jne _printd_backward_loop

    _printd_forward_loop:
        movl $'0', %eax
        addl %eax, (%esp)

        calll printc
        addl $4, %esp

        cmpl %esp, %ebp
        jne _printd_forward_loop

    _printd_ret:
        leavel
        retl

// void stop_execution()
.type stop_execution, @function
stop_execution:
    pushl $_stop_execution_message
    calll printlnf

    _stop_execution_loop:
        hlt
        jmp _stop_execution_loop

    _stop_execution_message: .string "Stopping the execution."