#include "stm32f4xx.h"
#include "clock_conf.h"

void set_sysclk_to_100(void)
{
  if(RCC->CR & (0x1U<<16)) // Check wheter HSE clock is enabled
  {
    RCC->CR |= (0x1U<<16); // Enable HSE clock
    while(!(RCC->CR & (0x1U<<17))); // Wait until HSE clock is ready
  }
  
  // Set the POWER Enable Clock and Voltage Regulator
  RCC->APB1ENR |= (0x1U<<28); // Power interface clock enable

  // Voltage scaling output 
  PWR->CR |= (0x1U<<14); 
  
  // Configure Flash 
  FLASH->ACR  |= (0x1U<<10) // Data cache enable
              |  (0x1U<<9)  // Instruciton cache enable
              |  (0x1U<<8)  // Prefetch enable
              |  (0x5U<<0); // Latency 5W states (150MHz < HCLK ≤ 168MHz)

  // Configure Prescalars
  RCC->CFGR &= ~(0x7U<<13); // Clear the APB high-speed prescaler bits
  RCC->CFGR |= (0x4U<<13);  // Set the APB2 prescaler /2
  RCC->CFGR &= ~(0x7U<<10); // Clear the APB low-speed prescaler bits
  RCC->CFGR |= (0x5U<<10);  // Set the APB1 prescaler /4
  RCC->CFGR &= ~(0x15U<<4); // Clear the AHB prescaler bits
  RCC->CFGR |= 0x00000000U; // Set the AHB prescaler /1

  /* Configure Main PLL(division factors M, N, P, Q)
  PLL_freq = (((HSE / M) * N) / P) */
  RCC->PLLCFGR |= (PLLM<<0)   // Set M factor 
             |  (PLLN<<6)   // Set N factor
             |  (PLLP<<16)  // Set P factor
             |  (0x1U<<22); // Set HSE as PLL source

  // Enable main PLL clock
  RCC->CR |= (0x1U<<24);         // Enable PLL clock
  while((RCC->CR) & (0x1U<<25)); // Wait until PLL clock is enabled

  // Select Clock Source
  RCC->CFGR |= (0x2U<<0);            // Set PLL as system clock
  while (!((RCC->CFGR) |= (0x2U<<2))); // Switch clock status as PLL
}