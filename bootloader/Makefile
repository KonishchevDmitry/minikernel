BUILD_DIR := ./build

.PHONY: clean
.EXTRA_PREREQS := Makefile | $(BUILD_DIR)

ASFLAGS := -c -g -m32
LDFLAGS := -m elf_i386

$(BUILD_DIR)/bootloader.bin: $(BUILD_DIR)/bootloader
	objcopy -O binary $< $@

$(BUILD_DIR)/bootloader: $(BUILD_DIR)/bootloader.o
	ld $(LDFLAGS) --section-start=.text=0x7C00 -o $@ $^

$(BUILD_DIR)/bootloader.o: bootloader.S ../lib/asm.S
	gcc $(ASFLAGS) -o $@ $<

$(BUILD_DIR):
	[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)