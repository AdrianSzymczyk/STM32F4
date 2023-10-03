#include "stm32f4xx.h"
#include "spi_conf.h"
#include "sd_card.h"
#include <debugio.h>

void delay(uint32_t m_seconds)
{
  for(uint32_t i=0; i<m_seconds; i++);
}

void SD_power_up_seq(void)
{
  // Deselect SD card (Pull the CS pin high)
  CS_disable();

  // Wait 1 msec to power up the SD card
  delay(1);

  // Send 80 clock cycles to synchronize
  for(uint8_t i=0; i<10; i++)
  {
    SPI_transmit(0xFF);
  }

  // Deselect SD card (Pull the CS pin high)
  CS_disable();
  SPI_transmit(0xFF);
}

void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  // Transmit command to SD card
  SPI_transmit(0x40|cmd);

  // Transmit argument
  SPI_transmit((uint8_t)(arg>>24)); // Transmit 1 Most Significant Byte
  SPI_transmit((uint8_t)(arg>>16)); // Transmit 2 Most Significant Byte
  SPI_transmit((uint8_t)(arg>>8));  // Transmit 3 Most Significant Byte
  SPI_transmit((uint8_t)(arg));     // Transmit 4 Most Significant Byte

  // Transmit CRC
  SPI_transmit(crc|0x01);
}

uint8_t SD_read_resp1(void)
{
  uint8_t response, i=0;
  
  // Poll until actual data received
  while((response = SPI_transmit(0xFF)) == 0xFF)
  {
    i++;
    // Break if no data received for 8 bytes
    if(i>8) break;
  }

  return response;
}

uint8_t SD_go_idle_state(void)
{
  // Assert chip select
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_enable();        // Pull the CS pin low
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 

  // Send CMD0 command
  SD_command(CMD0, CMD0_ARG, CMD0_CRC);

  // Read response
  uint8_t res = SD_read_resp1();

  // Deassert chip select 
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_disable();       // Pull the CS pin high
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 

  return res;
}

void SD_read_resp3_7(uint8_t *res)
{
  // Read response 1 in R7
  res[0] = SD_read_resp1();

  // If error reading R1
  if(res[0] > 1) return;

  // Read remaining bytes
  res[1] = SPI_transmit(0xFF);
  res[2] = SPI_transmit(0xFF);
  res[3] = SPI_transmit(0xFF);
  res[4] = SPI_transmit(0xFF);
}


void SD_send_if_cond(uint8_t *res)
{
  // Assert chip select
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_enable();        // Pull the CS pin low
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 

  // Send CMD8 command
  SD_command(CMD8, CMD8_ARG, CMD8_CRC);

  // Read response 
  SD_read_resp3_7(res);

  // Deassert chip select 
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_disable();       // Pull the CS pin high
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
}


void SD_read_OCR(uint8_t *res)
{
  // Assert chip select
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_enable();        // Pull the CS pin low
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 

  // Send CMD58 command
  SD_command(CMD58, CMD58_ARG, CMD58_CRC);

  // Read response
  SD_read_resp3_7(res);

  // Deassert chip select 
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_disable();       // Pull the CS pin high
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
}


uint8_t SD_app_cmd(void)
{
  // Assert chip select
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_enable();        // Pull the CS pin low
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 

  // Send CMD55 command
  SD_command(CMD55, CMD55_ARG, CMD55_CRC);

  // Read response
  uint8_t res = SD_read_resp1();

  // Deassert chip select 
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_disable();       // Pull the CS pin high
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  
  return res;
}


uint8_t SD_send_op_cond(void)
{
  // Assert chip select
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_enable();        // Pull the CS pin low
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 

  // Send ACMD41 command
  SD_command(ACMD41, ACMD41_ARG, ACMD41_CRC);

  // Read response
  uint8_t res = SD_read_resp1();

  // Deassert chip select 
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_disable();       // Pull the CS pin high
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  
  return res;
}


uint8_t SD_init(void)
{
  uint8_t res[5], cmd_attempts = 0;

  // Start power up sequence
  SD_power_up_seq();      

  // Reset the SD Memory Card
  while((res[0] = SD_go_idle_state()) != 0x01) // CMD0 - Reset the SD Memory Card
  {
    cmd_attempts++;
    if(cmd_attempts > 10)
    {
      return SD_ERROR; // Return SD error
    }
  }

  // Send interface conditions
  SD_send_if_cond(res); // CMD8 - Check whether the card is first generation of Version 2.00(or later) 
  if(res[0] != 0x01)
  {
    return SD_ERROR;    // Return SD error
  }

  // Check echo pattern
  if(res[4] != 0xAA)
  {
    return SD_ERROR; // Return SD error
  }

  cmd_attempts = 0;  // Reset cmd attempts
  do
  {
    if(cmd_attempts > 100)
    {
      return SD_ERROR;
    }
    // Define next command as an application command
    res[0] = SD_app_cmd(); // CMD55  - Define to the card that the next command is an application specific command

    // Check whethere no error in response
    if(res[0] < 2)
    {
      res[0] = SD_send_op_cond();
    }
    
    cmd_attempts++;
  }
  while(res[0] != SD_SUCCESS);

  // Read OCR
  SD_read_OCR(res); // CMD58  - Read the OCR register of a card
  
  // Check whether card is ready
  if(!(res[1] & 0x80))
  {
    return SD_ERROR; // Return SD error
  }

  return SD_SUCCESS;
}


uint8_t SD_read_single_block(uint32_t addr, uint8_t *buf, uint8_t *token)
 {
  /*
  addr = address of 0 will read back bytes 0-511, address 1 will read 512-1023
  For 32GB card will be (2^26)*512=32768 MB
  Maximum address will be 0x4000000
  token = 0xFE - Successful read
  token = 0x0X - Data error
  token = 0xFF - Timeout
  */
  uint8_t res1, read;
  uint16_t readAttempts;

  // Set token value to None
  *token = 0xFF;

  // Assert chip select
  SPI_transmit(0xFF);
  CS_enable();
  SPI_transmit(0xFF);

  // Send CMD17
  SD_command(CMD17, addr, CMD17_CRC);

  // Read response 
  res1 = SD_read_resp1();

  // If response received from card
  if(res1 !=0xFF)
  {
    // Wait for a response token (100ms timeout minimum for single read operations)
    readAttempts = 0;
    while(++readAttempts < SD_MAX_READ_ATTEMPTS)
    {
      if((read = SPI_transmit(0xFF)) != 0xFF) break;
    }

    // If response token is 0xFE
    if(read = 0xFE)
    {
      // Read 512 byte block
      for(uint16_t i=0; i<SD_BLOCK_LEN; i++)
      {
        *buf++ = SPI_transmit(0xFF);
      }
      // Read 16-bit CRC
      SPI_transmit(0xFF);
      SPI_transmit(0xFF);
    }

    // Set token to card response
    *token = read;
  }

  // Deassert chip select 
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 
  CS_disable();       // Pull the CS pin high
  SPI_transmit(0xFF); // Wait 8 recommended clock cycles 

  return res1;
}

void SD_print_data_error_token(uint8_t token)
{
  if(SD_TOKEN_ERROR(token))
    debug_printf("\tA general or an unknown error occurred during the operation.");
  else if(SD_TOKEN_CC(token))
    debug_printf("\tInternal card controller error.");
  else if(SD_TOKEN_CECCF(token))
    debug_printf("\tCard internal ECC (error correction) was applied but failed to correct the data.");
  else if(SD_TOKEN_OOR(token))
    debug_printf("Data address argument is out of range.");
}

uint8_t SD_write_single_block(uint32_t addr, uint8_t *buf, uint8_t *token)
{
  /*
  Write single 512 byte block
  token = 0x00 - busy timeout
  token = 0x05 - data accepted
  token = 0xFF - response timeout
  */
  uint8_t read, res1;
  uint16_t read_attempts;
  // Set token value to None
  *token = 0xFF;

  // Assert chip select
  SPI_transmit(0xFF);
  CS_enable();
  SPI_transmit(0xFF);

  // Send CMD24
  SD_command(CMD24, addr, CMD24_CRC);

  // Read response
  res1 = SD_read_resp1();

  // If card responds with any error
  if(res1 == SD_SUCCESS)
  {
    // Send Start Block Token
    SPI_transmit(SD_START_BLOCK_TOKEN);

    // Write buffer to SD card
    for(uint16_t i=0; i<SD_BLOCK_LEN; i++)
    {
      SPI_transmit(buf[i]);
    }

    // Wait for data response (timeout = 250ms)
    // Maximum timeout is defined as 250ms for all write operations
    read_attempts = 0;
    while(++read_attempts < SD_MAX_WRITE_ATTEMPTS)
    {
      if((read = SPI_transmit(0xFF)) != 0xFF) break;
    }

    // If data accepted
    if((read & 0x1F) == 0x05)
    {
      // Set token to data accepted
      *token = 0x05;

      // Wait for write to finish (timeout = 250ms)
      read_attempts=0;
      while(SPI_transmit(0xFF) == 0x00)
      {
        if(++read_attempts > SD_MAX_WRITE_ATTEMPTS)
        {
          *token = 0x00;
          break;
        }
      }
    }
  }
  
  // Deassert chip select
  SPI_transmit(0xFF);
  CS_disable();
  SPI_transmit(0xFF);

  return res1;
}

void SD_read_resp2(uint8_t *res)
{
  // Read the first byte which is identical to the response R1
  res[0] = SD_read_resp1();

  // Return if response 1 error
  if(res[0] > 1) return;
  
  // Read second byte
  res[1] = SPI_transmit(0xFF);
}


void SD_send_status(uint8_t *res)
{
  // Assert chip select
  SPI_transmit(0xFF);
  CS_enable();
  SPI_transmit(0xFF);

  // Send CMD13 command
  SD_command(CMD13, CMD13_ARG, CMD13_CRC);

  // Read R2 response
  SD_read_resp2(res);

  // Deassert chip select
  SPI_transmit(0xFF);
  CS_disable();
  SPI_transmit(0xFF);
}

uint8_t SD_read_multiple_block(uint32_t addr, uint8_t *buf, uint8_t *token, uint32_t num_blocks)
{
  /*
  token = 0xFE - Successful read
  token = 0x0X - Data error
  token = 0xFF - Timeout
  */
  uint8_t res1, res1b, read;
  uint16_t read_attempts;

  // Set token value to None
  *token = 0xFF;

  // Assert chip select
  SPI_transmit(0xFF);
  CS_enable();
  SPI_transmit(0xFF);
  
  // Send CMD17 command
  SD_command(CMD18, addr, CMD18_CRC);

  // Read response from the card
  res1 = SD_read_resp1();

  // If card responds with any error
  if(res1 != 0xFF)
  {
    for(uint32_t i=0; i<num_blocks; i++) // For loop to check address
    {
      debug_printf("\nCurrent address: 0x%08X", addr);
      // Wait 100ms timeout for multiple read operations
      read_attempts = 0;
      while(++read_attempts < SD_MAX_READ_ATTEMPTS)
      {
        if((read = SPI_transmit(0xFF) != 0xFF)) break;
      }

      // If response token is 0xFE (data is starting)
      if(read = 0xFE)
      {
        // Read 512 byte block of data
        for(uint16_t j=0; j<SD_BLOCK_LEN; j++)
        {
          *buf++ = SPI_transmit(0xFF);
        }
        // Read 16-bit CRC
        SPI_transmit(0xFF);
        SPI_transmit(0xFF);
      }

      // Check if CMD12(stop transmission command) needs to be send 
      if(i == num_blocks -1)
      {
        SD_command(CMD12, CMD12_ARG, CMD12_CRC);
      }
    }
    // Set token to card response
    *token = read;

    // Read the card response
    res1b = SD_read_resp1();
  }

  // Deassert chip select
  SPI_transmit(0xFF);
  CS_disable();
  SPI_transmit(0xFF);

  return res1b;
}

uint8_t SD_send_num_wr_blocks(void)
{
  // This method has to be preceded with CMD55 command
  // Assert chip select
  SPI_transmit(0xFF);
  CS_enable();
  SPI_transmit(0xFF);
  
  // Send ACMD22 command
  SD_command(ACMD22, ACMD22_ARG, ACMD22_CRC);

  // Read response
  uint8_t res = SD_read_resp1();

  // Deassert chip select
  SPI_transmit(0xFF);
  CS_disable();
  SPI_transmit(0xFF);

  return res;
}


uint8_t SD_write_multiple_block(uint32_t addr, uint8_t *buf, uint8_t *token, uint32_t num_blocks)
{
  /*
  token = 0x00 - busy timeout
  token = 0x05 - data accepted
  token = 0xFF - response timeout
  */
  uint8_t resp1, read, res[5];
  uint32_t read_attempts;
  // Set token value to None
  *token = 0xFF;

  // Assert chip select
  SPI_transmit(0xFF);
  CS_enable();
  SPI_transmit(0xFF);

  // Send CMD25 command
  SD_command(CMD25, addr, CMD25_CRC);

  // Read SC card response
  resp1 = SD_read_resp1();

  // If card responds with any error
  if(resp1 == SD_SUCCESS)
  {
    for(uint32_t i=0; i<num_blocks; i++)
    {
      // Check if CMD12 needs to be sent to stop the transmission
      if(i < num_blocks - 1)
      {
        SPI_transmit(SD_MULTIPLE_START_BLOCK_WRITE); // Send start block token
      }
      else SPI_transmit(SD_STOP_TRAN_TOKEN);         // Send stop transmission token

      // Write buffer to SD card
      for(uint16_t j=0; j<SD_BLOCK_LEN; j++)
      {
        SPI_transmit(*((buf+i*SD_BLOCK_LEN) + j));
      }

      // Wait for data response (timeout = 250ms)
      read_attempts = 0;
      while(++read_attempts < SD_MAX_WRITE_ATTEMPTS)
      {
        if((read = SPI_transmit(0xFF)) != 0xFF) break;
      }

      // If data accepted
      if((read & 0x1F) == 0x05)
      {
        *token = 0x05;
      
        // Wait until card is busy
        read_attempts = 0;
        while(SPI_transmit(0xFF) == 0x00)
        {
          if(++read_attempts > SD_MAX_WRITE_ATTEMPTS)
          {
            *token = 0x00;
            break;
          }
        }
      }
      // If Write Error or CRC error is indicated
      else if((read & 0x1F) == 0x0D || (read & 0x1F) == 0x0B)
      {
        // Send stop transmission token
        SPI_transmit(SD_STOP_TRAN_TOKEN);     
        
        // Optional: Send number of well written write blocks    
        res[0] = SD_app_cmd();              // Define next command as an application command
        if(res[0] < 2)                      // Check whethere no error in response
        {
          res[0] = SD_send_num_wr_blocks(); // Get the numbers of the well written blocks
        }
      }
    }
  }
  // Deassert chip select
  SPI_transmit(0xFF);
  CS_disable();
  SPI_transmit(0xFF);

  return resp1;
}