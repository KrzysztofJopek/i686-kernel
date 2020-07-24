#include "uart.h"
#include "port.h"
#include "print.h"
#include "fd.h"

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

static void uart_putc(uint8_t c, uint16_t port)
{
	while(!uart_write_ready){}
	outb(c, port);
}
int32_t uart_open(uint8_t* path)
{
	//always return 1 for now
	return 1;
}

int32_t uart_write(int32_t fd, uint8_t* buff, uint32_t count)
{
	//ignore fd for now
	if(buff == (void*)0){
		return -1;
	}
	for(uint32_t i=0; i<count ; i++){
		uart_putc(buff[i], COM1_PORT);
	}
}

int32_t uart_read(int32_t fd, uint8_t* buff, uint32_t count)
{
	//not implemented
	return 0;
}

int32_t uart_close(int32_t fd)
{
	//always return success for now
	return 0;
}

struct fops fops = {
	.open = uart_open,
	.write = uart_write,
	.read = uart_read,
	.close = uart_close
};

struct fops* COM_fops = &fops;
