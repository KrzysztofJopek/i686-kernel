CC = gcc
AS = nasm
LD = ld
CFLAGS = -m32 -Werror -Wextra -fstack-protector -Wno-builtin-declaration-mismatch
OUTPUT_OPTION = -MMD -MP -o $@
.DEFAULT_GOAL = kern

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o) head.o
DEPS = $(SRCS:%.c=%.d)
-include $(DEPS)

#-------

debug: CFLAGS += -ggdb -D_DEBUG_
debug: kern

kern: $(OBJS) link.ld
	$(LD) -m elf_i386 -T link.ld -o $@ $(OBJS)

head.o: head.s
	$(AS) -f elf32 $< -o $@
#-------

.PHONY: tags clean clean_tags run

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
