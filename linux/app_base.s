.section .text
    .global _start
    _start:
        push $0
        call print_args_and_env
        add $4, %esp

        call print_cpu_info

        push $0xFFFFFFFF
        push $test_format_arg_uint_max
        push $123
        push $test_format_arg_123
        push $0
        push $test_format_arg_zero
        push $test_format_string
        call printlnf
        add $(7 * 4), %esp

        push $test_format_arg_uint_max
        push $test_format_arg_123
        push $test_format_arg_zero
        push $test_format_error
        call printlnf
        add $(4 * 4), %esp

        push $0
        call exit

    // void print_args_and_env(bool print_env)
    .type print_args_and_env, @function
    print_args_and_env:
        push %esi

        lea 16(%esp), %esi # argv list is preceded by argc value
        sub $4, %esp       # print arg

        movl $running_message, (%esp)
        call prints

        _print_args_and_env_args_loop:
            cmpl $0, (%esi)
            je _print_args_and_env

            movl $' ', (%esp)
            call printc

            mov (%esi), %eax
            mov %eax, (%esp)
            call prints

            add $4, %esi
            jmp _print_args_and_env_args_loop

        _print_args_and_env:
            movl $'\n', (%esp)
            call printc

            cmpl $1, 12(%esp) # print_env
            jne _print_args_and_env_return

            movl $environment_message, (%esp)
            call printlnf

            add $4, %esi

        _print_args_and_env_env_loop:
            cmpl $0, (%esi)
            je _print_args_and_env_return

            movl $list_prefix, (%esp)
            call prints

            mov (%esi), %eax
            mov %eax, (%esp)
            call prints

            movl $'\n', (%esp)
            call printc

            add $4, %esi
            jmp _print_args_and_env_env_loop

        _print_args_and_env_return:
            add $4, %esp
            pop %esi
            ret

    // void print_cpu_info()
    .type print_cpu_info, @function
    print_cpu_info:
        push %ebx
        push %edi
        push %esi

        mov $0x80000000, %eax
        cpuid

        cmp $0xF80000004, %eax
        jb _unsupported_cpu

        mov $0x80000002, %esi
        mov $cpu_name, %edi

        _print_cpu_info_loop:
            mov %esi, %eax
            cpuid
            mov %eax,  0(%edi)
            mov %ebx,  4(%edi)
            mov %ecx,  8(%edi)
            mov %edx, 12(%edi)

            inc %esi
            add $16, %edi

            cmpl $0x80000004, %esi
            jbe _print_cpu_info_loop

        push $cpu_name
        push $running_on_message
        call printlnf
        add $8, %esp

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
        push $error_message_prefix
        call prints
        add $4, %esp

        call printf_inner

        push $'\n'
        call printc

        mov $1, %esp
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
