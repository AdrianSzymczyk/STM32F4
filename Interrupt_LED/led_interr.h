#ifndef LEDINTERR_H
#define LEDINTERR_H
#include "stm32f4xx.h"

extern uint16_t counter;

void GPIO_Init(void);
void toggle_Led(uint32_t pin, uint32_t d);
void delay(int iterations);
void Interrupt_Config(void);
void EXTI0_IRQHandler(void);

#endif