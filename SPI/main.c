#include <debugio.h>
#include "stm32f4xx.h"
#include "clock_conf.h"
#include "gpio_conf.h"
#include "spi_conf.h"
#include "sd_card.h"

const uint8_t data[3] = {0x02, 0x32, 0xf6};

void SPI_test(uint8_t *data);
uint8_t SD_single_read_test(uint32_t addr, uint8_t *buff, uint8_t token);
void SD_error_test(uint32_t addr, uint8_t *buff, uint8_t token, uint8_t *res);
void SD_single_write_test(uint32_t addr, uint8_t *buff, uint8_t token, uint8_t *res);
uint8_t SD_multiple_read_test(uint32_t addr, uint8_t *buff, uint8_t token, uint32_t num_blocks);
void SD_multiple_write_test(uint32_t addr, uint8_t *buff, uint8_t token, uint32_t num_blocks);


void main(void)
{
  // Create an array for responses
  uint8_t res[5], sdBuf[512], token, sd_multi_buf[10][512];

  SystemInit();
  // Set system clock to 100MHz
  set_sysclk_to_100();
  // Initialize essential GPIOs
  gpio_init();
  
  // Set up SPI
  SPI_init();       // Initializte SPI
  SPI_enable();     // Enable SPI protocol      
  //SPI_test(data);   // SPI test

  // SPI Mode Initialization Flow for SD memory card
  SD_init();

  // SD card read sector 0 test
  //res[0] = SD_read_test(0x00000000, sdBuf, token);
  
  // SD card read error
  //SD_error_test(0xFFFFFFFF, sdBuf, &token, res);
  
  // SD card write test
  //for(uint16_t i=0; i<512; i++) sdBuf[i] = 0x55;              // Fill buffer with 0x55
  //res[0] = SD_write_single_block(0x00000100, sdBuf, &token);  // Write 0x55 to address 0x100 (256)
  SD_single_write_test(0x00000100, sdBuf, token, res);
  for(uint8_t i=0; i<5; i++) debug_printf("Response %d = 0x%08X\n", i, res[i]);
  res[0] = SD_single_read_test(0x00000100, sdBuf, token);  // Read sector 100

  // SD card read multiple blocks sectore 0
  //res[0] = SD_read_multiple_block(0x00000000, (uint8_t *)sd_multi_buf, &token, 10);
  //if(token == 0xFE)
  //{
  //  for(uint8_t i=0; i<10; i++)
  //  {
  //    debug_printf("\nNew [%d] block of code: ", i+1);
  //    for(uint16_t j=0; j<512; j++)
  //      {
  //        debug_printf("%d, ", sd_multi_buf[i][j]);
  //      }
  //  }
  //}
  //else debug_printf("Error reading sector\n!");

  // SD card write multiple blocks 
  // Fill the buffer with example data
  //for(uint8_t i=0; i<10; i++)
  //{
  //  uint16_t value = 0x55 + i;
  //  for(uint16_t j=0; j<512; j++)
  //  {
  //    sd_multi_buf[i][j] = value;
  //  }
  //}
  //for(uint8_t i=0; i<10; i++) 
  //{
  //  debug_printf("\nValue for %d row: %d, %d, %d", i, sd_multi_buf[i][200], sd_multi_buf[i][300], sd_multi_buf[i][400]);
  //}
  //res[1] = SD_write_multiple_block(0x00000200, (uint8_t *)sd_multi_buf, &token, 10);
  while(1);
}


void SPI_test(uint8_t *data)
{
  CS_enable();
  SPI_transmit(data[0]);
  SPI_transmit(data[1]);
  SPI_transmit(data[2]);
  CS_disable();
}

uint8_t SD_single_read_test(uint32_t addr, uint8_t *buff, uint8_t token)
{
  uint8_t res;
  // Read sector addr
  res = SD_read_single_block(addr, buff, &token);
  if(token == 0xFE)
  {
    debug_printf("\nData: ");
    for(uint16_t i=0; i<512; i++)
    {
      debug_printf("%d, ", buff[i]);
    }
    debug_printf("\nEnd of the data");
  }
  else debug_printf("Error reading sector\n!");

  return res;
}

void SD_error_test(uint32_t addr, uint8_t *buff, uint8_t token, uint8_t *res)
{
  res[0] = SD_read_single_block(addr, buff, &token);
  
  // If error token received
  if(!(token & 0xF0))
  {
      SD_print_data_error_token(token);
  }
  else if(token == 0xFF)
  {
    debug_printf("\tTimeout!");
  }
}

void SD_single_write_test(uint32_t addr, uint8_t *buff, uint8_t token, uint8_t *res)
{
  // Fill the buffer array
  for(uint16_t i=0; i<512; i++)
  { 
    buff[i] = 0x55;  
  }
  // Write single block to data to SD Card
  res[2] = SD_write_single_block(addr, buff, &token);
  
  // Check the results of the programming 
  SD_send_status(res);
}

uint8_t SD_multiple_read_test(uint32_t addr, uint8_t *buff, uint8_t token, uint32_t num_blocks)
{
  
}