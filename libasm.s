.section .text
    // void prints(const char* s)
    .global prints
    prints:
        pushl %esi
        movl 8(%esp), %esi # current data pointer

        sub $4, %esp # current symbol

        _prints_loop:
            lodsb

            cmpb $0, %al
            je _prints_return

            movzxb %al, %eax
            mov %eax, (%esp)
            call printc

            jmp _prints_loop

        _prints_return:
            add $4, %esp
            popl %esi
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
        pushl %esi
        pushl %ebx

        movl 16(%esp), %esi # current format string pointer
        leal 20(%esp), %ebx # current argument pointer

        sub $4, %esp # current symbol

        _printfw_loop:
            cld
            lodsb

            cmpb $0, %al
            je _printfw_return

            cmpb $'%', %al
            je _printfw_format

        _printfw_char:
            movzxb %al, %eax
            mov %eax, (%esp)
            call printc
            jmp _printfw_loop

        _printfw_format:
            cld
            lodsb

            cmpb $0, %al
            je _printfw_return

            cmpb $'s', %al
            je _printfw_string

            cmpb $'%', %al
            je _printfw_char

            jmp _printfw_error

        _printfw_string:
            movl (%ebx), %eax
            add $4, %ebx
            movl %eax, (%esp)
            call prints
            jmp _printfw_loop

        _printfw_error:
            subl $2, %esi
            movl %esi, (%esp)
            call prints

        _printfw_return:
            add $4, %esp
            popl %ebx
            popl %esi
            ret
