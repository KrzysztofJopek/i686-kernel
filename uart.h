#ifndef _UART_H_
#define _UART_H_

#define COM1_PORT 0x3F8
#include <stdint.h>

int setup_uart(uint16_t port);
void uart_write(uint8_t c, uint16_t port);

#endif
