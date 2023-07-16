.PHONY: all clean

AS_FLAGS := --32
ifdef DEBUG
AS_FLAGS := $(AS_FLAGS) -gstabs
endif

LD_FLAGS := -m elf_i386

all: linux-app linux-stdlib-app
	./linux-app
	./linux-stdlib-app

$(wildcard *.o): Makefile

linux-app: linux_app.o linux_app_base.o libasm.o
	ld $(LD_FLAGS) -o $@ $^

linux_app.o: linux_app.s
	as $(AS_FLAGS) -o $@ $<

# Requires gcc-multilib package
linux-stdlib-app: linux_stdlib_app.o linux_app_base.o libasm.o
	ld $(LD_FLAGS) -dynamic-linker /lib/ld-linux.so.2 -o $@ $^ -lc

linux_stdlib_app.o: linux_stdlib_app.s
	as $(AS_FLAGS) -o $@ $<

linux_app_base.o: linux_app_base.s
	as $(AS_FLAGS) -o $@ $<

libasm.o: libasm.s
	as $(AS_FLAGS) -o $@ $<

clean:
	rm -f *.o linux-app linix-stdlib-app