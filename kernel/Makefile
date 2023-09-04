SHELL := /bin/bash
BUILD_DIR := ./build

.PHONY: clean
.EXTRA_PREREQS := Makefile | $(BUILD_DIR)

ASFLAGS := -c -g -Wall -Wextra -Werror
CFLAGS := $(ASFLAGS) -march=i386 -Os -nostdlib -fno-builtin -ffreestanding -fno-stack-protector -fno-pie -I ../lib
LDFLAGS := -m elf_i386

MODULES := . keyboard
HEADERS := $(foreach dir,$(MODULES) ../lib,$(wildcard $(dir)/*.h))

$(BUILD_DIR)/kernel.img: $(BUILD_DIR)/kernel.bin
	@set -eu; \
	size="$$(stat --printf=%s $<)"; \
	[ "$$size" -le $$((65535 - 0x7C00 - 512)) ] || { echo "Too big kernel size: $$size" >&2; exit 1; }; \
	(\
		printf "%b" "$$(printf '\\x%02X\\x%02X' "$$((size % 256))" "$$((size / 256))")"; \
		tail -c +3 $<; \
		[ "$$((size % 512))" -eq 0 ] || dd if=/dev/zero bs="$$((512 - size % 512))" count=1; \
	) > $@

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel
	objcopy -O binary --set-section-flags .bss=alloc,load,contents $< $@

REAL_MODE_OBJECTS := kernel16_asm libasm_real_mode bootstrap16_asm multiboot16 gdt16 idt16_asm
PROTECTED_MODE_OBJECTS := interrupts kmain misc multiboot pic pit pm ps2 textio vga vm \
                          keyboard_keys

# Requires gcc-multilib package
$(BUILD_DIR)/kernel: $(patsubst %, $(BUILD_DIR)/%.o, $(REAL_MODE_OBJECTS) $(PROTECTED_MODE_OBJECTS))
	ld $(LDFLAGS) --section-start=.text=0x7E00 --omagic -o $@ $^ $$(gcc -m32 $(CFLAGS) -print-libgcc-file-name)

idt.S: ./generate-idt
	$<

keyboard/keys.c: keyboard/keys.h
keyboard/keys.h: keyboard/generate-keys
	$<

$(BUILD_DIR)/libasm_real_mode.o: $(BUILD_DIR)/libasm16.o
	objcopy --prefix-symbols=real_mode_ $< $@

$(BUILD_DIR)/libasm16.o: ../lib/asm.S
	gcc -m32 $(ASFLAGS) -D ARCH_BITS=16 -o $@ $<

$(BUILD_DIR)/%_asm.o: %.S
	gcc -m32 $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/%16.o: %16.c $(HEADERS)
	gcc -m16 -mgeneral-regs-only $(CFLAGS) -o $@ $<

define c_object_target
$$(BUILD_DIR)/$(patsubst ._,,$1_)%.o: $1/%.c $$(HEADERS)
	gcc -m32 $$(CFLAGS) -o $$@ $$<
endef
$(foreach module,$(MODULES),$(eval $(call c_object_target,$(module))))

$(BUILD_DIR):
	[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)