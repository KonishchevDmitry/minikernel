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
            mov $SYS_write, %eax
            mov $1, %ebx
            lea 8(%esp), %ecx
            mov $1, %edx
            int $SYSCALL_INT

            cmp $-EINTR, %eax
            je _printc_write

        pop %ebx
        ret

    // void exit(int status)
    .global exit
    .type exit, @function
    exit:
        mov $SYS_exit, %eax
        mov 4(%esp), %ebx
        int $SYSCALL_INT
