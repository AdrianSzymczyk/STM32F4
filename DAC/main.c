#include <debugio.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "timer_conf.h"
#include "DAC_conf.h"
#include "DMA_conf.h"
#include "Clock_conf.h"
  
int main(void)
{ 
  SystemInit();
  //set_sysclock_to_84();
  GPIO_init();
  DMA1_init();
  DAC1_init();
  Timer6_init();

  DMA2_init();
  DAC2_init();
  Timer5_init();
  while(1);
  return 0;
}

