.PHONY: all boot clean debug gdb-boot gdb-kernel test


ASFLAGS := -c -g -m32
LDFLAGS := -m elf_i386

BOOT_CMD := qemu-system-i386 -drive file=bin/disk.img,format=raw -display curses -monitor stdio
GDB_CMD := gdb --quiet --command debug.gdb

all: test boot

$(wildcard bin/*.o): Makefile

boot: bin/disk.img
	$(BOOT_CMD)

debug: bin/disk.img
	$(BOOT_CMD) -S -gdb unix:bin/gdb.socket,server,nowait

gdb-boot:
	$(GDB_CMD) --symbols bin/bootloader

gdb-kernel:
	$(GDB_CMD) --symbols bin/kernel

bin/disk.img: bin/bootloader.bin bin/kernel.img
	@size="$$(stat --printf=%s bin/bootloader.bin)"; [ "$$size" -eq 512 ] || { echo "Invalid bootloader size: $$size" >&2; exit 1; }
	cat bin/bootloader.bin bin/kernel.img > bin/disk.img


bin:
	[ -d bin ] || mkdir bin

clean:
	rm -rf ./bin