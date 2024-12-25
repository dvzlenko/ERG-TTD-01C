#ifndef __alarm_h
#define __alarm_h

#include "stm32f10x_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"

void vAlarmTask(void *vpars);

#endif

