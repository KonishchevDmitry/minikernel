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
        call printfw
        ret

    // void printlnf(const char* format, ...)
    .global printlnf
    printlnf:
        call printfw

        pushl $'\n'
        call printc
        add $4, %esp

        ret

    // void printfw(void* _, const char* format, ...)
    .global printfw
    printfw:
        pushl %ebx
        pushl %esi

        movl 16(%esp), %ebx # current format string pointer
        leal 20(%esp), %esi # current argument pointer

        sub $4, %esp # current symbol

        _printfw_loop:
            movzxb (%ebx), %eax
            inc %ebx

            cmpl $0, %eax
            je _printfw_return

            cmpl $'%', %eax
            je _printfw_format

        _printfw_char:
            mov %eax, (%esp)
            call printc
            jmp _printfw_loop

        _printfw_format:
            movzxb (%ebx), %eax
            inc %ebx

            cmpl $0, %eax
            je _printfw_return

            cmpl $'s', %eax
            je _printfw_string

            cmpl $'%', %eax
            je _printfw_char

            jmp _printfw_error

        _printfw_string:
            movl (%esi), %eax
            add $4, %esi
            movl %eax, (%esp)
            call prints
            jmp _printfw_loop

        _printfw_error:
            subl $2, %ebx
            movl %ebx, (%esp)
            call prints

        _printfw_return:
            add $4, %esp
            popl %esi
            popl %ebx
            ret
