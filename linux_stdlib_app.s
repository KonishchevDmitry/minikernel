.section .text
    // void printc(int c)
    .global printc
    printc:
        jmp putchar
