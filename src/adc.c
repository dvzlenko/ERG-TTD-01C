#include "logger.h"

/* Resets the ADC  */
void ADC_Reset(void) {
    uint8_t i, tmp;
    // Select the ADC: Chip Select low
    ADC_CS_LOW();
    // Send 32 ones to ADC
    for (i = 0; i < 4; i++)
        tmp = MY_SPI_SendByte(0xFF);
    // Deselect the ADC: Chip Select high
    ADC_CS_HIGH();
    }

/** The next two functions operate with ADC779X CONFIGURATION REGISTER
  * CR: 00 X  X  0  XXX  00  X  X   0 XXX
  *        BO UB     G       RD BUF   CH
  * BO - Set to enale the Burn Out Current (1<<6)
  * UB - Clear to enable the Bipolar Mode (0<<5)
  * G  - Gain settings:
  *    000 (0x00) -   x1 
  *    001 (0x01) -   x2 
  *    010 (0x02) -   x4
  *    011 (0x03) -   x8
  *    100 (0x04) -  x16
  *    101 (0x05) -  x32
  *    110 (0x06) -  x64
  *    111 (0x07) - x128
  * RD  - Referense Detect function (1<<5). Set enables this function and enables NOREF bit functioning in STATUS REGISTER.
  * BUF - Set enables buferization (1<<4). The Manual said that if BUF is set: "...the voltage on any input pin must be limited to 100 mV within the power supply rails." !!!
  * CH  - Defines the active Analog Input Channels:
  *            Hz     ms
  *    000 (0x00) - AIN1(+) - AIN1(-)
  *    001 (0x01) - AIN2(+) - AIN2(-)
  *    010 (0x02) - AIN3(+) - AIN3(-)
  *    011 (0x03) - AIN1(-) - AIN1(-)  What The Fuck???
  *    100 (0x04) - RESERVED 
  *    101 (0x05) - RESERVED 
  *    110 (0x06) - RESERVED 
  *    111 (0x07) - AVdd monitor.  
  - pulls ADC_CS LOW
  */
uint16_t ADC_ReadConfig(void) {
    uint8_t tmp0, tmp1;
    // Select the FLASH: Chip Select low
    ADC_CS_LOW();
    // Send the 0b01010000 byte to read the ADC CR
    tmp0 = MY_SPI_SendByte((ADC_RD | ADC_CONFIG_REG));
    // Send the DUMMY_BYTE to obtain the CR MSB byte
    tmp0 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Send the DUMMY_BYTE to obtain the CR LSB byte
    tmp1 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Deselect the AD: Chip Select high
    ADC_CS_HIGH();
    return (tmp0 << 8) | tmp1;
    }

/* - pulls ADC_CS LOW */
void ADC_SetConfig(uint16_t CONF) {
    uint8_t tmp;
    // Select the FLASH: Chip Select low
    ADC_CS_LOW();
    // Send the 0b00001000 byte to write the ADC MR
    tmp = MY_SPI_SendByte((ADC_WR | ADC_CONFIG_REG));
    // Send the MSB byte of MR
    tmp = MY_SPI_SendByte(CONF>>8);
    // Send the LSB byte of MR
    tmp = MY_SPI_SendByte(CONF);
    // Deselect the AD: Chip Select high
    ADC_CS_HIGH();
    }


/** The next two functions operate with ADC779X MODE REGISTER
  * MR: X X X    X    00000000  X X X X 
  *      MD     PSW    zeros      FS
  * MD (MODE):
  *    000 (0<<5 = 0x00) - continuous conversion mode 
  *    001 (1<<5 = 0x20) - single conversion mode 
  *    010 (2<<5 = 0x40) - idle mode 
  *    011 (3<<5 = 0x60) - power down mode 
  *    100 (4<<5 = 0x80) - internal zero calibration mode 
  *    101 (5<<5 = 0xA0) - internal full-scale calibration mode 
  *    110 (6<<5 = 0xC0) - system zero calibration mode 
  *    111 (7<<5 = 0xE0) - system full-scale calibration mode 
  *
  * FS (Filter Update):
  *                   Hz     ms
  *    0000 (0x00) - RESERVED 
  *    0001 (0x01) - 470      4
  *    0010 (0x02) - 242      8
  *    0011 (0x03) - 123     16
  *    0100 (0x04) -  62     32
  *    0101 (0x05) -  50     40
  *    0110 (0x06) -  39     48
  *    0111 (0x07) -  33.2   60
  *    1000 (0x08) -  19.6  101
  *    1001 (0x09) -  16.7  120 
  *    1010 (0x0A) -  16.7  120 
  *    1011 (0x0B) -  12.5  160
  *    1100 (0x0C) -  10    200
  *    1101 (0x0D) -  8.33  240
  *    1110 (0x0E) -  6.25  320
  *    1111 (0x0F) -  4.17  480
  - pulls ADC_CS LOW
  */
uint16_t ADC_ReadMode(void) {
    uint8_t tmp0, tmp1;
    // Select the FLASH: Chip Select low
    ADC_CS_LOW();
    // Send the 0b01001000 byte to push ADC place the MR into the DR
    tmp0 = MY_SPI_SendByte((ADC_RD | ADC_MODE_REG));
    // Send the DUMMY_BYTE to obtain the MR MSB byte
    tmp0 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Send the DUMMY_BYTE to obtain the MR LSB byte
    tmp1 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Deselect the AD: Chip Select high
    ADC_CS_HIGH();
    return (tmp0 << 8) | tmp1;
    }

/* - pulls ADC_CS LOW */
void ADC_SetMode(uint16_t MODE) {
    uint8_t tmp;
    // Select the FLASH: Chip Select low
    ADC_CS_LOW();
    // Send the 0b00001000 byte to write the ADC MR
    tmp = MY_SPI_SendByte((ADC_WR | ADC_MODE_REG));
    // Send the MSB byte of MR
    tmp = MY_SPI_SendByte(MODE>>8);
    // Send the LSB byte of MR
    tmp = MY_SPI_SendByte(MODE);
    // Deselect the AD: Chip Select high
    ADC_CS_HIGH();
    }

/** Sends a byte 0b01000000 through the SPI interface to the ADC.
  * STATUS REGISTER value:
  * SR:  X   X    X    0  1   X X X
  *     RDY ERR NOREF    SR3   CH
  * RDY   - Ready Bit. Set when the DR is not ready
  * ERR   - Set when the result in DR is clamped to 0 or 1
  * NOREF - Set when there is a problem with the reference voltage
  * SR3   - Is set to 1 for ADC99 and to zero for ADC98. The same is for ADC94 and ADC95?
  * CH    - Channel selected: 000 - AIN1
  *                           001 - AIN2
  *                           010 - AIN3
  - pulls ADC_CS LOW
  */
uint8_t ADC_ReadStatus(void) {
    uint8_t tmp;
    // Select the FLASH: Chip Select low
    ADC_CS_LOW();
    // Send the 0b01000000 byte to push ADC place the SR into the DR
    tmp = MY_SPI_SendByte((ADC_RD | ADC_STATUS_REG));
    // Send the DUMMY_BYTE to obtain the SR from DR
    tmp = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Deselect the AD: Chip Select high
    ADC_CS_HIGH();
    return tmp;
    }

/* Cheks the ADC status without touching the CS pin 
   - does not pull ADC_CS LOW */
uint8_t ADC_CheckStatus(void) {
    uint8_t tmp;
    // Send the 0b01000000 byte to push ADC place the SR into the DR
    tmp = MY_SPI_SendByte((ADC_RD | ADC_STATUS_REG));
    // Send the DUMMY_BYTE to obtain the SR from DR
    tmp = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    return tmp;
    }

/* Reads ID 
   - pulls AD_CS LOW*/
uint8_t ADC_ReadID(void) {
    uint8_t tmp;
    // Select the ADC: Chip Select low
    ADC_CS_LOW();
    // Send the 0b01010000 byte to push ADC place the ID into the DR
    tmp = MY_SPI_SendByte((ADC_RD | ADC_ID_REG));
    // Send the DUMMY_BYTE to obtain the ID from DR
    tmp = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Deselect the ADC: Chip Select high
    ADC_CS_HIGH();
    return tmp;
    }

/* Writes to OFFSET REGISTER 
   - pulls AD_CS LOW */
void ADC_WriteOFF(uint32_t OFF_REG_VAL) {
    uint8_t tmp;
    // Select the ADC: Chip Select low
    ADC_CS_LOW();
    // Send the a command to read Offset Register
    tmp = MY_SPI_SendByte((ADC_WR | ADC_OFF_REG));
    // Send the OFF_REG_VAL
    tmp = MY_SPI_SendByte(OFF_REG_VAL>>16);
    tmp = MY_SPI_SendByte(OFF_REG_VAL>>8);
    tmp = MY_SPI_SendByte(OFF_REG_VAL);
    // Deselect the ADC: Chip Select high
    ADC_CS_HIGH();
    }

/* Reads OFFSET REGISTER 
   - pulls AD_CS LOW */
uint32_t ADC_ReadOFF(void) {
    uint8_t tmp0, tmp1, tmp2;
    // Select the ADC: Chip Select low
    ADC_CS_LOW();
    // Send the a command to read Offset Register
    tmp0 = MY_SPI_SendByte((ADC_RD | ADC_OFF_REG));
    // Send the DUMMY_BYTE to obtain the 1-st bit of the OFF_REG from DR
    tmp0 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Send the DUMMY_BYTE to obtain the 2-nd bit of the OFF_REG from DR
    tmp1 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Send the DUMMY_BYTE to obtain the 3-rd bit of the OFF_REG from DR
    tmp2 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Deselect the ADC: Chip Select high
    ADC_CS_HIGH();
    return (tmp0 << 16) | (tmp1 << 8) | tmp2;
    }

/* Reads FULL SCALE REGISTER 
   - pulls AD_CS LOW */
uint32_t ADC_ReadFS(void) {
    uint8_t tmp0, tmp1, tmp2;
    // Select the ADC: Chip Select low
    ADC_CS_LOW();
    // Send the a command to read Offset Register
    tmp0 = MY_SPI_SendByte((ADC_RD | ADC_FS_REG));
    // Send the DUMMY_BYTE to obtain the 1-st bit of the OFF_REG from DR
    tmp0 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Send the DUMMY_BYTE to obtain the 2-nd bit of the OFF_REG from DR
    tmp1 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Send the DUMMY_BYTE to obtain the 3-rd bit of the OFF_REG from DR
    tmp2 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Deselect the ADC: Chip Select high
    ADC_CS_HIGH();
    return (tmp0 << 16) | (tmp1 << 8) | tmp2;
    }

/* Reads the single conversion result from ADC9X ADC 
   - pulls AD_CS LOW */
uint32_t ADC_ReadDataSingle(uint8_t FS) {
    uint8_t tmp0, tmp1, tmp2;
    // Select the ADC: Chip Select low
    ADC_CS_LOW();
    // Send the command to enable a single conversion mode
    tmp0 = MY_SPI_SendByte((ADC_WR | ADC_MODE_REG));
    tmp0 = MY_SPI_SendByte(0x20);
    tmp0 = MY_SPI_SendByte(FS);
    // Wait for the SR RDY bit to clear
    while (ADC_CheckStatus() & 0x80);
    // Send the command to push ADC place the ID into the DR
    tmp0 = MY_SPI_SendByte((ADC_RD | ADC_DATA_REG));
    // Send the DUMMY_BYTE to obtain the 1-st bit of the DATA from DR
    tmp0 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Send the DUMMY_BYTE to obtain the 2-nd bit of the DATA from DR
    tmp1 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Send the DUMMY_BYTE to obtain the 3-rd bit of the DATA from DR
    tmp2 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
    // Deselect the ADC: Chip Select high
    ADC_CS_HIGH();
    return (tmp0 << 16) | (tmp1 << 8) | tmp2;
    }

/* Performs Internal Zero-Scale and then Internam Full-Scale
   calibration cycles for a given ADC GAIN setting. 
   As far as this cycle is not available for GAIN128, 
   this gain was depricated allover the project.
   GAIN must be programmed before call of this function.
   The ADC Update Rate was chosen as ADC_MR_FS_16 for accuracy
   - pulls AD_CS LOW */
void ADC_MakeCalibration(void) {
    // Select the ADC
    ADC_CS_LOW();
    // Zero Calibration
    MY_SPI_SendByte((ADC_WR | ADC_MODE_REG));
    MY_SPI_SendByte(0x80);
    MY_SPI_SendByte(ADC_MR_FS_16);
    while (ADC_CheckStatus() & 0x80);
    // Full-Scale Calibration
    MY_SPI_SendByte((ADC_WR | ADC_MODE_REG));
    MY_SPI_SendByte(0xA0);
    MY_SPI_SendByte(ADC_MR_FS_16);
    while (ADC_CheckStatus() & 0x80);
    // Deselect the ADC
    ADC_CS_HIGH();
    }

/* Continuously reads the conversion results from ADC9X ADC 
   - pulls AD_CS LOW */
void ADC_ReadDataCont(uint32_t* data, uint16_t num, uint8_t FS) {
    uint8_t i, tmp0, tmp1, tmp2;
    // Select the ADC
    ADC_CS_LOW();
    // Wait a bit!!!!
    while (ADC_CheckStatus() & 0x80);
    // Send the command to enable a continuous conversion mode
    tmp0 = MY_SPI_SendByte((ADC_WR | ADC_MODE_REG));
    tmp0 = MY_SPI_SendByte(0x00);
    tmp0 = MY_SPI_SendByte(FS);
    for (i = 0; i < num; i++) {
        // Wait for the SR RDY bit to clear
        while (ADC_CheckStatus() & 0x80);
        // Send the 0x58 byte to declare read from DATA REGISTER
        tmp0 = MY_SPI_SendByte(ADC_RD | ADC_DATA_REG);
        // Send the DUMMY_BYTE to obtain the 1-st bit of the DATA from DR
        tmp0 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
        // Send the DUMMY_BYTE to obtain the 2-nd bit of the DATA from DR
        tmp1 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
        // Send the DUMMY_BYTE to obtain the 3-rd bit of the DATA from DR
        tmp2 = MY_SPI_SendByte(SPI_DUMMY_BYTE);
        // Fill the buffer data with the converted value
        *data = ((tmp0 << 16) | (tmp1 << 8) | tmp2);
        data++;
        }
    // Send the command to put ADC in Idle Mode
    tmp0 = MY_SPI_SendByte((ADC_WR | ADC_MODE_REG));
    tmp0 = MY_SPI_SendByte(0x40);
    tmp0 = MY_SPI_SendByte(FS);
    // Deselect the ADC
    ADC_CS_HIGH();
    }

/* SOME CODE BY PITON IS BELOW !!! */
struct adc_chan {
	uint32_t	rcc;
	GPIO_TypeDef 	*gpio;
	uint16_t	pin;
    };

static struct adc_chan chan_list[] = {
	{	/* channel 0 */
		.rcc = 	RCC_APB2Periph_GPIOA,
		.gpio =	GPIOA,
		.pin =	GPIO_Pin_0,
	},
	{	/* channel 1 */
		.rcc = 	RCC_APB2Periph_GPIOA,
		.gpio =	GPIOA,
		.pin =	GPIO_Pin_1,
	},
	{	/* channel 2 */
		.rcc = 	RCC_APB2Periph_GPIOA,
		.gpio =	GPIOA,
		.pin =	GPIO_Pin_2,
	},
	{	/* channel 3 */
		.rcc = 	RCC_APB2Periph_GPIOA,
		.gpio =	GPIOA,
		.pin =	GPIO_Pin_3,
	},
	{	/* channel 4 */
		.rcc = 	RCC_APB2Periph_GPIOA,
		.gpio =	GPIOA,
		.pin =	GPIO_Pin_4,
	},
	{	/* channel 5 */
		.rcc = 	RCC_APB2Periph_GPIOA,
		.gpio =	GPIOA,
		.pin =	GPIO_Pin_5,
	},
	{	/* channel 6 */
		.rcc = 	RCC_APB2Periph_GPIOA,
		.gpio =	GPIOA,
		.pin =	GPIO_Pin_6,
	},
	{	/* channel 7 */
		.rcc = 	RCC_APB2Periph_GPIOA,
		.gpio =	GPIOA,
		.pin =	GPIO_Pin_7,
	},
	{	/* channel 8 */
		.rcc = 	RCC_APB2Periph_GPIOB,
		.gpio =	GPIOB,
		.pin =	GPIO_Pin_0,
	},
	{	/* channel 9 */
		.rcc = 	RCC_APB2Periph_GPIOB,
		.gpio =	GPIOB,
		.pin =	GPIO_Pin_1,
	},
	{	/* channel 10 */
		.rcc = 	RCC_APB2Periph_GPIOC,
		.gpio =	GPIOC,
		.pin =	GPIO_Pin_0,
	},
	{	/* channel 11 */
		.rcc = 	RCC_APB2Periph_GPIOC,
		.gpio =	GPIOC,
		.pin =	GPIO_Pin_1,
	},
	{	/* channel 12 */
		.rcc = 	RCC_APB2Periph_GPIOC,
		.gpio =	GPIOC,
		.pin =	GPIO_Pin_2,
	},
	{	/* channel 13 */
		.rcc = 	RCC_APB2Periph_GPIOC,
		.gpio =	GPIOC,
		.pin =	GPIO_Pin_3,
	},
	{	/* channel 14 */
		.rcc = 	RCC_APB2Periph_GPIOC,
		.gpio =	GPIOC,
		.pin =	GPIO_Pin_4,
	},
	{	/* channel 15 */
		.rcc = 	RCC_APB2Periph_GPIOC,
		.gpio =	GPIOC,
		.pin =	GPIO_Pin_5,
	},
};

int adc_vals[ARRAY_SIZE(chan_list)];

xSemaphoreHandle adc_vals_lock = NULL;

static volatile int chmask = 0;

void adc_init(int chans) {
	int i;
	uint32_t tmp;
	GPIO_InitTypeDef sGPIOinit;
	ADC_InitTypeDef sADCinit;

	chmask = chans & ((1 << ARRAY_SIZE(chan_list)) - 1);

	/* set  adc clock to 72/6=12MHz */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	/* enable  ADC and input GPIOs clock */
	tmp = RCC_APB2Periph_ADC1;
	for (i = 0; i < ARRAY_SIZE(chan_list); i++)
		if (chans & (1 << i))
			tmp |= chan_list[i].rcc;
	RCC_APB2PeriphClockCmd(tmp, ENABLE);

	ADC_DeInit(ADC1);

	/* configure ADC inputs */
	sGPIOinit.GPIO_Speed = GPIO_Speed_10MHz;
	sGPIOinit.GPIO_Mode = GPIO_Mode_AIN;
	for (i = 0; i < ARRAY_SIZE(chan_list); i++)
		if (chans & (1 << i)) {
			sGPIOinit.GPIO_Pin = chan_list[i].pin;
			GPIO_Init(chan_list[i].gpio, &sGPIOinit);
		}

	/* adc setup */
	sADCinit.ADC_Mode = ADC_Mode_Independent;
	sADCinit.ADC_ScanConvMode = DISABLE;
	sADCinit.ADC_ContinuousConvMode = DISABLE;
	sADCinit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	sADCinit.ADC_DataAlign = ADC_DataAlign_Right;
	sADCinit.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1,&sADCinit);

	ADC_Cmd(ADC1,ENABLE);

	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) != SET)
		;
    }

int adc_get(int i) {
	if (!(chmask & (1 << i)))
		return -1;

	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_RegularChannelConfig(ADC1, i, 1, ADC_SampleTime_71Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		;
	return ADC_GetConversionValue(ADC1);
    }

int adc_get_stored(int i) {
	if (i >= ARRAY_SIZE(chan_list))
		return 0;
	return adc_vals[i];
    }

void vADCTask(void* vpars) {
	int i, j, ref;
	while (1) {
		vTaskDelay(100);
	    }
    }

