.section .text
    // void printc(int c)
    .global printc
    .type printc, @function
    printc:
        jmp putchar
