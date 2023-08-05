target remote ./build/gdb.socket

break _start

# https://sourceware.org/gdb/onlinedocs/gdb/TUI-Commands.html
tui new-layout my-src {-horizontal src 1 regs 1} 2 status 0 cmd 1
tui new-layout my-asm {-horizontal asm 1 regs 1} 2 status 0 cmd 1

tui layout my-src
tui focus cmd