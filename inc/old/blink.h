#ifndef __blink_h
#define __blink_h

#include "stm32f10x_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#define SYS_LED_RCC     RCC_APB2Periph_GPIOA
#define SYS_LED_GPIO    GPIOA
#define SYS_LED_PIN     GPIO_Pin_8

void Blink_Init();
void vBlinkTask(void *vpars);

static inline void Blink_Toggle();

#endif
