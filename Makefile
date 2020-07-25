CC = gcc
AS = nasm
LD = ld
CFLAGS = -Iinc -m32 -Werror -Wextra -fno-stack-protector -Wno-builtin-declaration-mismatch
OUTPUT_OPTION = -MMD -MP -o $@
.DEFAULT_GOAL = kern
GDB=cgdb

SRCS = $(wildcard src/*.c)
OBJS = $(addprefix obj/,$(notdir $(SRCS:%.c=%.o))) obj/head.o
DEPS = $(OBJS:%.o=%.d)
-include $(DEPS)


#-------

debug: CFLAGS += -ggdb -D_DEBUG_
debug: kern

kern: $(OBJS) link.ld
	$(LD) -m elf_i386 -T link.ld -o $@ $(OBJS)

obj/head.o: src/head.s
	$(AS) -f elf32 $< -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $(OUTPUT_OPTION) $<
#-------

.PHONY: tags clean clean_tags run gdb

tags:
	ctags -R
	cscope -bR

clean: 
	@rm -f kern $(OBJS) $(DEPS)
	@echo Repository cleaned

clean_tags:
	@rm -f tags cscope.out
	@echo Tags cleaned

run: kern
	qemu-system-i386 -s -kernel kern -serial stdio

gdb:
	$(GDB) kern -x .gdbargs
