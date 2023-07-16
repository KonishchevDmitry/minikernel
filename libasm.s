.section .text
    // void prints(const char* s)
    .global prints
    prints:
        pushl %ebx
        movl 8(%esp), %ebx # current data pointer

        sub $4, %esp # current symbol

        _prints_loop:
            movzxb (%ebx), %eax

            cmpl $0, %eax
            je _prints_return

            mov %eax, (%esp)
            call printc

            inc %ebx
            jmp _prints_loop

        _prints_return:
            add $4, %esp
            popl %ebx
            ret

    // void printf(const char* format, ...)
    .global printf
    printf:
        pushl %ebx
        pushl %esi

        movl 12(%esp), %ebx # current format string pointer
        leal 16(%esp), %esi # current argument pointer

        sub $4, %esp # current symbol

        _printf_loop:
            movzxb (%ebx), %eax
            inc %ebx

            cmpl $0, %eax
            je _printf_return

            cmpl $'%', %eax
            je _printf_format

        _printf_char:
            mov %eax, (%esp)
            call printc
            jmp _printf_loop

        _printf_format:
            movzxb (%ebx), %eax
            inc %ebx

            cmpl $0, %eax
            je _printf_return

            cmpl $'s', %eax
            je _printf_string

            cmpl $'%', %eax
            je _printf_char

            jmp _printf_error

        _printf_string:
            movl (%esi), %eax
            add $4, %esi
            movl %eax, (%esp)
            call prints
            jmp _printf_loop

        _printf_error:
            subl $2, %ebx
            movl %ebx, (%esp)
            call prints

        _printf_return:
            pushl $'!'
            call printc
            popl %eax

            add $4, %esp
            popl %esi
            popl %ebx
            ret
