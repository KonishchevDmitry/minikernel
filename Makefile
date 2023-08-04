.PHONY: all boot clean debug gdb-boot gdb-kernel test

SHELL := /bin/bash

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

bin/bootloader.bin: bin/bootloader
	objcopy -O binary $< $@

bin/bootloader: bin/bootloader.o
	ld $(LDFLAGS) --section-start=.text=0x7C00 -o $@ $^

bin/bootloader.o: bootloader.S libasm.S | bin
	gcc $(ASFLAGS) -o $@ $<

bin/kernel.img: bin/kernel.bin
	@set -e; size="$$(stat --printf=%s bin/kernel.bin)"; \
	[ "$$size" -le 65535 ] || { echo "Too big kernel size: $$size" >&2; exit 1; }; \
	(\
		printf "%b" "$$(printf '\\x%02X\\x%02X' "$$((size % 256))" "$$((size / 256))")"; \
		tail -c +3 bin/kernel.bin; \
		[ "$$(size % 512)" -eq 0 ] || dd if=/dev/zero bs="$$((512 - size % 512))" count=1; \
	) > bin/kernel.img

bin/kernel.bin: bin/kernel
	objcopy -O binary $< $@

bin/kernel: bin/kernel.o
	ld $(LDFLAGS) --section-start=.text=0x7E00 -o $@ $^

bin/kernel.o: kernel.S libasm.S | bin
	gcc $(ASFLAGS) -o $@ $<

bin:
	[ -d bin ] || mkdir bin

clean:
	rm -rf ./bin