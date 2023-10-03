#include "stm32f4xx.h"

void SPI_init(void)
{
  /************** STEPS TO FOLLOW *****************
  1. Enable SPI clock
  2. Configure the Control Register 1
  3. Configure the CR2
  ************************************************/

  // Enable SPI1 clock
  RCC->APB2ENR |= (0x1U<<12); 

  // Configure the control register 1
  SPI1->CR1 &= ~(0x1U<<0) & ~(0x1U<<1); // CPHA=0, CPOL=0
  SPI1->CR1 |= (0x1U<<2);               // Enable master mode
  SPI1->CR1 &= ~(0x7U<<3);              // BR[2:0] = 000: fPCLK/2, PCLK2 = 50MHz, SPI CLK = 25MHz
  SPI1->CR1 &= ~(0x1U<<7);              // MSB first
  SPI1->CR1 |= (0x1U<<8) | (0x1U<<9);   // Software Slave Management
  SPI1->CR1 &= ~(0x1U<<10);             // Full duplex mode
  SPI1->CR1 &= ~(0x1U<<11);             // 8-bit data format

  // Reset the entire CR2 register
  SPI1->CR2 = 0;               
}

void SPI_enable(void)
{
  SPI1->CR1 |= (0x1U<<6);  // Peripheral enabled
}

void SPI_disable(void)
{
  SPI1->CR1 &= ~(0x1U<<6); // Peripheral disabled
}

void CS_enable(void)
{
  GPIOA->BSRR |= (0x1U<<4+16); // Reset the SPI1 CS bit
}

void CS_disable(void)
{
  GPIOA->BSRR |= (0x1U<<4); // Set the corresponding SPI1 CS bit
}

uint8_t SPI_transmit(uint8_t data)
{
  /************** STEPS TO FOLLOW *****************
  1. Wait for the TXE bit to set in the Status Register
  2. Write the data to the Data Register
  3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
  4. Clear the Overrun flag by reading DR and SR
  ************************************************/	

  // Write data in SPI data register
  while(!((SPI1->SR) & (0x1U<<1))); // Wait for TXE bit to set
  SPI1->DR = data;                  // Load the data into the Data Register
  
  while(!((SPI1->SR) & (0x1U<<1))); // Wait for TXE bit to set -> indicate that the buffer is empty 
  while(((SPI1->SR) & (0x1U<<7)));  // Wait for BSY bit to reset -> indicate that SPI is not busy in communication

  // Clear the overrun flag by reading DR and SR
  uint8_t tmp = SPI1->DR;
  tmp = SPI1->SR;

  return SPI1->DR;
}