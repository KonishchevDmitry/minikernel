target remote ./bin/gdb.socket

break _start

# https://sourceware.org/gdb/onlinedocs/gdb/TUI-Commands.html
tui new-layout my {-horizontal src 1 regs 1} 2 status 0 cmd 1
tui layout my
tui focus cmd