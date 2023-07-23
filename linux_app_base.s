.section .text
    .global _start
    _start:
        call print_cpu_info

        pushl $0xFFFFFFFF
        pushl $test_format_arg_uint_max
        pushl $123
        pushl $test_format_arg_123
        pushl $0
        pushl $test_format_arg_zero
        pushl $test_format_string
        call printlnf
        addl $(7 * 4), %esp

        pushl $test_format_arg_uint_max
        pushl $test_format_arg_123
        pushl $test_format_arg_zero
        pushl $test_format_error
        call printlnf
        addl $(4 * 4), %esp

        pushl $0
        call exit

    .type print_cpu_info, @function
    print_cpu_info:
        push %ebx
        push %edi
        push %esi

        mov $0x80000000, %eax
        cpuid

        cmpl $0xF80000004, %eax
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
        call printlnf
        addl $8, %esp

        pop %esi
        pop %edi
        pop %ebx
        ret

        _unsupported_cpu:
            mov $unsupported_cpu_message, %esi
            call die

    // void die(const char* format, ...)
    .type die, @function
    die:
        pushl $error_message_prefix
        call prints
        add $4, %esp

        call printfw

        pushl $'\n'
        call printc

        movl $1, %esp
        call exit

.section .rodata
    running_on_message: .asciz "Running on %s"
    unsupported_cpu_message: .asciz "Unsupported CPU"
    error_message_prefix: .asciz "Error: "

    test_format_string: .asciz "Test formatting: %s=%d, %s=%d, %% (percent), %s=%d"
    test_format_arg_zero: .asciz "zero"
    test_format_arg_123: .asciz "123"
    test_format_arg_uint_max: .asciz "4294967295"

    test_format_error: .asciz "Test format error: zero=%s, 123=%s, %e (invalid), %s (invalid)"

.section .bss
    cpu_name: .zero 48
