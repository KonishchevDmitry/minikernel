BUILD_DIR := ./build

.PHONY: all boot bootloader clean debug gdb-boot gdb-kernel image kernel test
.EXTRA_PREREQS := Makefile | $(BUILD_DIR)

BOOT_CMD := qemu-system-i386 -drive file=$(BUILD_DIR)/disk.img,format=raw -display curses -monitor stdio
GDB_CMD := gdb --quiet --command debug.gdb

all: test boot

test: image
	make -C linux

boot: image
	$(BOOT_CMD)

debug: image
	$(BOOT_CMD) -S -gdb unix:$(BUILD_DIR)/gdb.socket,server,nowait

gdb-boot:
	$(GDB_CMD) --symbols bootloader/build/bootloader

gdb-kernel:
	$(GDB_CMD) --symbols kernel/build/kernel

image: bootloader kernel
	@set -eu; \
		size="$$(stat --printf=%s bootloader/build/bootloader.bin)"; \
		[ "$$size" -eq 512 ] || { echo "Invalid bootloader size: $$size" >&2; exit 1; }
	cat bootloader/build/bootloader.bin kernel/build/kernel.img > $@

kernel:
	make -C kernel

bootloader:
	make -C bootloader

$(BUILD_DIR):
	[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	make -C linux clean
	make -C kernel clean
	make -C bootloader clean