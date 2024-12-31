#ifndef __logger_h
#define __logger_h

#include "hw_config.h"
#include "usb_lib.h"
#include "stdio.h"
#include "stdlib.h"
#include "FreeRTOS.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "memory.h"
#include "adc.h"

//#define PROG_SET_ADDR 0x0801EC00  // 119-th kByte of the STM32F103 medium density devices
#define PROG_SET_ADDR   0x0801F000  // 120-th kByte of the STM32F103 medium density devices
#define TIME_SET_ADDR   0x0801F400  // 121-st kByte of the STM32F103 medium density devices
#define DEV_INFO_ADDR   0x0801F800  // 122-th kByte of the STM32F103 medium density devices

// BLINK PIN
#define SYS_LED_RCC     RCC_APB2Periph_GPIOA
#define SYS_LED_GPIO    GPIOA
#define SYS_LED_PIN     GPIO_Pin_8

// MCU POWER OFF PIN
#define POWER_OFF_RCC    RCC_APB2Periph_GPIOA
#define POWER_OFF_GPIO   GPIOA
#define POWER_OFF_PIN    GPIO_Pin_9

// USB cable detection stuff
#define USB_DTC_RCC      RCC_APB2Periph_GPIOB
#define USB_DTC_GPIO     GPIOB
#define USB_DTC_PIN      GPIO_Pin_3

// TPS63001 PSM PIN
#define TPS_PSM_RCC      RCC_APB2Periph_GPIOB
#define TPS_PSM_GPIO     GPIOB
#define TPS_PSM_PIN      GPIO_Pin_4

// IR LED zero
#define LED_ON_RCC       RCC_APB2Periph_GPIOB
#define LED_ON_GPIO      GPIOB
#define LED_ON_PIN       GPIO_Pin_9   
    
// SPI
#define MY_SPI           SPI1
#define MY_SPI_RCC       RCC_APB2Periph_GPIOA
#define MY_SPI_CLK       RCC_APB2Periph_SPI1
#define MY_SPI_GPIO      GPIOA                       
#define MY_SPI_SCK_PIN   GPIO_Pin_5                  
#define MY_SPI_MISO_PIN  GPIO_Pin_6              
#define MY_SPI_MOSI_PIN  GPIO_Pin_7              
// DAC CS
#define DAC_CS_RCC       RCC_APB2Periph_GPIOB
#define DAC_CS_GPIO      GPIOB
#define DAC_CS_PIN       GPIO_Pin_0   
// ADC CS
#define ADC_CS_RCC       RCC_APB2Periph_GPIOB
#define ADC_CS_GPIO      GPIOB
#define ADC_CS_PIN       GPIO_Pin_5 
// MEM CS
#define MEM_CS_RCC       RCC_APB2Periph_GPIOA
#define MEM_CS_GPIO      GPIOA
#define MEM_CS_PIN       GPIO_Pin_4   
#define SPI_DUMMY_BYTE   0xA5

// DAC
#define DAC_CS_LOW()     GPIO_ResetBits(DAC_CS_GPIO, DAC_CS_PIN) 
#define DAC_CS_HIGH()    GPIO_SetBits(DAC_CS_GPIO, DAC_CS_PIN)
// ADC
#define ADC_CS_LOW()     GPIO_ResetBits(ADC_CS_GPIO, ADC_CS_PIN)
#define ADC_CS_HIGH()    GPIO_SetBits(ADC_CS_GPIO, ADC_CS_PIN)
// MEM
#define MEM_CS_LOW()     GPIO_ResetBits(MEM_CS_GPIO, MEM_CS_PIN)
#define MEM_CS_HIGH()    GPIO_SetBits(MEM_CS_GPIO, MEM_CS_PIN)
// SYS LED
#define SYS_LED_LOW()    GPIO_ResetBits(SYS_LED_GPIO, SYS_LED_PIN)
#define SYS_LED_HIGH()   GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN)
// IR LED
#define LED_ON_LOW()     GPIO_ResetBits(LED_ON_GPIO, LED_ON_PIN)
#define LED_ON_HIGH()    GPIO_SetBits(LED_ON_GPIO, LED_ON_PIN)
// TPS63001 PSM
#define TPS_PSM_LOW()    GPIO_ResetBits(TPS_PSM_GPIO, TPS_PSM_PIN)
#define TPS_PSM_HIGH()   GPIO_SetBits(TPS_PSM_GPIO, TPS_PSM_PIN)
// POWER OFF
#define POWER_OFF_LOW()  GPIO_ResetBits(POWER_OFF_GPIO, POWER_OFF_PIN)
#define POWER_OFF_HIGH() GPIO_SetBits(POWER_OFF_GPIO, POWER_OFF_PIN)

// operation modes for AD5320 DAC device
#define DAC_NM          0b00  /* 0b0000000 - Normal Mode */
#define DAC_PD1         0b01  /* first Power Down mode: 1 kOhm to GND*/
#define DAC_PD2         0b10  /* second Power Down mode: 100 kOhm to GND*/
#define DAC_TS          0b11  /* three state mode */

typedef struct {
//extern struct {
    uint32_t freq; 
    uint32_t start;
    uint32_t finish; 
    } LoggerSettings;

typedef struct {
    uint32_t start; 
    float prescaler;
    } TimeSettings;

typedef struct {
    char    model[32];
    char    serial[32];
    char    mcu[32];
    char    adc[32];
    char    flash[32];
    char    sensorT[32];
    char    sensorP[32];
    char    sensorH[32];
    // temperature
    char    A1[32];
    char    A2[32];
    char    A3[32];
    char    R0[32];
    // pressure
    char    TD[32];
    char    alpha[32];
    char    zero[32];
    // turbidity 10 mA
    char    K1[32];
    char    X01[32];
    char    r1[32];
    char    dt1[32];
    char    dv1[32];
    // turbidity 80 mA
    char    K2[32];
    char    X02[32];
    char    r2[32];
    char    dt2[32];
    char    dv2[32];
    } DeviceInfo;

//+++++++++++++++++++++++++++++++++++++++++++++++
void Blink_Toggle();
void vAlarmTask(void *vpars);

int         DetermineGain(int CHAN);
int         MakePreciseMeasurement(int CHAN, int GAIN, int NUM, uint8_t FREQ, uint32_t VREF);
void        MakeMeasurement(void);

void        vBlinkTask(void *vpars);
void        SetFreqLow(void);
void        SetFreqHigh(void);
void        MY_GPIO_Init(void);
void        MY_SPI_Init(uint8_t CLK);
void        MY_SPI_DeInit(void);
void        Set_DAC_Output(uint8_t mode, uint16_t value);
uint8_t     RTC_Init(void);
uint8_t     MY_SPI_SendByte(uint8_t byte);
uint16_t    MU_SPI_SendHalfWord(uint16_t HalfWord);
uint32_t    GetTime(void);
uint32_t    SetWakeUp(uint8_t num_byte);

#endif

