.section .text
.globl _start
    _start:
        call print_cpu_info

        pushl $0
        call exit

    print_cpu_info:
        push %ebx
        push %edi
        push %esi

        mov $0x80000000, %eax
        cpuid

        cmpl $0x80000004, %eax
        jb _unsupported_cpu

        movl $0x80000002, %esi
        movl $cpu_name, %edi

        _print_cpu_info_loop:
            movl %esi, %eax
            cpuid
            movl %eax,  0(%edi)
            movl %ebx,  4(%edi)
            movl %ecx,  8(%edi)
            movl %edx, 12(%edi)

            inc %esi
            add $16, %edi

            cmpl $0x80000004, %esi
            jbe _print_cpu_info_loop

        pushl $cpu_name
        pushl $running_on_message
        call printf
        addl $(4 * 2), %esp

        pop %esi
        pop %edi
        pop %ebx
        ret

        _unsupported_cpu:
            mov $unsupported_cpu_message, %esi
            call die

    # Prints %esi and exits
    die:
        pushl %esi
        pushl $error_message
        pushl $2
        call dprintf
        addl $(4 * 3), %esp

        pushl $1
        call exit

.section .bss
    .lcomm cpu_name, 12

.section .rodata
    running_on_message:
        .asciz "Running on %s\n"

    unsupported_cpu_message:
        .asciz "Unsupported CPU"

    error_message:
        .asciz "Error: %s.\n"
