SHELL := /bin/bash
BUILD_DIR := ./build

.PHONY: clean
.EXTRA_PREREQS := Makefile | $(BUILD_DIR)

ASFLAGS := -m32 -g -c
CFLAGS := -g -c -I ../lib -Wall -Wextra -Werror -nostdlib -fno-builtin -ffreestanding -fno-stack-protector -fno-pie
LDFLAGS := -m elf_i386

HEADERS := $(wildcard *.h ../lib/*.h)

$(BUILD_DIR)/kernel.img: $(BUILD_DIR)/kernel.bin
	@set -eu; \
	size="$$(stat --printf=%s $<)"; \
	[ "$$size" -le 65535 ] || { echo "Too big kernel size: $$size" >&2; exit 1; }; \
	(\
		printf "%b" "$$(printf '\\x%02X\\x%02X' "$$((size % 256))" "$$((size / 256))")"; \
		tail -c +3 $<; \
		[ "$$((size % 512))" -eq 0 ] || dd if=/dev/zero bs="$$((512 - size % 512))" count=1; \
	) > $@

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel
	objcopy -O binary $< $@

$(BUILD_DIR)/kernel: $(patsubst %, $(BUILD_DIR)/%.o, kernel bootstrap gdt libasm_real_mode vga misc)
	ld $(LDFLAGS) --section-start=.text=0x7E00 --omagic -o $@ $^

$(BUILD_DIR)/kernel.o: kernel.S
	gcc $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/bootstrap.o: bootstrap.S
	gcc $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/gdt.o: gdt.c $(HEADERS)
	gcc -m16 $(CFLAGS) -o $@ $<

$(BUILD_DIR)/libasm.o: ../lib/asm.S
	gcc $(ASFLAGS) -D ARCH_BITS=16 -o $@ $<

$(BUILD_DIR)/libasm_real_mode.o: $(BUILD_DIR)/libasm.o
	objcopy --prefix-symbols=real_mode_ $< $@

$(BUILD_DIR)/%.o: %.c $(HEADERS)
	gcc -m32 $(CFLAGS) -o $@ $<

$(BUILD_DIR):
	[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)