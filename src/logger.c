#include "logger.h"

/*  
 Here are some settings for PWM
 PWM_COUNT = 18000 -> 100Hz at 72MHz and prescaler 19
 PWM_COUNT =   500 -> 100Hz at  8MHz and prescaler 19
 PWM_COUNT = 36000 -> 500Hz at 72MHz and prescaler  1
 PWM_COUNT =  2500 -> 500Hz at  8MHz and prescaler 19
 PWM_COUNT = 18000 ->  1kHz at 72MHz and prescaler  1
 PWM_COUNT =   500 ->  1kHz at  8MHz and prescaler  1
*/
//int PWM_COUNT     = 18000; 
//int TIM_PRESCALER =     1;

/*  
 Here are some settings for IR LED:
 DAC_CODE  =  273 -> 10 mA
 DAC_CODE  =  546 -> 20 mA
 DAC_CODE  = 1092 -> 40 mA
 DAC_CODE  = 2184 -> 80 mA 
*/
int DAC_CODE_1 =  273;
int DAC_CODE_2 = 1092;


// structures definition
LoggerSettings  loggerSettings;
TimeSettings    timeSettings;
DeviceInfo      deviceInfo;

/*+++++++++++++++++++++++++++++++++++++++++++++*/
/*                                             */
/*   FUNCTIONS THAT OPERATE TIME, WAKEUP ETC   */
/*                                             */
/*+++++++++++++++++++++++++++++++++++++++++++++*/

uint8_t RTC_Init(void) { 
    // Enable clock and power for RTC
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // Enable access to the BKP domain
    PWR_BackupAccessCmd(ENABLE);
    // Enable RTC in case it is not active
    if((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN) {
        // reset the content of the BKP registers
        RCC_BackupResetCmd(ENABLE);
        RCC_BackupResetCmd(DISABLE);
        // switch to the LSE clock
        RCC_LSEConfig(RCC_LSE_ON);
        while ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY) {}
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        // default prescaler for seconds 
        RTC_SetPrescaler(0x7FFF);
        // enable RTC
        RCC_RTCCLKCmd(ENABLE);
        // waiting for synchronization
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        return 1;
    }
    // Configure the TAMPER pin as an ALARM output
    BKP_RTCOutputConfig(BKP_RTCOutputSource_Alarm);
    //BKP_RTCOutputConfig(BKP_RTCOutputSource_Second);
    return 0;
}

/* returns corrected time since last calibration in seconds */
uint32_t GetTime(void) {
    uint32_t epoch, dT1, dT2;
    // native RTC seconds counter from MCU
    epoch = RTC_GetCounter();
    // time settings storebd by MCU
    ReadTimeSettings();
    // convert time according to the identified clock drift
    dT1 = epoch - timeSettings.start;
    dT2 = dT1 * timeSettings.prescaler;
    return timeSettings.start + dT2;
}

/* sets the next wake up time and returns the value of the counter to rise the alarm 
   byte -> number of bytes per measurement */
uint32_t SetWakeUp(uint8_t num_byte) {
    uint32_t address, num, real_alarm_time, real_operation_time, stm_operation_time, stm_alarm_time, stm_time;
    double prescaler;
    // reading of the settings
    ReadProgramSettings();
    ReadTimeSettings();
    address = GetAddress();
    prescaler = timeSettings.prescaler;
    // brick the DEVICE if the schedule is in the past
    if (GetTime() > loggerSettings.finish)
        RTC_SetAlarm(0);
    else {
        // RTC seems to produce the pulse at the END of the corresponding second :( So, I need to extract one second from X_stm_alarm_time
        stm_time = RTC_GetCounter();
        // calculation of the wake up time acounting for the RTC drift
        num = (address / num_byte) + GetNumberCorrection();
        real_alarm_time = loggerSettings.start + loggerSettings.freq * num;
        real_operation_time = real_alarm_time - timeSettings.start;
        stm_operation_time = real_operation_time / prescaler; 
        stm_alarm_time = timeSettings.start + stm_operation_time - 1;
        while (stm_alarm_time < stm_time + 1) {
            stm_alarm_time += loggerSettings.freq;
            SaveNumberCorrection(GetNumberCorrection() + 1);
        }
        RTC_SetAlarm(stm_alarm_time);
        return stm_alarm_time;
    }
}


/*+++++++++++++++++++++++++++++++++++++++++*/
/*                                         */
/*   FUNCTIONS THAT OPERATE GPIO AND SPI   */
/*                                         */
/*+++++++++++++++++++++++++++++++++++++++++*/

/* Initializes the GPIO periphery */
void MY_GPIO_Init(void) {
    // JTAG -- PA15 and PB3 pins REMAP
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    // Init structure
    GPIO_InitTypeDef GPIO_InitStructure;

    // Configure TPS63001 PSM PIN
    RCC_APB2PeriphClockCmd(TPS_PSM_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin     = TPS_PSM_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(TPS_PSM_GPIO, &GPIO_InitStructure);
    // pull in DOWN - to power save mode
    GPIO_ResetBits(TPS_PSM_GPIO, TPS_PSM_PIN);

    // Configure IR_LED PIN
    RCC_APB2PeriphClockCmd(LED_ON_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin     = LED_ON_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(LED_ON_GPIO, &GPIO_InitStructure);
    RCC_APB2PeriphClockCmd(SYS_LED_RCC, ENABLE);
    // pull it UP - to prevent any current through IR LED
    GPIO_SetBits(LED_ON_GPIO, LED_ON_PIN);

    // Configure SYS_LED PIN
    RCC_APB2PeriphClockCmd(SYS_LED_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin      = SYS_LED_PIN;
    GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_Out_PP;
    GPIO_Init(SYS_LED_GPIO, &GPIO_InitStructure);
    // pull it DOWN to switch SYS_LED OFF
    GPIO_ResetBits(SYS_LED_GPIO, SYS_LED_PIN);

    // POWER_OFF PIN
    RCC_APB2PeriphClockCmd(POWER_OFF_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin       = POWER_OFF_PIN;
    GPIO_InitStructure.GPIO_Speed     = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_Out_PP;
    GPIO_Init(POWER_OFF_GPIO, &GPIO_InitStructure);
    // pull it DOWN not to BRICK the MCU
    GPIO_ResetBits(POWER_OFF_GPIO, POWER_OFF_PIN);

    // USB_CABLE_DETECT PIN 
    RCC_APB2PeriphClockCmd(USB_DTC_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin      = USB_DTC_PIN;
    GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USB_DTC_GPIO, &GPIO_InitStructure);
    }

/* Initializes the SPI periphery */
void MY_SPI_Init(uint8_t CLK) {
    // JTAG (PA15 and PB3 pins REMAP
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    // Enable clock for SPI
    RCC_APB2PeriphClockCmd(MY_SPI_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(MY_SPI_CLK, ENABLE);
    // GPIO stuff
    GPIO_InitTypeDef GPIO_InitStructure;
    // Configure MY_SPI pins: SCLK
    GPIO_InitStructure.GPIO_Pin     = MY_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init(MY_SPI_GPIO, &GPIO_InitStructure);
    // Configure MY_SPI pins: MOSI
    GPIO_InitStructure.GPIO_Pin     = MY_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init(MY_SPI_GPIO, &GPIO_InitStructure);
    // Configure MY_SPI pins: MISO
    GPIO_InitStructure.GPIO_Pin     = MY_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
    GPIO_Init(MY_SPI_GPIO, &GPIO_InitStructure);
    // Configure MEM CS PIN pin
    RCC_APB2PeriphClockCmd(MEM_CS_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin     = MEM_CS_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(MEM_CS_GPIO, &GPIO_InitStructure);
    // Configure ADC CS PIN pin
    RCC_APB2PeriphClockCmd(ADC_CS_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin     = ADC_CS_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(ADC_CS_GPIO, &GPIO_InitStructure);
    // Configure DAC CS PIN pin
    RCC_APB2PeriphClockCmd(DAC_CS_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin     = DAC_CS_PIN;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(DAC_CS_GPIO, &GPIO_InitStructure);
    // Pull all CS pins high
    MEM_CS_HIGH();
    ADC_CS_HIGH();
    DAC_CS_HIGH();
    // SPI configuration
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_InitStructure.SPI_Direction  = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode       = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize   = SPI_DataSize_8b;
    if (CLK)
        SPI_InitStructure.SPI_CPOL   = SPI_CPOL_High; // for ADC and sFLASH 
    else
        SPI_InitStructure.SPI_CPOL   = SPI_CPOL_Low;  // for DAC
    SPI_InitStructure.SPI_CPHA       = SPI_CPHA_2Edge;   
    SPI_InitStructure.SPI_NSS        = SPI_NSS_Soft;
    SPI_InitStructure.SPI_FirstBit   = SPI_FirstBit_MSB;
    // SPI DaudRatePrescaler definition depends on the device operation mode
    if (GPIO_ReadInputDataBit(USB_DTC_GPIO, USB_DTC_PIN))
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // PLLCLK -> HCLK -> PCLK2 -> SCK = 48MHz -> 24MHz -> 6MHz -> 1.5MHz 
        //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    else
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // I need it to be as fast as possible in case if MCU operates at 1 MHz
    // Enable SPI 
    SPI_Cmd(MY_SPI, DISABLE);
    SPI_Init(MY_SPI, &SPI_InitStructure);
    SPI_SSOutputCmd(MY_SPI, ENABLE);  // not to write each time ch_high??? WTF? And people say it doesn't work
    SPI_NSSInternalSoftwareConfig(MY_SPI, SPI_NSSInternalSoft_Set); // setting the CS high by default, as I realized, it just for fun :)
    SPI_Cmd(MY_SPI, ENABLE);
    }

/* DeInitializes the SPI peripherals */
void MY_SPI_DeInit() {
    // Disable MY_SPI 
    SPI_Cmd(MY_SPI, DISABLE);
    // SPI clock disable
    //RCC_APB2PeriphClockCmd(MY_SPI_CLK, DISABLE);
    //RCC_APB2PeriphClockCmd(MY_SPI_RCC, DISABLE);
    // Configure all pins used by the SPI as input floating
    GPIO_InitTypeDef GPIO_InitStructure;
    // SCK 
    GPIO_InitStructure.GPIO_Pin  = MY_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(MY_SPI_GPIO, &GPIO_InitStructure);
    // MISO
    GPIO_InitStructure.GPIO_Pin  = MY_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(MY_SPI_GPIO, &GPIO_InitStructure);
    // MOSI
    GPIO_InitStructure.GPIO_Pin  = MY_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(MY_SPI_GPIO, &GPIO_InitStructure);
    // Pull all CS pins high
    MEM_CS_HIGH();
    ADC_CS_HIGH();
    DAC_CS_HIGH();
    }

/* Sends a byte through the SPI interface and return the byte received from the SPI bus 
   - does not pull ADC_CS LOW */
uint8_t MY_SPI_SendByte(uint8_t byte) {
    // Loop while DR register in not emplty
    while (SPI_I2S_GetFlagStatus(MY_SPI, SPI_I2S_FLAG_TXE) == RESET);
    // Send byte through the SPI1 peripheral
    SPI_I2S_SendData(MY_SPI, byte);       
    // Wait to receive a byte       
    while (SPI_I2S_GetFlagStatus(MY_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    // Return the byte read from the SPI bus
    return SPI_I2S_ReceiveData(MY_SPI);
    }
 
/* Sends a Half Word through the SPI interface and return the Half Word received from the SPI bus
   - does not pull ADC_CS LOW */
uint16_t MY_SPI_SendHalfWord(uint16_t HalfWord) {
    /*!< Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(MY_SPI, SPI_I2S_FLAG_TXE) == RESET);
    /*!< Send Half Word through the MEM peripheral */
    SPI_I2S_SendData(MY_SPI, HalfWord);
    /*!< Wait to receive a Half Word */
    while (SPI_I2S_GetFlagStatus(MY_SPI, SPI_I2S_FLAG_RXNE) == RESET);
    /*!< Return the Half Word read from the SPI bus */
    return SPI_I2S_ReceiveData(MY_SPI);
    }

/* toggles the SYS_LED_PIN - for operating LED flashing */
void Blink_Toggle() {
    if (GPIO_ReadOutputDataBit(SYS_LED_GPIO, SYS_LED_PIN))
        GPIO_ResetBits(SYS_LED_GPIO, SYS_LED_PIN);
    else
        GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN);
    }

/* AD5320 DAC control funtion */
void Set_DAC_Output(uint8_t mode, uint16_t value) {
    uint8_t tmp;
    uint16_t command;
    command = (mode << 12) | (value & 0x0FFF);
    // SPI init
    MY_SPI_Init(0);
    // Select the DAC: Chip Select low
    DAC_CS_LOW();
    // Send command to DAC
    tmp = MY_SPI_SendByte(command >> 8);
    tmp = MY_SPI_SendByte(command);
    // Deselect the DAC: Chip Select high
    DAC_CS_HIGH();
    // SPI de init
    MY_SPI_DeInit();
    }


/*+++++++++++++++++++++++++++++++++++++++*/
/*                                       */
/*   FUNCTIONS THAT MAKES MEASUREMENTS   */
/*                                       */
/*+++++++++++++++++++++++++++++++++++++++*/

/* Determning the correct GAIN of the AD7799 ADC 
   It works in bipolar mode!!! */
int DetermineGain(int CHAN) {
    float coef;
    uint32_t COUNT;

    // getting a test count from ADC
    MY_SPI_Init(1);
    ADC_Reset();
    ADC_SetConfig(ADC_CR_BM | ADC_CR_Gain1 | ADC_CR_RD | ADC_CR_BUF | CHAN);
    COUNT = ADC_ReadDataSingle(ADC_MR_FS_50);
    MY_SPI_DeInit();

    // GAIN128 does not allow Internal Full-Scale Calibration in AD7799
    // so, it was deprecated throughout the projet
    if (COUNT == 8388608)
        return ADC_CR_Gain64;
    else {
        coef = COUNT;
        coef = 8388608 / abs(coef - 8388608);
        if (coef > 128)
            return ADC_CR_Gain64;
        else if (coef > 64)
            return ADC_CR_Gain32;
        else if (coef > 32)
            return ADC_CR_Gain16;
        else if (coef > 16)
            return ADC_CR_Gain8;
        else if (coef > 8)
            return ADC_CR_Gain4;
        else if (coef > 4)
            return ADC_CR_Gain2;
        else
            return ADC_CR_Gain1;
        }
    }

/* Make PRECISE measurement in BIPOLAR MODE 
   - CNAN -> ADC channel
   - GAIN -> ADC GAIN
   - NUM  -> number of samples to average
   - FREQ -> ADC frequency
   - VREF -> reference voltage to convert ADC code in nV!!!
*/
int MakePreciseMeasurement(int CHAN, int GAIN, int NUM, uint8_t FREQ, uint32_t VREF) {
    int i;
    long V;
    uint32_t DD[NUM];
    float coef;
    float D1, D2;
    
    MY_SPI_Init(1);
    ADC_Reset();
    ADC_SetConfig(ADC_CR_BM | GAIN | ADC_CR_RD | ADC_CR_BUF | CHAN);
    ADC_ReadDataCont(DD, sizeof(DD) / 4, FREQ);
    MY_SPI_DeInit();
    // for virtual 1.0V of reference voltage    
    coef = (1 << (GAIN >> 8));
    for (i = 0; i < sizeof(DD) / 4; i++) {
        D1 = DD[i];
        D2 += (1000000000 / 8388608.0) * (D1 - 8388608.0) / coef;
    }
    // voltage as INTEGER in the NANO-volts
    coef = NUM;
    V = (VREF / 1000000000) * D2 / coef;
    return V;
    }


/* Makes the MEASUREMENT
   Taking into account the space necessary for date storage, 
   it seems better to measure all of the parameters together
*/
void MakeMeasurement() { 
    int gain, freq, num, res;
    uint32_t address, time, VREF = 3000000000;
    char DATA[4];

    // some settings of the measurement
    freq = ADC_MR_FS_123;
    num  = 64;

    // Get the address in the FLASH
    address = GetAddress();

    // Aquire time
    time = GetTime();
    DATA[ 0] = time >> 24;
    DATA[ 1] = time >> 16;
    DATA[ 2] = time >> 8;
    DATA[ 3] = time;
    // Write the DATA to the FLASH
    MY_SPI_Init(1);
    if ((address & 0x00000FFF) == 0) 
        MEM_EraseSector(address);
    address += MEM_WriteData(DATA, address, sizeof(DATA));
    MY_SPI_DeInit();

    // Aquire temperature
    gain = DetermineGain(ADC_CR_CH2); 
    res  = MakePreciseMeasurement(ADC_CR_CH2, gain, num, freq, VREF);
    DATA[ 0] = res >> 24;
    DATA[ 1] = res >> 16;
    DATA[ 2] = res >> 8;
    DATA[ 3] = res;
    // Write the DATA to the FLASH
    MY_SPI_Init(1);
    if ((address & 0x00000FFF) == 0) 
        MEM_EraseSector(address);
    address += MEM_WriteData(DATA, address, sizeof(DATA));
    MY_SPI_DeInit();

    // Aquire pressure
    gain = DetermineGain(ADC_CR_CH1); 
    res  = MakePreciseMeasurement(ADC_CR_CH1, gain, num, freq, VREF);
    DATA[ 0] = res >> 24;
    DATA[ 1] = res >> 16;
    DATA[ 2] = res >> 8;
    DATA[ 3] = res;
    // Write the DATA to the FLASH
    MY_SPI_Init(1);
    if ((address & 0x00000FFF) == 0) 
        MEM_EraseSector(address);
    address += MEM_WriteData(DATA, address, sizeof(DATA));
    MY_SPI_DeInit();

    // Aquire turbidity at 0 mA - zero current and noise lightening
    gain = DetermineGain(ADC_CR_CH3); 
    res  = MakePreciseMeasurement(ADC_CR_CH3, gain, num, freq, VREF);
    DATA[ 0] = res >> 24;
    DATA[ 1] = res >> 16;
    DATA[ 2] = res >> 8;
    DATA[ 3] = res;
    // Write the DATA to the FLASH
    MY_SPI_Init(1);
    if ((address & 0x00000FFF) == 0) 
        MEM_EraseSector(address);
    address += MEM_WriteData(DATA, address, sizeof(DATA));
    MY_SPI_DeInit();

    // set the IR LED current to 10 mA
    Set_DAC_Output(DAC_NM, DAC_CODE_1);
    LED_ON_LOW();
    // Aquire turbidity at 10 mA
    gain = DetermineGain(ADC_CR_CH3); 
    res  = MakePreciseMeasurement(ADC_CR_CH3, gain, num, freq, VREF);
    DATA[ 0] = res >> 24;
    DATA[ 1] = res >> 16;
    DATA[ 2] = res >> 8;
    DATA[ 3] = res;
    // switch the IR LED off
    MY_SPI_Init(0);
    Set_DAC_Output(DAC_PD1, 0);
    MY_SPI_DeInit();
    // Write the DATA to the FLASH
    MY_SPI_Init(1);
    if ((address & 0x00000FFF) == 0) 
        MEM_EraseSector(address);
    address += MEM_WriteData(DATA, address, sizeof(DATA));
    MY_SPI_DeInit();
    
    // switch TPS63001 to the normal mode
    TPS_PSM_HIGH();
    // set the IR LED current to 40 mA
    Set_DAC_Output(DAC_NM, DAC_CODE_2);
    // Aquire turbidity at 80 mA
    gain = DetermineGain(ADC_CR_CH3); 
    res  = MakePreciseMeasurement(ADC_CR_CH3, gain, num, freq, VREF);
    DATA[ 0] = res >> 24;
    DATA[ 1] = res >> 16;
    DATA[ 2] = res >> 8;
    DATA[ 3] = res;
    // switch the IR LED off
    LED_ON_HIGH();
    Set_DAC_Output(DAC_PD1, 0);
    // switch TPS63001 to the power save mode
    TPS_PSM_LOW();
    // Write the DATA to the FLASH
    MY_SPI_Init(1);
    if ((address & 0x00000FFF) == 0)
        MEM_EraseSector(address);
    address += MEM_WriteData(DATA, address, sizeof(DATA));
    MY_SPI_DeInit();

    // Save the address to BKP
    SaveAddress(address);
    }

/*++++++++++++++++++++++++++++++++++++++*/
/*                                      */
/*   FUNCTIONS THAT OPERATE MCU CLOCK   */
/*                                      */
/*++++++++++++++++++++++++++++++++++++++*/

/* Resets the MCU
   This is a HAL NVIC_SystemReset() function!!! */
void SYS_Reset(void) {
    SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | SCB_AIRCR_SYSRESETREQ_Msk);
    __DSB();
    while(1);
    }


/* switches MCU to 72 MHz on the fly */
void SetFreqHigh(void) {
    // INCRASE THE FLASH LATTENCY
    FLASH_SetLatency(FLASH_Latency_2);
    // SET PLL SOURCE AND MULTIPLIER
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    // ENABLE PLL, WAIT FOR IT TO BE READY, and SET SYSCLK SOURCE AS PLLCLK
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){};
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    // SET HCLK = SYSCLK = 72MHZ
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    // SET PCLK2 = HCLK = 72MHZ
    RCC_PCLK2Config(RCC_HCLK_Div1);
    // SET PCLK1 = HCLK/2 = 36MHZ (maximum available)
    RCC_PCLK1Config(RCC_HCLK_Div2);
    // CORE CLOCK UPDATE
    SystemCoreClockUpdate();
    //USB PIN RESET AND APB CLOCK DISABLE
    usb_dp_pu();
    }

/* switches MCU to 2MHz on the fly */
void SetFreqLow(void) {
    // SET HSE AS SYSCLK SOURCE, 8MHz
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
    // SET HCLK = SYSCLK / 4 = 2MHz
    RCC_HCLKConfig(RCC_SYSCLK_Div4);
    // SET PCLK1 = HCLK = 2MHZ
    RCC_PCLK1Config(RCC_HCLK_Div1);
    // SET PCLK2 = HCLK = 2MHZ
    // TODO Check if this calculations are valid!!! Refer to the PWM settings derived for ERG-CTD series
    RCC_PCLK2Config(RCC_HCLK_Div1);
    // DISABLE PLL
    RCC_PLLCmd(DISABLE);
    // DECREASE THE FLASH LATTENCY
    FLASH_SetLatency(FLASH_Latency_0);
    // CORE CLOCK UPDATE
    SystemCoreClockUpdate();
    }

/*++++++++++++*/
/*            */
/*   VTASKS   */
/*            */
/*++++++++++++*/

/* System LED blink task */
void vBlinkTask(void *vpars) {
    while (1) {
        Blink_Toggle();
        vTaskDelay(200);
        }
    }

/* WakeUp move forward task */
void vAlarmTask(void *vpars) {
    uint32_t tm, wkptm, delay, corr;
    /// reset the operation correction number
    SaveNumberCorrection(0);
    // get program settings
    ReadProgramSettings();
    // delay definition
    if (loggerSettings.freq < 60)
        delay = 120;
    else
        delay = loggerSettings.freq;

    
    wkptm = SetWakeUp(NUM_BYTES);
    while (1) {
        tm = GetTime();
        if ((wkptm < tm + delay) & (wkptm  != 0)) {
            corr = GetNumberCorrection();
            SaveNumberCorrection(corr + 1);
            }
        wkptm = SetWakeUp(NUM_BYTES);
        POWER_OFF_HIGH();
        vTaskDelay(1000);
        }
    }

