#ifndef __UART_CONF_H
#define __UART_CONF_H

#include "stm32f4xx.h"

void UART_init(void);
void UART_putc(const unsigned char);
void UART_puts(const char*);
unsigned char UART_getc(void);

#endif