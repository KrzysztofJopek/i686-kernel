#include "port.h"
#include "print.h"

#define OFF_DATA 0x0 //[DLAB:0] Data register
#define OFF_DL 0x0 //[DLAB:1] Divisior - low byte
#define OFF_IE 0x1 //[DLAB:0] Interrupt enable
#define OFF_DH 0x1 //[DLAB:1] Divisior - high byte
#define OFF_II 0x2 //Interrupt identification and FIFO control
#define OFF_LC 0x3 //Line control
#define OFF_MC 0x4 //Modem control
#define OFF_LS 0x5 //Line status
#define OFF_MS 0x6 //Modem status
#define OFF_S 0x7 //Scratch

static void set_dlab(uint8_t dlab, uint16_t port)
{
	//DLAB is most significatn bit
	uint8_t val = inb(port+OFF_LC);
	val = (val & 0b0111111) | (dlab << 7);
	outb(val, port+OFF_LC);
}

static void set_divisior(uint16_t div, uint16_t port)
{
	set_dlab(1, port);
	outb(div&0xff, port+OFF_DL);
	outb((div&0xff00)>>8, port+OFF_DH);
	set_dlab(0, port);
}

void setup_uart(uint16_t port)
{
	outb(0x00, port+OFF_IE); //disable interrupts
	set_divisior(3, port);
	outb(0x03, port+OFF_LC); //8bit, no parity, one stop bit
	outb(0xC7, port + OFF_II); //fifo
	outb(0x0B, port + OFF_MC);// IRQ
}


void uart_handler_main()
{
	//UART IRQ not implemented yet
}

static inline int uart_write_ready(uint16_t port)
{
	return !!(inb(port + OFF_LS) & 0x20);
}

void uart_write(uint8_t c, uint16_t port)
{
	while(!uart_write_ready){}
	outb(c, port);
}
