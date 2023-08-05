// Core assembly library
//
// Attention:
//
// It must support both 16-bit and 32-bit mode. 16-bit variant shouldn't use 32-bit commands and registers because it
// leads to much bigger binary size, but we need to keep it compact to fit into MBR.
//
// Please note that 16-bit addressing mode doesn't support SP in base/index expressions.

#include "arch.S"

// void prints(const char* s)
.global prints
.type prints, @function
prints:
    enter $0, $0
    push %SI

    mov S(2)(%BP), %SI

    _prints_loop:
        cld
        lodsb

        cmp $0, %al
        je _prints_return

        movzxb %al, %AX
        push %AX
        call printc
        add $S(1), %SP

        jmp _prints_loop

    _prints_return:
        pop %SI
        leave
        ret

// void printf(const char* format, ...)
.global printf
.type printf, @function
printf:
    call printf_inner
    ret

// void printlnf(const char* format, ...)
.global printlnf
.type printlnf, @function
printlnf:
    call printf_inner

    push $'\n'
    call printc
    add $S(1), %SP

    ret

// void printf_inner(void* ret, const char* format, ...)
.global printf_inner
.type printf_inner, @function
printf_inner:
    enter $0, $0
    push %BX
    push %SI

    mov S(3)(%BP), %SI # current format string pointer
    lea S(4)(%BP), %BX # current argument pointer

    _printf_inner_loop:
        cld
        lodsb

        cmp $'%', %al
        je _printf_inner_format

        cmp $0, %al
        je _printf_inner_return

    _printf_inner_char:
        movzxb %al, %AX
        push %AX
        call printc
        add $S(1), %SP
        jmp _printf_inner_loop

    _printf_inner_format:
        cld
        lodsb

        cmp $'s', %al
        je _printf_inner_string

        cmp $'d', %al
        je _printf_inner_decimal

        cmp $'%', %al
        je _printf_inner_char

        jmp _printf_inner_error

    _printf_inner_string:
        mov (%BX), %AX
        add $S(1), %BX
        push %AX
        call prints
        add $S(1), %SP
        jmp _printf_inner_loop

    _printf_inner_decimal:
        mov (%BX), %AX
        add $S(1), %BX
        push %AX
        call printd
        add $S(1), %SP
        jmp _printf_inner_loop

    _printf_inner_error:
        sub $2, %SI
        push %SI
        call prints
        add $S(1), %SP

    _printf_inner_return:
        pop %SI
        pop %BX
        leave
        ret

// void printd(int value)
.type printd, @function
printd:
    enter $0, $0

    mov S(2)(%BP), %AX # value
    mov $10, %CX       # divisor

    _printd_backward_loop:
        xor %DX, %DX
        div %CX

        add $'0', %DX
        push %DX

        cmp $0, %AX
        jne _printd_backward_loop

    _printd_forward_loop:
        call printc
        add $S(1), %SP

        cmp %SP, %BP
        jne _printd_forward_loop

    _printd_ret:
        leave
        ret

#if ARCH_BITS == 16
// void printc_bios(int c)
.global printc_bios
.type printc_bios, @function
printc_bios:
    enter $0, $0

    cmpw $'\n', 4(%bp)
    je _printc_bios_cr

    _printc_bios:
        push %bx

        // https://en.wikipedia.org/wiki/INT_10H
        mov $0x0E, %ah
        mov 4(%bp), %al
        mov $0, %bh
        int $0x10

        pop %bx
        leave
        ret

    _printc_bios_cr:
        push $'\r'
        call printc_bios
        add $2, %sp
        jmp _printc_bios
#endif

// void halt()
.global halt
.type halt, @function
halt:
    cli
    _halt_loop:
        hlt
        jmp _halt_loop
