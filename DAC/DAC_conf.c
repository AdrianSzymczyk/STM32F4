#include "DAC_conf.h"
#include "DMA_conf.h"
#include "timer_conf.h"


const uint32_t SINE_WAVE[32] =
{
    2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056,
    4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447,
    2047, 1647, 1263, 909, 599, 344, 155, 38,
    0, 38, 155, 344, 599, 909, 1263, 1647
};

const uint16_t SINE_SAMPLES = 32;

volatile uint32_t dac_value = 0x00U;

void GPIO_init(void)
{
  // Enable GPIO port A clock
  RCC->AHB1ENR |= (0x1U<<0);

  // Set GPIO PA4 an analog mode
  GPIOA->MODER &= ~(0x3U<<8); // Clear the bits for PA4 port
  GPIOA->MODER |= (0x3U<<8);  // Set up analog mode

  // Set GPIO PA5 an analog mode
  GPIOA->MODER &= ~(0x3U<<10); // Clear the bits for PA4 port
  GPIOA->MODER |= (0x3U<<10);  // Set up analog mode
}

void DAC1_init(void)
{
  // Enable DAC clock
  RCC->APB1ENR |= (0x1U<<29);

  // Enable DAC Channel1
  DAC->CR |= (0x1U<<0);
  
  // Configure DAC channel1
  DAC->CR |= (0x0U<<1);   // Enable DAC channel1 output buffer
  DAC->CR |= (0x1U<<2);   // Enable DAC channel1 trigger
  DAC->CR &= ~(0x7U<<3);  // DAC Channel1 trigger selection (Timer 6 TRGO event)

  DAC->CR |= (0x1U<<12);  // Enable DMA for DAC Channel1

  // Set output to Vref * (dac_value/0xFFF)
  DAC->DHR12R1 = SINE_WAVE[0];
}

void DAC2_init(void)
{
  // Enable DAC clock
  RCC->APB1ENR |= (0x1U<<29);

  //Enable DAC Channel2
  DAC->CR |= (0x1U<<16);

  //Configure DAC Channel2
  DAC->CR &= ~(0x1U<<17); // Enable DAC channel2 output buffer
  DAC->CR |= (0x1U<<18);  // Enable DAC channel2 trigger
  DAC->CR &= ~(0x7U<<19); // Clear channel2 trigger selection bits
  DAC->CR |= (0x3U<<19);  // DAC Channel2 trigger selection (Timer 5 TRGO event)
  //DAC->CR |= (0x0U<<19);  // DAC Channel2 trigger selection (Timer 6 TRGO event)

  DAC->CR |= (0x1U<<28); // Enable DMA for DAC channel2 
  
  // Set 12-bit data for DAC channel2 
  DAC->DHR12R2 = dac_value;
}