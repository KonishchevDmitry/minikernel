BUILD_DIR := ./build

.PHONY: clean test
.EXTRA_PREREQS := Makefile | $(BUILD_DIR)

ASFLAGS := -c -g -m32
LDFLAGS := -m elf_i386

test: $(BUILD_DIR)/app $(BUILD_DIR)/stdlib-app
	$(BUILD_DIR)/app --arg value
	$(BUILD_DIR)/stdlib-app --arg value

$(BUILD_DIR)/app: $(patsubst %, $(BUILD_DIR)/%.o, app app_base libasm)
	ld $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/app.o: app.S
	gcc $(ASFLAGS) -o $@ $<

# Requires gcc-multilib package
$(BUILD_DIR)/stdlib-app: $(patsubst %, $(BUILD_DIR)/%.o, stdlib_app app_base libasm)
	ld $(LDFLAGS) -dynamic-linker /lib/ld-linux.so.2 -o $@ $^ -lc

$(BUILD_DIR)/stdlib_app.o: stdlib_app.s
	gcc $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/app_base.o: app_base.s
	gcc $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/libasm.o: ../lib/asm.S
	gcc $(ASFLAGS) -D ARCH_BITS=32 -o $@ $<

$(BUILD_DIR):
	[ -d $(BUILD_DIR) ] || mkdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)