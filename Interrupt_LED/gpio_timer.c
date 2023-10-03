#include "gpio_timer.h"
#include "led_interr.h"


void clock_init(void)
{
  if ((RCC->CR & (0x1U<<1)) == 0){ // Check whether the HSI clock is ready and enable
    RCC->CR |= (0x1U<<0); // Enable HSI clock

    while (!(RCC->CR & (0x1U<<1))); // Wait until HSI clock is ready
  } 

  RCC->CFGR |= 0x00000000U; // Set AHB prescaler to 1

  RCC->CFGR |= 0x00000000U; // Set APB1(low speed) prescaler to 1 

  RCC->CFGR |= 0x00000000U; // Set APB2(high speed) prescaler to 1

  RCC->CFGR &= ~(0x3U<<0); // Clear the SW bits
  RCC->CFGR |= 0x00000000U; // HSI selected as system clock

  // Enable instruciton cache and data cache, set up the LATENCY on the Flash memory 
  FLASH->ACR = (0x1U<<10) | (0x1U<<9) | (0x3U<<0); // 90 < HCLK ≤ 120

  RCC->CR &= ~(0x1U<<16); // Disable HSE clock
}

void timer_init(void)
{
  // Enable TIM2 clock 
  RCC->APB1ENR |= (0x1u<<0);

  // Set the prescaler value for 168 Mhz clock
  // (84 MHz / (83999+1)) = 1 KHz timer clock speed
  TIM2->PSC = 83999;
  // (1 kHz / 1000) = 1Hz = 1s
  TIM2->ARR = (uint32_t)999;

  // Set the prescaler value for 16 Mhz clock
  // (16 MHz / (15999+1)) = 1 KHz timer clock speed
  //TIM2->PSC = 15999;
  // (1 kHz / 1000) = 1Hz = 1s
  //TIM2->ARR = (uint32_t)999;


  // Set the initial value of counter
  TIM2->CNT = 0;

  // Generate an update to update the prescaler and auto-reload values
  TIM2->EGR |= (0x1U<<0);
  // Enable TIM2 the update interrupt 
  TIM2->DIER |= (0x1U<<0);

  //Enable TIM2 interrupt in NVIC
  NVIC_EnableIRQ(TIM2_IRQn);

  // Enable TIM2 counter
  TIM2->CR1 |= (0x1u<<0); 
}


void TIM2_IRQHandler(void)
{
  if (TIM2->SR & (0x1U<<0)){ // Check Update interrupt flag
    toggle(counter);
    counter ++;
    if (counter > 15) counter = 12;
    TIM2->SR &= ~(0x1U<<0); // Reset UIF(Update interrupt flag)
  }
}

void toggle(uint16_t pin)
{
  GPIOD->ODR ^= (1<<pin);
}


void timer_delay(uint32_t ms)
{
  for (uint32_t i=0; i<=ms; i++)
  {
    TIM2->CNT = 0; // Clear the counter 
    
    // Wait UIF to be set
    while ((TIM2->SR & TIM_SR_UIF)); // This generate delay

    TIM2->SR &= ~(0x1U<<0); // Reset UIF(Update interrupt flag)
  }
}

void LSI_frequency_measure(void)
{
  /*
  Use the following procedure to measure the LSI frequency:
  1. Enable the TIM5 timer and configure channel4 in Input capture mode.
  2. Set the TI4_RMP bits in the TIM5_OR register to 0x01 to connect the LSI clock
  internally to TIM5 channel4 input capture for calibration purposes.
  3. Measure the LSI clock frequency using the TIM5 capture/compare 4 event or interrupt.
  4. Use the measured LSI frequency to update the prescaler of the RTC depending on the
  desired time base and/or to compute the IWDG timeout.
  */

}