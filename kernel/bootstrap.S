.code16

// Ensures that A20 address line is enabled (see https://wiki.osdev.org/A20_Line for details)
// void ensure_a20_address_line()
.global ensure_a20_address_line
.type ensure_a20_address_line, @function
ensure_a20_address_line:
    push %ds
    push %es
    push %si
    push %di

    // DS:SI = 0x0000:0x0500 (0x00000500)
	xor %ax, %ax
	mov %ax, %ds
	mov $0x0500, %si

    // ES:DI = 0xFFFF:0x0510 (0x00100500)
	not %ax
	mov %ax, %es
	mov $0x0510, %di

    // Original values
    movw %ds:(%si), %cx
    movw %es:(%di), %dx

    // Test it
    movw %ax, %ds:(%si)
    movw $0, %es:(%di)
    cmpw %ax, %ds:(%si)

    // Revert changes
    movw %cx, %ds:(%si)
    movw %dx, %es:(%di)

    pop %di
    pop %si
    pop %es
    pop %ds

    jne _ensure_a20_address_line_error
    ret

    _ensure_a20_address_line_error:
        push $_ensure_a20_address_line_error_message
        call real_mode_panic

        _ensure_a20_address_line_error_message: .string "A20 address line is not enabled."

// void enter_protected_mode()
.global enter_protected_mode
.type enter_protected_mode, @function
enter_protected_mode:
    push $_enter_protected_mode_message
    call printlnf
    add $2, %sp

    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    ret

    _enter_protected_mode_message: .string "Entering protected mode..."

// void real_mode_panic(const char* format, ...)
.global real_mode_panic
.type real_mode_panic, @function
real_mode_panic:
    push $_real_mode_panic
    call prints
    add $2, %sp

    call printf_inner

    push $'\n'
    call printc
    add $2, %sp

    call halt

    _real_mode_panic: .string "Panic: "