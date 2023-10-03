#ifndef __SD_CARD_H
#define __SD_CARD_H

#include "stm32f4xx.h"

// Definitions of hardware parameters
#define F_CPU                 4000000               // HSE oscillator value
#define SD_BLOCK_LEN          512                   // SD Block of data length

// Definitions for SDC/MMC command
#define CMD0                  0                     // CMD0 Command index
#define CMD0_ARG              0x00000000            // CMD0 Argument
#define CMD0_CRC              0x94                  // CMD0 CRC(Cyclic Redundancy Check)
#define CMD8                  8                     // CMD8 Command index
#define CMD8_ARG              0x000001AA            // CMD8 Arguemnt (Reserved bits, PCIE, VHS, Check pattern)
#define CMD8_CRC              0x86                  // CMD8 CRC(1000011<<1)
#define CMD58                 58                    // CMD58 Command index
#define CMD58_ARG             0x00000000            // CMD58 Argument
#define CMD58_CRC             0x00                  // CMD58 CRC
#define CMD55                 55                    // CMD55 Command index
#define CMD55_ARG             0x00000000            // CMD55 Argument
#define CMD55_CRC             0x00                  // CMD55 CRC
#define ACMD41                41                    // ACMD41 Command index
#define ACMD41_ARG            0x40000000            // ACMD41 Argument
#define ACMD41_CRC            0x00                  // ACMD41 CRC
#define ACMD22                22                    // ACMD22 Command index
#define ACMD22_ARG            0x40000000            // ACMD41 Argument
#define ACMD22_CRC            0x00                  // ACMD22 CRC
#define CMD17                 17                    // CMD17 Command index
#define CMD17_CRC             0x00                  // CMD17 CRC
#define CMD24                 24                    // CMD24 Command index
#define CMD24_CRC             0x00                  // CMD24 CRC
#define CMD13                 13                    // CMD13 Command index
#define CMD13_ARG             0x00000000            // CMD13 Argument
#define CMD13_CRC             0x00                  // CMD13 CRC
#define CMD18                 18                    // CMD18 Command index
#define CMD18_CRC             0x00                  // CMD18 CRC
#define CMD12                 12                    // CMD12 Command index
#define CMD12_ARG             0x00000000            // CMD12 Argument
#define CMD12_CRC             0x00                  // CMD12 CRC
#define CMD25                 25                    // CMD25 Command index
#define CMD25_CRC             0x00                  // CMD25 CRC
/*
For a 4 Mhz HSE oscillator and SPI clock set to divide by 2,
to get the number of bytes we need to send over SPI to reach 100ms
(0.1s * 4000000 Hz) / (2 * 8 bytes) = 625000
*/
#define SD_MAX_READ_ATTEMPTS  (0.1*F_CPU) / (4*8)   // Max number of SD card read attempts
#define SD_MAX_WRITE_ATTEMPTS (0.25*F_CPU) / (4*8)  // Max number of SD card write attempts


// Definition of Data Tokens
#define SD_TOKEN_OOR(X)                 X & 0b00001000
#define SD_TOKEN_CECCF(X)               X & 0b00000100
#define SD_TOKEN_CC(X)                  X & 0b00000010
#define SD_TOKEN_ERROR(X)               X & 0b00000001
#define SD_START_BLOCK_TOKEN            0xFE
#define SD_MULTIPLE_START_BLOCK_WRITE   0xFC
#define SD_STOP_TRAN_TOKEN              0xFD


// Definition SD statu codes
#define SD_SUCCESS            0
#define SD_ERROR              1

  
// SD card methods
void delay(uint32_t);
void SD_power_up_seq(void);
void SD_command(uint8_t, uint32_t, uint8_t);
uint8_t SD_read_resp1();
uint8_t SD_go_idle_state();
void SD_read_resp3_7(uint8_t *res);
void SD_send_if_cond(uint8_t *res);
void SD_read_OCR(uint8_t *);
uint8_t SD_app_cmd(void);
uint8_t SD_send_op_cond(void);
uint8_t SD_init(void);
uint8_t SD_read_single_block(uint32_t addr, uint8_t *buf, uint8_t *token);
uint8_t SD_read_multiple_block(uint32_t addr, uint8_t *buf, uint8_t *token, uint32_t num_blocks);
void SD_print_data_error_token(uint8_t);
uint8_t SD_write_single_block(uint32_t addr, uint8_t *buf, uint8_t *token);
uint8_t SD_write_multiple_block(uint32_t addr, uint8_t *buf, uint8_t *token, uint32_t num_blocks);
void SD_read_resp2(uint8_t *res);
void SD_send_status(uint8_t *res);
uint8_t SD_send_num_wr_blocks(void);

#endif