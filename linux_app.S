#include <errno.h>
#include <sys/syscall.h>

#define SYSCALL_INT 0x80

.section .text
    // void printc(int c)
    .globl printc
    .type printc, @function
    printc:
        push %ebx

        _printc_write:
            movl $SYS_write, %eax
            movl $1, %ebx
            leal 8(%esp), %ecx
            movl $1, %edx
            int $SYSCALL_INT

            cmpl $-EINTR, %eax
            je _printc_write

        pop %ebx
        ret

    // void exit(int status)
    .global exit
    .type exit, @function
    exit:
        movl $SYS_exit, %eax
        movl 4(%esp), %ebx
        int $SYSCALL_INT
