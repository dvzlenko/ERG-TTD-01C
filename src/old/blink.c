#include "blink.h"

void Blink_Init()
{
	GPIO_InitTypeDef sGPIOinit;

	RCC_APB2PeriphClockCmd(SYS_LED_RCC, ENABLE);
	sGPIOinit.GPIO_Mode     = GPIO_Mode_Out_PP;
	sGPIOinit.GPIO_Speed    = GPIO_Speed_10MHz;
	sGPIOinit.GPIO_Pin      = SYS_LED_PIN;
	GPIO_Init(SYS_LED_GPIO, &sGPIOinit);
	GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN);
}

void vBlinkTask(void *vpars)
{
	Blink_Init();
	while (1) {
		Blink_Toggle();
		vTaskDelay(200);
	}
}

static inline void Blink_Toggle()
{
	if (GPIO_ReadOutputDataBit(SYS_LED_GPIO, SYS_LED_PIN))
		GPIO_ResetBits(SYS_LED_GPIO, SYS_LED_PIN);
	else
		GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN);
}

