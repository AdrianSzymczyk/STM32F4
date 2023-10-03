#ifndef DAC_CONF_H
#define DAC_CONF_H

#include "stm32f4xx.h"

extern const uint32_t SINE_WAVE[32];
extern const uint16_t SINE_SAMPLES;
extern volatile uint32_t dac_value;

void GPIO_init(void);
void DAC1_init(void);
void DAC2_init(void);

#endif