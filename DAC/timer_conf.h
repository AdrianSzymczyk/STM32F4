#ifndef TIMER_CONF_H
#define TIMER_CONF_H

#include "stm32f4xx.h"
#include <stdio.h>

uint16_t i;
volatile uint32_t set_value;

void delay(uint32_t);

void Timer6_init(void);
void Timer5_init(void);

#endif