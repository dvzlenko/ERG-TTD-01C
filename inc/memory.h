#ifndef __memory_h
#define __memory_h

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_flash.h"
#include "chat.h"
#include "logger.h"
    
#define PAGE_SIZE            256
#define SECTOR_SIZE         1024
#define MEM_DUMMY_BYTE   0xA5

/* M25P SPI Flash supported commands */
#define MEM_CMD_WRSR     0x01  /* Write Status Register instruction */
#define MEM_CMD_WRITE    0x02  /* Write to Memory instruction */
#define MEM_CMD_READ     0x03  /* Read from Memory instruction */
#define MEM_CMD_WRDI     0x04  /* Write disable instruction */
#define MEM_CMD_RDSR     0x05  /* Read Status Register instruction  */
#define MEM_CMD_WREN     0x06  /* Write enable instruction */
#define MEM_CMD_RDID     0x9F  /* Read identification */
#define MEM_CMD_RDSFDP   0x5A  /* Read The Serial Flash Discoverable Parameter */
#define MEM_CMD_RES      0xAB  /* Read Electronic Identificator */
#define MEM_CMD_SE       0x20  /* 4K Sector Erase instruction */
#define MEM_CMD_BE       0xD8  /* 64K Block Erase instruction */
#define MEM_CMD_CE       0xC7  /* Chip Erase instruction */
#define MEM_CMD_EN4B     0xB7  /* Enable 4-byte mode for addressing */
#define MEM_CMD_EX4B     0xE9  /* Disable 4-byte mode for addressing */
#define MEM_CMD_RDSCUR   0x2B  /* Read Security Register */
#define MEM_CMD_WRSCUR   0x2F  /* Wright Security Register */

#define MEM_WIP_FLAG     0x01  /* Write In Progress (WIP) flag */


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void     ReadDevInfo(void);
void     WriteDevInfo(void);
void     ReadProgramSettings(void);
void     WriteProgramSettings(void);
void     ReadTimeSettings(void);
void     WriteTimeSettings(void);
void     SaveAddress(uint32_t addr);
void     SaveNumberCorrection(uint32_t num);
void     SaveBlinkMode(uint16_t num);
uint16_t GetBlinkMode(void);
uint32_t GetNumberCorrection(void);
uint32_t GetAddress(void);
//void     SPI_LowLevel_Init(void);
//void     SPI_DeInit(void);
void     MEM_Init(void);
void     MEM_DeInit(void);
void     MEM_LowLevel_Init(void);
void     MEM_EN4B(void);
void     MEM_WriteEnable(void);
void     MEM_WriteDisable(void);
void     MEM_WaitForWriteEnd(void);
void     MEM_EraseSector(uint32_t SectorAddr);
void     MEM_EraseBlock(uint32_t SectorAddr);
void     MEM_ReadData(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void     MEM_StartReadSequence(uint32_t ReadAddr);
uint8_t  MEM_SendByte(uint8_t byte);
uint8_t  MEM_ReadByte(void);
uint8_t  MEM_ReadSR(void);
uint8_t  MEM_DO(uint16_t CMD);
uint16_t MEM_WriteData(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
uint32_t MEM_ReadID(void);

#endif

