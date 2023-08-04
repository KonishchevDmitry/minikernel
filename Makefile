.PHONY: all boot clean debug gdb-boot gdb-kernel test

BUILD_DIR := ./build

BOOT_CMD := qemu-system-i386 -drive file=$(BUILD_DIR)/disk.img,format=raw -display curses -monitor stdio
GDB_CMD := gdb --quiet --command debug.gdb

all: test boot

test: $(BUILD_DIR)/disk.img
	make -C linux

boot: $(BUILD_DIR)/disk.img
	$(BOOT_CMD)

debug: $(BUILD_DIR)/disk.img
	$(BOOT_CMD) -S -gdb unix:$(BUILD_DIR)/gdb.socket,server,nowait

gdb-boot:
	$(GDB_CMD) --symbols bootloader/build/bootloader

gdb-kernel:
	$(GDB_CMD) --symbols kernel/build/kernel

$(BUILD_DIR)/disk.img: Makefile | $(BUILD_DIR)
	make -C bootloader
	make -C kernel
	@set -eu; \
		size="$$(stat --printf=%s bootloader/build/bootloader.bin)"; \
		[ "$$size" -eq 512 ] || { echo "Invalid bootloader size: $$size" >&2; exit 1; }
	cat bootloader/build/bootloader.bin kernel/build/kernel.img > $@

$(BUILD_DIR):
	[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	make -C linux clean
	make -C kernel clean
	make -C bootloader clean