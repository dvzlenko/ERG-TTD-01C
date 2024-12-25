#include <memory.h>

// structures definition
extern LoggerSettings  loggerSettings;
extern TimeSettings    timeSettings;
extern DeviceInfo      deviceInfo;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
/* FUNCTIONS THAT OPERATE THE INTERNAL FLASH OF THE STM */ 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 

/* reads Device Info from the MCU FLASH */ 
void ReadDevInfo(void) {
    uint8_t i;
    uint32_t *source_addr = (uint32_t *)DEV_INFO_ADDR;
    uint32_t *dest_addr = (void *)&deviceInfo;
    for (i = 0; i < sizeof(deviceInfo)/4; i++) {
        *dest_addr = *(__IO uint32_t*)source_addr;
        source_addr++;
        dest_addr++;
    }
}

/* writes Device Info to the MCU FLASH */ 
void WriteDevInfo(void) {
    uint8_t i;
    uint32_t *source_addr = (void *)&deviceInfo;
    uint32_t *dest_addr = (uint32_t *) DEV_INFO_ADDR;
    FLASH_Unlock();
    FLASH_ErasePage(DEV_INFO_ADDR);
    for (i = 0; i < sizeof(deviceInfo)/4; i++) {
        FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
        source_addr++;
        dest_addr++;
    }
    FLASH_Lock();
}

/* reads ProgrammSettings from 1 kByte page of the MCU FLASH */ 
void ReadProgramSettings(void) {
    uint8_t i;
    uint32_t *source_addr = (uint32_t *)PROG_SET_ADDR;
    uint32_t *dest_addr = (void *)&loggerSettings;
    for (i = 0; i < sizeof(loggerSettings)/4; i++) {
        *dest_addr = *(__IO uint32_t*)source_addr;
        source_addr++;
        dest_addr++;
    }
}

/* writes ProgrammSettings in one 1 kByte page of the MCU FLASH */ 
void WriteProgramSettings(void) {
    uint8_t i;
    uint32_t *source_addr = (void *)&loggerSettings;
    uint32_t *dest_addr = (uint32_t *) PROG_SET_ADDR;
    FLASH_Unlock();
    FLASH_ErasePage(PROG_SET_ADDR);
    for (i = 0; i < sizeof(loggerSettings)/4; i++) {
        FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
        source_addr++;
        dest_addr++;
    }
    FLASH_Lock();
}

/* reads TimeSettings from 1 kByte page of the MCU FLASH */ 
void ReadTimeSettings(void) {
    uint8_t i;
    uint32_t *source_addr = (uint32_t *)TIME_SET_ADDR;
    uint32_t *dest_addr = (void *)&timeSettings;
    for (i = 0; i < sizeof(timeSettings)/4; i++) {
        *dest_addr = *(__IO uint32_t*)source_addr;
        source_addr++;
        dest_addr++;
    }
}

/* writes Time Settings in one 1 kByte page of the MCU FLASH */ 
void WriteTimeSettings(void) {
    uint8_t i;
    uint32_t *source_addr = (void *)&timeSettings;
    uint32_t *dest_addr = (uint32_t *) TIME_SET_ADDR;
    FLASH_Unlock();
    FLASH_ErasePage(TIME_SET_ADDR);
    for (i = 0; i < sizeof(timeSettings)/4; i++) {
        FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
        source_addr++;
        dest_addr++;
    }
    FLASH_Lock();
}

/* reads the current address in the FLASH from the BPR of STM */ 
uint32_t GetAddress(void) {
    uint16_t tmp1, tmp2;
    tmp1 = BKP_ReadBackupRegister(BKP_DR1);
    tmp2 = BKP_ReadBackupRegister(BKP_DR2);
    return ((tmp1 << 16) | tmp2);
}

/* saves the current address in the FLASH to the BPR of STM */ 
void SaveAddress(uint32_t addr) {
    BKP_WriteBackupRegister(BKP_DR1, addr >> 16);
    BKP_WriteBackupRegister(BKP_DR2, addr);
}

/* reads the correction number for the effective data points made since the schedule start time
   it is necessary as the start time can be placed in the past and the programm needs to calculate 
   the correctly the next WakeUp time moment */ 
uint32_t GetNumberCorrection(void) {
    uint16_t tmp1, tmp2;
    tmp1 = BKP_ReadBackupRegister(BKP_DR5);
    tmp2 = BKP_ReadBackupRegister(BKP_DR6);
    return ((tmp1 << 16) | tmp2);
}

/* saves the correction number */ 
void SaveNumberCorrection(uint32_t num) {
    BKP_WriteBackupRegister(BKP_DR5, num >> 16);
    BKP_WriteBackupRegister(BKP_DR6, num);
}

/* reads the BLINK constant
   1 - means 1/4 sec and 0 - means 1/10 sec */
uint16_t GetBlinkMode(void) {
    return BKP_ReadBackupRegister(BKP_DR9);
}

/* writes the BLINK constant */ 
void SaveBlinkMode(uint16_t num) {
    BKP_WriteBackupRegister(BKP_DR9, num);
}


/*+++++++++++++++++++++++++++++++++++++++++++*/ 
/* FUNCTIONS THAT OPERATE THE SPI FLASH CHIP */
/*+++++++++++++++++++++++++++++++++++++++++++*/


/* Enables the write access to the FLASH */ 
void MEM_WriteEnable(void) {
    uint8_t tmp;
    // Select the FLASH: Chip Select low
    MEM_CS_LOW();
    // Send "Write Enable" instruction
    tmp = MY_SPI_SendByte(MEM_CMD_WREN);
    // Deselect the FLASH: Chip Select high
    MEM_CS_HIGH();
    }

/* Disables the write access to the FLASH */ 
void MEM_WriteDisable(void) {
    uint8_t tmp;
    // Select the FLASH: Chip Select low
    MEM_CS_LOW();
    // Send "Write Disable" instruction
    tmp = MY_SPI_SendByte(MEM_CMD_WRDI);
    // Deselect the FLASH: Chip Select high
    MEM_CS_HIGH();
    }

/* Polls the status of the Write In Progress (WIP) flag in the FLASH's */ 
void MEM_WaitForWriteEnd(void) {
    uint8_t tmp, flashstatus = 0;
    // Select the FLASH: Chip Select low 
    MEM_CS_LOW();
    // Send "Read Status Register" instruction
    tmp = MY_SPI_SendByte(MEM_CMD_RDSR);
    // Loop as long as the memory is busy with a write cycle
    // Send a dummy byte to generate the clock needed by the FLASH and put the value of the status register in FLASH_Status variable
    do {
        flashstatus = MY_SPI_SendByte(MEM_DUMMY_BYTE);
        }
    while ((flashstatus & MEM_WIP_FLAG) == SET); // Write in progress
    // Deselect the FLASH: Chip Select high
    MEM_CS_HIGH();
    }

/* Reads FLASH identification */ 
uint32_t MEM_ReadID(void) {
    uint32_t tmp0 = 0, tmp1 = 0, tmp2 = 0;
    // Select the FLASH: Chip Select low
    MEM_CS_LOW();
    // Send "RDID " instruction
    tmp0 = MY_SPI_SendByte(0x9F);
    // Read a byte from the FLASH
    tmp0 = MY_SPI_SendByte(MEM_DUMMY_BYTE);
    // Read a byte from the FLASH 
    tmp1 = MY_SPI_SendByte(MEM_DUMMY_BYTE);
    // Read a byte from the FLASH
    tmp2 = MY_SPI_SendByte(MEM_DUMMY_BYTE);
    // Deselect the FLASH: Chip Select high
    MEM_CS_HIGH();
    return (tmp0 << 16) | (tmp1 << 8) | tmp2;
    }

/* Reads FLASH status register */ 
uint8_t MEM_ReadSR() {
    uint8_t tmp;
    // Select the FLASH: Chip Select low
    MEM_CS_LOW();
    // Send "RDID " instruction
    tmp = MY_SPI_SendByte(MEM_CMD_RDSR);
    // Read a byte from the FLASH
    tmp = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Deselect the FLASH: Chip Select high
    MEM_CS_HIGH();
    return tmp;
    }

uint8_t MEM_DO(uint16_t CMD) {
    uint8_t tmp;
    // Select the FLASH: Chip Select low
    MEM_CS_LOW();
    // Send "RDSR " instruction
    tmp = MY_SPI_SendByte(CMD);
    // Read a byte from the FLASH
    tmp = MY_SPI_SendByte(MEM_DUMMY_BYTE);
    // Deselect the FLASH: Chip Select high
    MEM_CS_HIGH();
    return tmp;
    }

/* Erases the specified FLASH sector */ 
void MEM_EraseSector(uint32_t SectorAddr) {
    uint8_t tmp;
    // Send write enable instruction
    MEM_WriteEnable();
    // Sector Erase 
    // Select the FLASH: Chip Select low 
    MEM_CS_LOW();
    // Send Sector Erase instruction  
    tmp = MY_SPI_SendByte(MEM_CMD_SE);
    // Send SectorAddr high nibble address byte  
    tmp = MY_SPI_SendByte((SectorAddr & 0xFF0000) >> 16);
    // Send SectorAddr medium nibble address byte  
    tmp = MY_SPI_SendByte((SectorAddr & 0xFF00) >> 8);
    // Send SectorAddr low nibble address byte  
    tmp = MY_SPI_SendByte(SectorAddr & 0xFF);
    // Deselect the FLASH: Chip Select high  
    MEM_CS_HIGH();
    // Wait for write end  
    MEM_WaitForWriteEnd();
    }

void MEM_EraseBlock(uint32_t SectorAddr) {
    uint8_t tmp;
    // Send write enable instruction  
    MEM_WriteEnable();
    // Sector Erase  
    // Select the FLASH: Chip Select low  
    MEM_CS_LOW();
    // Send Sector Erase instruction  
    tmp = MY_SPI_SendByte(MEM_CMD_BE);
    // Send SectorAddr high nibble address byte  
    tmp = MY_SPI_SendByte((SectorAddr & 0xFF0000) >> 16);
    // Send SectorAddr medium nibble address byte  
    tmp = MY_SPI_SendByte((SectorAddr & 0xFF00) >> 8);
    // Send SectorAddr low nibble address byte  
    tmp = MY_SPI_SendByte(SectorAddr & 0xFF);
    // Deselect the FLASH: Chip Select high  
    MEM_CS_HIGH();
    // Wait for write end  
    MEM_WaitForWriteEnd();
    }

/* Writes more than one byte to the FLASH with a single WRITE cycle (Page WRITE sequence) */
uint16_t MEM_WriteData(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
    uint8_t tmp;
    uint16_t rt = NumByteToWrite;
    // Enable the write access to the FLASH  
    MEM_WriteEnable();
    // Select the FLASH: Chip Select low  
    MEM_CS_LOW();
    // Send "Write to Memory " instruction  
    tmp = MY_SPI_SendByte(MEM_CMD_WRITE);
    // Send WriteAddr high nibble address byte to write to  
    tmp = MY_SPI_SendByte((WriteAddr & 0xFF0000) >> 16);
    // Send WriteAddr medium nibble address byte to write to  
    tmp = MY_SPI_SendByte((WriteAddr & 0xFF00) >> 8);
    // Send WriteAddr low nibble address byte to write to  
    tmp = MY_SPI_SendByte(WriteAddr & 0xFF);
    // while there is data to be written on the FLASH  
    while (NumByteToWrite--) {
        // Send the current byte  
        tmp = MY_SPI_SendByte(*pBuffer);
        // Point on the next byte to be written  
        pBuffer++;
        }
    // Deselect the FLASH: Chip Select high  
    MEM_CS_HIGH();
    // Wait for write end  
    MEM_WaitForWriteEnd();
    return rt;
    }

/* Reads a block of data from the FLASH */
void MEM_ReadData(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) {
    uint8_t tmp;
    // Select the FLASH: Chip Select low  
    MEM_CS_LOW();
    // Send "Read from Memory " instruction  
    tmp = MY_SPI_SendByte(MEM_CMD_READ);
    // Send ReadAddr high nibble address byte to read from  
    tmp = MY_SPI_SendByte((ReadAddr & 0xFF0000) >> 16);
    // Send ReadAddr medium nibble address byte to read from  
    tmp = MY_SPI_SendByte((ReadAddr & 0xFF00) >> 8);
    // Send ReadAddr low nibble address byte to read from  
    tmp = MY_SPI_SendByte(ReadAddr & 0xFF);
    // while there is data to be read 
    while (NumByteToRead--) {
        // Read a byte from the FLASH  
        *pBuffer = MY_SPI_SendByte(MEM_DUMMY_BYTE);
        // Point to the next location where the byte read will be saved  
        pBuffer++;
        }
    // Deselect the FLASH: Chip Select high  
    MEM_CS_HIGH();
    }

/* Initiates a read data byte (READ) sequence from the Flash. This is done by driving the /CS line low to select the device, then the READ
   instruction is transmitted followed by 3 bytes address. This function exit and keep the /CS line low, so the Flash still being selected. 
   With this technique the whole content of the Flash is read with a single READ instruction. */
void MEM_StartReadSequence(uint32_t ReadAddr) {
    uint8_t tmp;
    // Select the FLASH: Chip Select low  
    MEM_CS_LOW();
    // Send "Read from Memory " instruction  
    tmp = MY_SPI_SendByte(MEM_CMD_READ);
    // Send ReadAddr high nibble address byte  
    tmp = MY_SPI_SendByte((ReadAddr & 0xFF0000) >> 16);
    // Send ReadAddr medium nibble address byte  
    tmp = MY_SPI_SendByte((ReadAddr & 0xFF00) >> 8);
    // Send ReadAddr low nibble address byte  
    tmp = MY_SPI_SendByte(ReadAddr & 0xFF);
    }

