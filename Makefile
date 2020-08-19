CC = i686-elf-gcc
AS = nasm
LD = i686-elf-gcc
INCFLAGS = -Iinc -Ihalloc

CFLAGS = $(INCFLAGS) -m32 -Werror -Wextra -fno-stack-protector -Wno-builtin-declaration-mismatch -masm=intel \
	 -ffreestanding	 
OUTPUT_OPTION = -MMD -MP -o $@
.DEFAULT_GOAL = kern
GDB=cgdb

SRCS = $(wildcard src/*.c)
OBJS = $(addprefix obj/,$(notdir $(SRCS:%.c=%.o))) obj/head.o obj/swtch.o halloc/halloc.o obj/trap.o obj/exc.o
DEPS = $(OBJS:%.o=%.d)

-include $(DEPS)


#-------

debug: CFLAGS += -ggdb -D_DEBUG_
debug: kern

kern: $(OBJS) link.ld
	$(LD) -Wl,-Tlink.ld -o $@ $(OBJS) -nostdlib -lgcc

obj/head.o: src/head.s
	$(AS) -f elf32 $< -o $@

obj/swtch.o: src/swtch.s
	$(AS) -f elf32 $< -o $@

obj/trap.o: src/trap.s
	$(AS) -f elf32 $< -o $@

obj/exc.o: src/exc.s
	$(AS) -f elf32 $< -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $(OUTPUT_OPTION) $<

halloc/halloc.o: halloc/halloc.c halloc/halloc.h
	$(MAKE) -C halloc halloc.o

bochs: debug
	cp kern bochs/isodir/boot/kern
	cd bochs && grub-mkrescue -o kern.iso isodir/ && bochs


#-------

.PHONY: tags clean clean_tags run gdb

tags:
	ctags -R
	cscope -bR

clean: 
	@rm -f kern $(OBJS) $(DEPS)
	@$(MAKE) -C halloc clean
	@echo Repository cleaned

clean_tags:
	@rm -f tags cscope.out
	@echo Tags cleaned

run: kern
	qemu-system-i386 -s -kernel kern -serial stdio

gdb:
	$(GDB) kern -x .gdbargs
