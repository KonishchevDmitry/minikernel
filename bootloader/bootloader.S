// Kernel MBR bootloader
//
// See https://wiki.osdev.org/MBR_(x86) for details

.code16

.global _start
_start:
    jmp $0, $_init_segment_registers
    _init_segment_registers:
        mov $0, %ax
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs
        mov %ax, %ss
        mov $_start, %sp

    movzxb %dl, %bx # drive number

    push $(_end - _start + 2)
    push $_greeting_message
    call printlnf
    add $4, %sp

    push %bx
    call load_kernel
    add $2, %sp

    jmp kernel_start + 2

    _greeting_message: .string "Bootloader is running (size=%d)..."

// void load_kernel(int drive_number)
load_kernel:
    enter $0, $0

    push $0     # buffer offset
    push $1     # count
    push $1     # start sector
    push 4(%bp) # drive number
    call read_disk
    add $6, %sp

    mov kernel_size, %ax
    push %ax
    push $_kernel_loading_message
    call printlnf
    add $4, %sp

    call get_kernel_sectors
    dec %ax
    jz _load_kernel_return

    push $512   # buffer offset
    push %ax    # count
    push $2     # start sector
    push 4(%bp) # drive number
    call read_disk
    add $6, %sp

    _load_kernel_return:
        push $_kernel_loaded_message
        call printlnf
        add $2, %sp

        leave
        ret

    _kernel_loading_message: .string "Loading kernel (size=%d)..."
    _kernel_loaded_message: .string "Kernel loaded."

// int get_kernel_sectors()
get_kernel_sectors:
    xor %dx, %dx
    mov kernel_size, %ax
    mov $512, %cx
    div %cx

    cmp $0, %dx
    je _get_kernel_sector_return
    inc %ax

    _get_kernel_sector_return:
        ret

// void read_disk(int drive_number, int start_sector, int sector_count, int buffer_offset)
read_disk:
    enter $0, $0
    push %si

    mov 10(%bp), %ax
    mov %ax, _read_disk_memory_offset

    mov 8(%bp), %ax
    mov %ax, _read_disk_sector_count

    mov 6(%bp), %ax
    mov %ax, _read_disk_start_sector

    // https://wiki.osdev.org/ATA_in_x86_RealMode_(BIOS)
    mov $0x42, %ah # Extended Read Sectors From Drive
    mov 4(%bp), %dx # Drive number
    mov $_read_disk_address_packet, %si
    int $0x13
    jc _read_disk_error

    pop %si
    leave
    ret

    _read_disk_error:
        movzx %ah, %ax

        push %ax
        push $_read_disk_error_message
        call printlnf
        jmp halt

        _read_disk_error_message: .string "Disk read error (code=%d)"

    .align 4
    _read_disk_address_packet:
        .byte 16    # size
        .byte 0     # padding
        _read_disk_sector_count:
        .word 0     # count (limited to 127, but it's the exact limit on our kernel size, taking into account the separate reading of first sector)
        _read_disk_memory_offset:
        .word 0     # memory offset
        .word 0x7E0 # memory segment ((0x7C00 + 512) >> 4)
        _read_disk_start_sector:
        .quad 0     # sector to start from

// void printc(int c)
printc:
    jmp printc_bios

#define ARCH_BITS 16
#define ASMLIB_MINIMAL 1
#include "../lib/asm.S"

_end:

# MBR magic number
.org 510
.word 0xAA55

// See https://wiki.osdev.org/Memory_Map_(x86) for usable memory details
kernel_start:
kernel_size:
