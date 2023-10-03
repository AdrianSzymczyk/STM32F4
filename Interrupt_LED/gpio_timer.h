#include "stm32f4xx.h"

void clock_init(void);
void LSI_frequency_measure(void);
void timer_init(void);

void toggle(uint16_t);
void TIM2_IRQHandler(void);
void timer_delay();
