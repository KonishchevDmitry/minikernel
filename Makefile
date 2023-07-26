.PHONY: all clean

ASFLAGS := -c -m32
ifdef DEBUG
ASFLAGS := $(ASFLAGS) -g
endif

LDFLAGS := -m elf_i386

all: linux-app linux-stdlib-app
	./linux-app --arg value
	./linux-stdlib-app --arg value

$(wildcard *.o): Makefile

linux-app: linux_app.o linux_app_base.o libasm.o
	ld $(LDFLAGS) -o $@ $^

linux_app.o: linux_app.S
	gcc $(ASFLAGS) -o $@ $<

# Requires gcc-multilib package
linux-stdlib-app: linux_stdlib_app.o linux_app_base.o libasm.o
	ld $(LDFLAGS) -dynamic-linker /lib/ld-linux.so.2 -o $@ $^ -lc

linux_stdlib_app.o: linux_stdlib_app.s
	gcc $(ASFLAGS) -o $@ $<

linux_app_base.o: linux_app_base.s
	gcc $(ASFLAGS) -o $@ $<

libasm.o: libasm.s
	gcc $(ASFLAGS) -o $@ $<

clean:
	rm -f *.o linux-app linux-stdlib-app