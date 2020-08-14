#ifndef _VM_H_
#define _VM_H_

#define KERN_ADDR 0xC0000000
#define KERN_HEAP_ADDR (KERN_ADDR + 1024*4096)
#define KERN_HEAP_PAGES 1024
#define KERN_HEAP_SIZE (KERN_HEAP_PAGES * 4096)

void setup_vm();

#endif
