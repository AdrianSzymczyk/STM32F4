#include "DMA_conf.h"
#include "stm32f4xx.h"
#include "timer_conf.h"
#include "DAC_conf.h"

void DMA1_init(void)
{
  // Enable DMA1 clock
  RCC->AHB1ENR |= (0x1U<<21);

  // DMA configuration (DMA1, channel 7, stream 5).
  DMA1_Stream5->CR = 0x00000000U;       // Disable DMA stream 5 for registers configuration 
  while(DMA1_Stream5->CR & (0x1U<<0));  // Wait until DMA stream 5 is disabled

  DMA1_Stream5->CR |= (0x7U<<25) |      // Channel selection (channel 7 selected)
                      (0x1U<<16) |      // Priority level (medium)
                      //(0x1U<<10) |    // Memory increment mode 
                      (0x1U<<13) |      // Memory data size (half-word 16-bit)
                      (0x1U<<11) |      // Peripheral data size (half-word 16-bit)
                      //(0x1U<<8)  |    // Circular mode enabled
                      (0x1U<<6);        // Data transfer direction (Memory-to-peripheral)

  // Configuration for sinus wave 
  DMA1_Stream5->PAR = (uint32_t)&(DAC->DHR12R1); // Destination address (DAC data register)
  DMA1_Stream5->M0AR = (uint32_t)&SINE_WAVE;     // Source address of the sine wave in memory
  DMA1_Stream5->NDTR = (uint16_t)SINE_SAMPLES;   // Set DMA data transfer length

  // Enable DMA1 and start the DMA transfer
  DMA1_Stream5->CR |= (0x1U<<0);
}

void DMA2_init(void)
{
  // Enable DMA1 clock
  RCC->AHB1ENR |= (0x1U<<21);

  // DMA configuration (DMA1, channel 7, stream 5).
  DMA1_Stream5->CR = 0x00000000U;       // Disable DMA stream 5 for registers configuration 
  while(DMA1_Stream5->CR & (0x1U<<0));  // Wait until DMA stream 5 is disabled

  DMA1_Stream5->CR |= (0x7U<<25) |      // Channel selection (channel 7 selected)
                      (0x1U<<16) |      // Priority level (medium)
                      //(0x1U<<10) |    // Memory increment mode 
                      (0x1U<<13) |      // Memory data size (half-word 16-bit)
                      (0x1U<<11) |      // Peripheral data size (half-word 16-bit)
                      //(0x1U<<8)  |    // Circular mode enabled
                      (0x1U<<6);        // Data transfer direction (Memory-to-peripheral)

  // Configuration for sawtooth wave
  DMA1_Stream5->PAR = (uint32_t)&(DAC->DHR12R2); // Destination address (DAC data register)
  DMA1_Stream5->M0AR = (uint32_t)&dac_value;     // Source address of the value in memory
  DMA1_Stream5->NDTR = (uint16_t)4096;           // Set DMA data transfer length

  // Enable DMA1 and start the DMA transfer
  DMA1_Stream5->CR |= (0x1U<<0);
}