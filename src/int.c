#include "int.h"
#include "kerndefs.h"
#include "port.h"
#include "print.h"
#include "kbd.h"
#include "uart.h"

#define IDT_SIZE 256
#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1


extern void load_idt(void* idt_ptr);
extern void keyboard_handler(void);
extern void uart_handler(void);

struct IDT_entry{
	uint16_t offset_low;
	uint16_t selector;
	uint8_t zero;
	uint8_t type;
	uint16_t offset_high;
};

static struct IDT_entry IDT[IDT_SIZE];

static void setup_pic()
{
	//ICW1
	//configure pic
	outb(0x11, PIC1_CMD);
	outb(0x11, PIC2_CMD);
	//ICW2
	//offset
	outb(0x20 , PIC1_DATA);
	outb(0x28 , PIC2_DATA);
	//ICW3
	//cascading
	outb(0x00 , PIC1_DATA);
	outb(0x00 , PIC2_DATA);
	//ICW4
	//flags
	outb(0x01 , PIC1_DATA);
	outb(0x01 , PIC2_DATA);
	//mask all
	outb(0xff , PIC1_DATA);
	outb(0xff , PIC2_DATA);
}

static void setup_entry(void* addr, uint8_t pos)
{
	uint32_t u_addr = (uint32_t)addr; 
	IDT[pos].offset_low = u_addr & 0xffff;
	IDT[pos].selector = 0x08;
	IDT[pos].zero = 0;
	IDT[pos].type = 0x8e;
	IDT[pos].offset_high = (u_addr & 0xffff0000) >> 16;
}

void setup_int()
{

	setup_entry(keyboard_handler, 0x21);
	setup_entry(uart_handler, 0x24);
	setup_uart(COM1_PORT);
	setup_pic();
	struct{
		uint16_t size;
		struct IDT_entry* ptr;
	} __attribute__((packed)) idt_ptr = {sizeof(struct IDT_entry)*IDT_SIZE-1, IDT};

	asm volatile (
			"lidt [%0];"
			"sti;"
			:
			: "q" (&idt_ptr)
		     );

	//unmask
	outb(0b11101101, PIC1_DATA);
}

//TODO define ports, handle error
void keyboard_handler_main()
{
	outb(PIC1_CMD, PIC1_CMD);
	uint8_t status = inb(0x64); //keyboard status port 
	if(status * 0x01){
		uint8_t c = inb(0x60);
		putc(kbdus[c]);
	}
	else{
		//error
	}
}
