#include "stm32f4xx.h"
#include "timer_conf.h"
#include "DAC_conf.h"

uint16_t i = 0;
uint32_t set_value = 0;


void delay(uint32_t iterations){
  for(uint32_t i=0; i<iterations; i++){};
}

void Timer6_init(void)
{
  // Enable TIM6 Clock
  RCC->APB1ENR |= (0x1U<<4); 

  // Set the prescaler values
  // Timer delay 0,01ms
  TIM6->PSC = 15999; // 1Mhz
  TIM6->ARR = 20;    // 100kHz
  
  // Set trigger output on update event 
  TIM6->CR2 |= (0x2U<<4); 
  
  // Set the initial value of counter
  TIM6->CNT = 0; 
  
  // Configure DMA/Interrupt register
  TIM6->DIER |= (0x1U<<0)  // Enable update interrupt
             | (0x1U<<8);  // Enable DMA update

  // Configure TIM6 interrupt in NVIC
  NVIC_SetPriority(TIM6_DAC_IRQn, 1); // Set Priority level to 1
  NVIC_EnableIRQ(TIM6_DAC_IRQn);      // Enable TIM6 interrupt in NVIC

  // Enable TIM6 counter
  TIM6->CR1 |= (0x1U<<0); 
}

void TIM6_DAC_IRQHandler(void)
{
  // clear interrupt status
  if (TIM6->DIER & 0x01) {      // Check whether the interrupts are enabled
    if (TIM6->SR & 0x01) {      // Check whether the interrupt flag is interrupt pending
        TIM6->SR &= ~(1U << 0); // Clear the update flag
    }
  }

  // Update values for sine wave 
  set_value = SINE_WAVE[i];
  DAC->DHR12R1 = set_value;
  i++;
  if (i>32){
    i=0;
    set_value = SINE_WAVE[0];
  }
}


void Timer5_init(void)
{
  // Enable TIM5 clock
  RCC->APB1ENR |= (0x1U<<3);
  
  // Set the prescaler values
  // Timer delay 1ms
  TIM5->PSC = 15999; // 1Mhz
  TIM5->ARR = 1;  // 1kHz

  // Set trigger output on update event 
  TIM5->CR2 |= (0x2U<<4); 

  // Set the initial counter value 
  TIM5->CNT = 0;

  // Configure DMA/Interrupt register
  TIM5->DIER |= (0x1U<<0)  // Enable update interrupt
             |  (0x1U<<8); // Enable DMA request

  // Configure TIM7 interrupt in NVIC
  NVIC_SetPriority(TIM5_IRQn, 1); // Set priority level to 1
  NVIC_EnableIRQ(TIM5_IRQn);      // Enable TIM5 interrupt in NVIC

  // Enable counter
  TIM5->CR1 |= (0x1U<<0);
}

void TIM5_IRQHandler(void)
{
  // Clear interrupt status
  if (TIM5->DIER & 0x01){     // Check whether the interrupts are enabled
    if (TIM5->SR & 0x01){     // Check whether the interrupt flag is interrupt pending
      TIM5->SR &= ~(0x1U<<0); // Clear the update flag
    }
  }

  // Update the new value, let timer trgo handle dac
  DAC->DHR12R2 = dac_value;
  dac_value ++;
  if (dac_value>4095) dac_value=0;
}