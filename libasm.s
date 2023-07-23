.section .text
    // void prints(const char* s)
    .global prints
    .type prints, @function
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
    .type printf, @function
    printf:
        call printfw
        ret

    // void printlnf(const char* format, ...)
    .global printlnf
    .type printlnf, @function
    printlnf:
        call printfw

        pushl $'\n'
        call printc
        add $4, %esp

        ret

    // void printfw(void* _, const char* format, ...)
    .global printfw
    .type printfw, @function
    printfw:
        pushl %esi
        pushl %ebx

        movl 16(%esp), %esi # current format string pointer
        leal 20(%esp), %ebx # current argument pointer

        sub $4, %esp # current symbol

        _printfw_loop:
            cld
            lodsb

            cmpb $'%', %al
            je _printfw_format

            cmpb $0, %al
            je _printfw_return

        _printfw_char:
            movzxb %al, %eax
            mov %eax, (%esp)
            call printc
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
            add $4, %ebx
            movl %eax, (%esp)
            call prints
            jmp _printfw_loop

        _printfw_decimal:
            movl (%ebx), %eax
            add $4, %ebx
            movl %eax, (%esp)
            call printd
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

            call printc
            addl $4, %esp

            cmpl %esp, %ebp
            jne _printd_forward_loop

        _printd_ret:
            leavel
            ret