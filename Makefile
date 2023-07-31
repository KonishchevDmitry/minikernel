.PHONY: all boot clean debug gdb-boot test

SHELL := /bin/bash

ASFLAGS := -c -g -m32
LDFLAGS := -m elf_i386
BOOT_CMD := qemu-system-i386 -drive file=bin/disk.img,format=raw -display curses -monitor stdio

all: test boot

$(wildcard bin/*.o): Makefile

test: bin/linux-app bin/linux-stdlib-app
	./bin/linux-app --arg value
	./bin/linux-stdlib-app --arg value

boot: bin/disk.img
	$(BOOT_CMD)

debug: bin/disk.img
	$(BOOT_CMD) -S -gdb unix:bin/gdb.socket,server,nowait

gdb-boot:
	gdb --quiet --symbols bin/bootloader --command debug.gdb

bin/disk.img: bin/bootloader.bin bin/kernel.img
	@size="$$(stat --printf=%s bin/bootloader.bin)"; [ "$$size" -eq 512 ] || { echo "Invalid bootloader size: $$size" >&2; exit 1; }
	cat bin/bootloader.bin bin/kernel.img > bin/disk.img

bin/bootloader.bin: bin/bootloader
	objcopy -O binary $< $@

bin/bootloader: bin/bootloader.o
	ld $(LDFLAGS) --section-start=.text=0x7C00 -o $@ $^

bin/bootloader.o: bootloader.s libasm.s | bin
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
	ld $(LDFLAGS) -o $@ $^

bin/kernel.o: kernel.s | bin
	gcc $(ASFLAGS) -o $@ $<

bin/linux-app: bin/linux_app.o bin/linux_app_base.o bin/libasm.o
	ld $(LDFLAGS) -o $@ $^

bin/linux_app.o: linux_app.S | bin
	gcc $(ASFLAGS) -o $@ $<

# Requires gcc-multilib package
bin/linux-stdlib-app: bin/linux_stdlib_app.o bin/linux_app_base.o bin/libasm.o
	ld $(LDFLAGS) -dynamic-linker /lib/ld-linux.so.2 -o $@ $^ -lc

bin/linux_stdlib_app.o: linux_stdlib_app.s | bin
	gcc $(ASFLAGS) -o $@ $<

bin/linux_app_base.o: linux_app_base.s | bin
	gcc $(ASFLAGS) -o $@ $<

bin/libasm.o: libasm.s | bin
	gcc $(ASFLAGS) -o $@ $<

bin:
	[ -d bin ] || mkdir bin

clean:
	rm -rf ./bin