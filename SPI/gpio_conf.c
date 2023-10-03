#include "gpio_conf.h"
#include "stm32f4xx.h"


void gpio_init(void)
{
  // Enable GPIOA clock
  RCC->AHB1ENR |= (0x1U<<0);

  // Configure pins mode
  GPIOA->MODER |= (0x1U<<8)  |  // Output mode for PA4 (CS)
                  (0x2U<<10) |  // Alternate mode for PA5 (SCK)
                  (0x2U<<12) |  // Alternate mode for PA6 (MISO)
                  (0x2U<<14);   // Alternate mode for PA7 (MOSI)
  
  // Configure pins speed
  GPIOA->OSPEEDR |= (0x3U<<8)  | // Very high speed for PA4
                    (0x3U<<10) | // Very high speed for PA5
                    (0x3U<<12) | // Very high speed for PA6
                    (0x3U<<14);  // Very high speed for PA7

  // Configure alternate functions
  GPIOA->AFR[0] |= (0x5U<<20) | // Set AF5 dedicated function for PA5
                   (0x5U<<24) | // Set AF5 dedicated function for PA6
                   (0x5U<<28);  // Set AF5 dedicated function for PA7
}