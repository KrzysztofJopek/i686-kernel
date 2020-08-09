/*OUTPUT_FORMAT(elf32-i386)*/
ENTRY(_start)
SECTIONS
 {
   . = 1M;
   .rodata BLOCK(4k) : ALIGN(4k)
   {
      *(.multiboot)
   }
   .text BLOCK(4k) : ALIGN(4k)
   {
      *(.text)
   }
   .rodata BLOCK(4k) : ALIGN(4k)
   {
      *(.rodata)
   }
   .data BLOCK(4k) : ALIGN(4k)
   {
      *(.data)
   }
   .bss BLOCK(8k) : ALIGN(4k)
   {
      *(COMMON)
      *(.bss)
   }
   PROVIDE(end_mem = .); /*End of memory*/
 }
