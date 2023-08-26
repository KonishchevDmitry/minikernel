BUILD_DIR := ./build

.PHONY: all boot boot-vnc bootloader clean debug gdb-boot gdb-kernel image kernel test
.EXTRA_PREREQS := Makefile | $(BUILD_DIR)

BOOT_CMD := \
	qemu-system-i386 -name minikernel,process=minikernel,debug-threads=on \
	-smp cores=1 -m 16M -drive file=$(BUILD_DIR)/disk.img,format=raw \
	-nodefaults -vga std -monitor stdio

BOOT_ARGS := -machine type=pc,accel=kvm -cpu host
DEBUG_ARGS := -machine type=pc -cpu qemu32

GDB_CMD := gdb --quiet --command debug.gdb

all: test boot

test: image
	make -C linux

boot: image
	$(BOOT_CMD) $(BOOT_ARGS) -display curses

boot-vnc: image
	$(BOOT_CMD) $(BOOT_ARGS) -display none -vnc unix:$(BUILD_DIR)/vnc.socket

debug: image
	$(BOOT_CMD) $(DEBUG_ARGS) -display curses -S -gdb unix:$(BUILD_DIR)/gdb.socket,server,nowait

gdb-boot:
	$(GDB_CMD) --symbols bootloader/build/bootloader

gdb-kernel:
	$(GDB_CMD) --symbols kernel/build/kernel

image: bootloader kernel
	@set -eu; \
		size="$$(stat --printf=%s bootloader/build/bootloader.bin)"; \
		[ "$$size" -eq 512 ] || { echo "Invalid bootloader size: $$size" >&2; exit 1; }
	cat bootloader/build/bootloader.bin kernel/build/kernel.img > $(BUILD_DIR)/disk.img

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