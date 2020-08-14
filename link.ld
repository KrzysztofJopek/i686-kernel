/*OUTPUT_FORMAT(elf32-i386)*/
ENTRY(_start)
SECTIONS
 {
   . = 0x100000; /*1M*/
   .multiboot.data :
   {
      *(.multiboot.data)
   }
   .multiboot.text :
   {
      *(.multiboot.text)
   }

   . = 0xC0101000; /*3G + 1M*/
   .text ALIGN(4K) : AT(ADDR(.text)-0xC0000000)
   {
      *(.text)
   }
   .rodata ALIGN(4K) : AT(ADDR(.rodata)-0xC0000000)
   {
      *(.rodata)
   }
   .data ALIGN(4K) : AT(ADDR(.data)-0xC0000000)
   {
      *(.data)
   }
   .bss ALIGN(8K) : AT(ADDR(.bss)-0xC0000000)
   {
      *(COMMON)
      *(.bss)
   }
   PROVIDE(end_mem = .); /*End of memory*/
 }
