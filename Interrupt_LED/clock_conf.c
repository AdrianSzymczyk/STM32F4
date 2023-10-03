#include "clock_conf.h"



void set_sysclock_to_168(void)
{
  /*************>>>>>>> STEPS FOLLOWED <<<<<<<<************
  1. ENABLE HSE and wait for the HSE to become Ready
  2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
  3. Configure the FLASH PREFETCH and the LATENCY Related Settings
  4. Configure the PRESCALARS HCLK, PCLK1, PCLK2
  5. Configure the MAIN PLL
  6. Enable the PLL and wait for it to become ready
  7. Select the Clock Source and wait for it to be set
  ********************************************************/


  if(!(RCC->CR & (0x1U<<16))){ // Check whether the HSE clock is enabled 
    RCC->CR |= (0x1U<<16);       // Enable HSE clock
    while(!(RCC->CR & (0x1U<<17))); // Wait until HSE clock will be ready
  }

  // Set the POWER Enable Clock and Voltage Regulator
  RCC->APB1ENR  |= (0x1U<<28); // Power interface clock enable

  /* set voltage scale to 1 for max frequency (PWR_CR:bit 14)
  * (0b0) scale 2 for fCLK <= 144 MHz
  * (0b1) scale 1 for 144 MHz < fCLK <= 168 MHz
  */
  PWR->CR       |= (0x1U<<14); // Voltage scaling output 
  
  // Configure Flash 
  FLASH->ACR  |= (0x1U<<10) // Data cache enable
              |  (0x1U<<9)  // Instruciton cache enable
              |  (0x1U<<8)  // Prefetch enable
              |  (0x5U<<0); // Latency 5W states  
  
  // Configure Prescalars
  RCC->CFGR &= ~(0x7U<<13); // Clear the APB high-speed prescaler bits
  RCC->CFGR |= (0x4U<<13);  // Set the APB2 prescaler /2
  RCC->CFGR &= ~(0x7U<<10); // Clear the APB low-speed prescaler bits
  RCC->CFGR |= (0x5U<<10);  // Set the APB1 prescaler /4
  RCC->CFGR &= ~(0x15U<<4); // Clear the AHB prescaler bits
  RCC->CFGR |= 0x00000000U; // Set the AHB prescaler /1

  // Configure Main PLL(division factors M, N, P, Q)
  RCC->PLLCFGR |= (PLLM<<0)   // Set M factor 
               |  (PLLN<<6)   // Set N factor
               |  (PLLP<<16)  // Set P factor
               |  (0x1U<<22); // Set HSE as PLL source

  // Enable PLL clock
  RCC->CR |= (0x1U<<24);           // Enable PLL clock
  while (!(RCC->CR & (0x1U<<25))); // Wait until PLL clock will be ready


  // Select Clock Source
  RCC->CFGR |= (0x2U<<0);            // Set PLL as system clock
  while (!(RCC->CFGR |= (0x2U<<2))); // Switch clock status as PLL
}