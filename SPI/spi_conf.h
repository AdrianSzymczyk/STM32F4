#ifndef __SPI_CONF_H
#define __SPI_CONF_H

#include "stm32f4xx.h"

void SPI_init(void);
void SPI_enable(void);
void SPI_disable(void);
void CS_enable(void);
void CS_disable(void);
uint8_t SPI_transmit(uint8_t);

#endif