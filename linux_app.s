.section .text
    // void printc(int c)
    .globl printc
    printc:
        push %ebx

        movl $SYSCALL_WRITE, %eax
        movl $STDOUT, %ebx
        leal 8(%esp), %ecx
        movl $1, %edx
        int $SYSCALL_INT

        pop %ebx
        ret

    // void exit(int status)
    .global exit
    exit:
        movl $SYSCALL_EXIT, %eax
        movl 4(%esp), %ebx
        int $SYSCALL_INT

.section .rodata
    .set SYSCALL_INT, 0x80
    .set SYSCALL_WRITE, 4
    .set SYSCALL_EXIT, 1

    .set STDOUT, 1
