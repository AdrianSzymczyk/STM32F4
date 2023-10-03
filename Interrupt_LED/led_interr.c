#include "led_interr.h"

uint16_t counter = 12;

void GPIO_Init(void){
  // Configure LED's 
  RCC->AHB1ENR |= (1<<3); // Enable GPIOD clock
  GPIOD->MODER |= (0x1<<24) | (0x1<<26) | (0x1<<28) |(0x1<<30); // Set LEDs (PD12, PD13, PD14, PD15) as outputs


  // Configure User button
  RCC->AHB1ENR |= (1<<0); // Enable the clock of port A of the GPIO
  // Configure PA0 as an input
  GPIOA->MODER &= ~(3<<0); // Bits 1:0 = 0:0 -> PA0 as input. User button, set pin 0 as input. 
  GPIOA->PUPDR |= (1<<1); // Bits 1:0 = 1:0 -> PA0 in Pull-down Mode. [pull-up didn't want to work properly]
}

void delay(int iterations){
  for(uint32_t i=0; i<iterations; i++){};
}

void Interrupt_Config(void) {
    RCC->APB2ENR |= (1<<14);// Enable SYSCFG clock
    
    SYSCFG->EXTICR[0] &= ~(0xF<<0); // Connect EXTI0 line to PA0 pin

    EXTI->IMR |= (1<<0); // Disable Mask on EXTI 0
    
    EXTI->FTSR |= (1<<0); // Enable Falling edge Trigger for PA0
    EXTI->RTSR &= ~(1<<0); // Disable Rising edge Trigger for PA0

    NVIC_SetPriority(EXTI0_IRQn, 1); //Set the Interrupt Priority
    NVIC_EnableIRQ(EXTI0_IRQn); // Enable EXTI0 IRQ in NVIC
}

void EXTI0_IRQHandler(void) {
    // Clear the EXTI line 0 pending bit
    EXTI->PR |= EXTI_PR_PR0;
    
    // Toggle LED lights one by one
    if (counter <=15){
      toggle_Led(counter, 600000);
      counter ++;
      if (counter>15) counter = 12;
    }
}

void toggle_Led(uint32_t pin, uint32_t d)
{
  if((GPIOD->ODR & 1<<pin) == 0)
  {
    GPIOD->BSRR = (1<<pin);
  }else{
    GPIOD->BSRR = (1<<pin+16);
  }
  delay(d);
}
