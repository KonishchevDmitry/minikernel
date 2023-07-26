.section .text
    .global _start
    _start:
        pushl $0
        call print_args_and_env
        addl $4, %esp

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

    // void print_args_and_env(bool print_env)
    .type print_args_and_env, @function
    print_args_and_env:
        pushl %esi

        leal 16(%esp), %esi # argv list is preceded by argc value
        subl $4, %esp       # print arg

        movl $running_message, (%esp)
        call prints

        _print_args_and_env_args_loop:
            cmpl $0, (%esi)
            je _print_args_and_env

            movl $' ', (%esp)
            call printc

            movl (%esi), %eax
            movl %eax, (%esp)
            call prints

            addl $4, %esi
            jmp _print_args_and_env_args_loop

        _print_args_and_env:
            movl $'\n', (%esp)
            call printc

            cmpl $1, 12(%esp) # print_env
            jne _print_args_and_env_return

            movl $environment_message, (%esp)
            call printlnf

            addl $4, %esi

        _print_args_and_env_env_loop:
            cmpl $0, (%esi)
            je _print_args_and_env_return

            movl $list_prefix, (%esp)
            call prints

            movl (%esi), %eax
            movl %eax, (%esp)
            call prints

            movl $'\n', (%esp)
            call printc

            addl $4, %esi
            jmp _print_args_and_env_env_loop

        _print_args_and_env_return:
            addl $4, %esp
            popl %esi
            ret

    // void print_cpu_info()
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
    running_message: .string "Running:"
    environment_message: .string "Environment:"

    running_on_message: .string "Running on %s"
    unsupported_cpu_message: .string "Unsupported CPU"

    list_prefix: .string "* "
    error_message_prefix: .string "Error: "

    test_format_string: .string "Test formatting: %s=%d, %s=%d, %% (percent), %s=%d"
    test_format_arg_zero: .string "zero"
    test_format_arg_123: .string "123"
    test_format_arg_uint_max: .string "4294967295"

    test_format_error: .string "Test format error: zero=%s, 123=%s, %e (invalid), %s (invalid)"

.section .bss
    cpu_name: .zero 48
