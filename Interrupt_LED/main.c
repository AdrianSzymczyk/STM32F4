#include <debugio.h>
#include "stm32f4xx.h"
#include "led_interr.h"
#include "gpio_timer.h"
#include "clock_conf.h"

int main(void){
  //Loop Variable Initialisation
  uint32_t i;
  SystemInit();
  set_sysclock_to_168();
  GPIO_Init(); // Initialize GPIO pins
  Interrupt_Config();
  timer_init();
  while(1)
  {
    ;
  }
}

