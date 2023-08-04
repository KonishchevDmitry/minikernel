.PHONY: clean

SHELL := /bin/bash
BUILD_DIR := ./build

ASFLAGS := -c -g -m32
LDFLAGS := -m elf_i386

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

$(BUILD_DIR)/kernel: $(BUILD_DIR)/kernel.o
	ld $(LDFLAGS) --section-start=.text=0x7E00 -o $@ $^

$(BUILD_DIR)/kernel.o: kernel.S ../libasm.S | $(BUILD_DIR)
	gcc $(ASFLAGS) -o $@ $<

$(BUILD_DIR):
	[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)