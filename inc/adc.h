#ifndef H_ADC
#define H_ADC

#include "logger.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* IO operations definition for communication register of ADC9X ADC*/
#define ADC_WR          (0<<6)  /* 0b0000000 - write to */
#define ADC_RD          (1<<6)  /* 0b1000000 - read from */
#define ADC_CRD         (0<<2)  /* 0b100 - disable a continuous read mode */
#define ADC_CRE         (1<<2)  /* 0b100 - enable a continuous read mode */

/* Registers address description for ADC9X ADC */
#define ADC_STATUS_REG  (0<<3)  /* 0b0000;   8-bit. Status Register During a Read Operation (RO) */
#define ADC_MODE_REG    (1<<3)  /* 0b1000;   16-bit. Mode Register (R/w) */
#define ADC_CONFIG_REG  (2<<3)  /* 0b10000;  16-bit. Configuration Register (R/W) */
#define ADC_DATA_REG    (3<<3)  /* 0b11000;  16/24-bit. Data Register (RO) */
#define ADC_ID_REG      (4<<3)  /* 0b100000; 8-bit. ID Register (RO) */
#define ADC_IO_REG      (5<<3)  /* 0b100000; 8-bit. IO Register (RW) */
#define ADC_OFF_REG     (6<<3)  /* 0b100000; 24-bit. OFFSET REGISTER Register (RW) */
#define ADC_FS_REG      (7<<3)  /* 0b100000; 24-bit. FULL SCALE Register (RW) */

/* MODE REGISTER constants for ADC9X ADC */
#define ADC_MR_MD_CC    (0<<13) /* Continuous Conversion mode */
#define ADC_MR_MD_SC    (1<<13) /* Single Conversion mode */
#define ADC_MR_MD_IM    (2<<13) /* Idle Mode */
#define ADC_MR_MD_PD    (3<<13) /* Power Down mode */ 
#define ADC_MR_MD_IZC   (4<<13) /* Internal Zero Calibration mode */ 
#define ADC_MR_MD_IFC   (5<<13) /* Internal Full-scale Calibration mode */ 
#define ADC_MR_MD_SZC   (6<<13) /* System Zero Calibration mode */ 
#define ADC_MR_MD_SFC   (7<<13) /* System Full-scale Calibration mode */ 
#define ADC_MR_FS_470    0x01   /* 470 Hz,   4 ms */
#define ADC_MR_FS_242    0x02   /* 242 Hz,   8 ms */
#define ADC_MR_FS_123    0x03   /* 123 Hz,  16 ms */
#define ADC_MR_FS_62     0x04   /*  62 Hz,  32 ms */
#define ADC_MR_FS_50     0x05   /*  50 Hz,  40 ms */
#define ADC_MR_FS_39     0x06   /*  39 Hz,  48 ms */
#define ADC_MR_FS_33     0x07   /*  33 Hz,  60 ms */
#define ADC_MR_FS_19     0x08   /*  19 Hz, 101 ms */
#define ADC_MR_FS_16     0x0A   /*  16 Hz, 120 ms */
#define ADC_MR_FS_12     0x0B   /*  12 Hz, 160 ms */
#define ADC_MR_FS_10     0x0C   /*  10 Hz, 200 ms */
#define ADC_MR_FS_8      0x0D   /*   8 Hz, 240 ms */
#define ADC_MR_FS_6      0x0E   /*   6 Hz, 320 ms */
#define ADC_MR_FS_4      0x0F   /*   4 Hz, 480 ms */

/* CONFIGURATION REGISTER constants for ADC9X ADC */
#define ADC_CR_BO       (1<<13) // Enable Burn-Out current
#define ADC_CR_BM       (0<<12) // Enable Bipollar operation Mode
#define ADC_CR_UM       (1<<12) // Enable Unipolar operation Mode
#define ADC_CR_Gain1    (0<<8)  // Gain   x1
#define ADC_CR_Gain2    (1<<8)  // Gain   x2
#define ADC_CR_Gain4    (2<<8)  // Gain   x4
#define ADC_CR_Gain8    (3<<8)  // Gain   x8
#define ADC_CR_Gain16   (4<<8)  // Gain  x16
#define ADC_CR_Gain32   (5<<8)  // Gain  x32
#define ADC_CR_Gain64   (6<<8)  // Gain  x64
#define ADC_CR_Gain128  (7<<8)  // Gain x128
#define ADC_CR_RD       (1<<5)  // Enable Reference Detect Function
#define ADC_CR_BUF      (1<<4)  // Enable data Buferization
#define ADC_CR_CH1       0x00   // AIN1(+) - AIN1(-)
#define ADC_CR_CH2       0x01   // AIN2(+) - AIN2(-)
#define ADC_CR_CH3       0x02   // AIN3(+) - AIN3(-)

void        ADC_Reset(void);
void        ADC_SetMode(uint16_t MODE);
void        ADC_SetConfig(uint16_t CONF);
void        ADC_ReadDataCont(uint32_t* data, uint16_t num, uint8_t FS);
void        ADC_WriteOFF(uint32_t OFF_REG_VAL);
void        ADC_MakeCalibration(void);
uint8_t     ADC_SendByte(uint8_t byte);
uint8_t     ADC_ReadID(void);
uint8_t     ADC_ReadStatus(void);
uint8_t     ADC_CheckStatus(void);
uint16_t    ADC_ReadMode(void);
uint16_t    ADC_ReadConfig(void);
uint32_t    ADC_ReadOFF(void);
uint32_t    ADC_ReadFS(void);
uint32_t    ADC_ReadDataSingle(uint8_t FS);

int     adc_get(int);
int     adc_get_stored(int);
void    adc_init(int);
void    vADCTask(void* vpars);

#endif /* H_ADC */
