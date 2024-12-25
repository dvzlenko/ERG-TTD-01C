#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "stdlib.h"
#include "cdcio.h"
#include "FreeRTOS.h"
#include "strtok.h"
#include "flash.h"
#include "logger.h"

struct chat_rw_funcs cdc_rw = {
    .read  = cdc_read,
    .write = cdc_write,
    };

extern LoggerSettings  loggerSettings;

int main(void) {
    flash_load();
    // Set SYSCLK to 8 MHz
    SetFreqLow();
    // Initialize the GPIO periphery
    MY_GPIO_Init();
    // Put TPS63001 to the normal operation mode
    TPS_PSM_HIGH();
    // Set IR LED PIN HIGH - swith it off
    LED_ON_HIGH();
    // Set DAC output to zero
    MY_SPI_Init(0);
    Set_DAC_Output(DAC_PD1, 0);
    MY_SPI_DeInit();
    // Activate RTC and Backup Domain access
    RTC_Init();
    // Below cycle is just for illumination
    for (uint32_t i = 0; i < 1000; i++) {
        Blink_Toggle();
        }

    while (1) {
        if (GPIO_ReadInputDataBit(USB_DTC_GPIO, USB_DTC_PIN)) {
            // Put TPS63001 into the Normal Mode
            TPS_PSM_HIGH();
            // Set SYSCLK to 72 MHz
            SetFreqHigh();            
            
            // USB configuration
            Set_USBClock();
            USB_Interrupts_Config();
            USB_Init();

            // Task scheduler
            portBASE_TYPE err;
            err = xTaskCreate(vBlinkTask, "blink", 64, NULL,    tskIDLE_PRIORITY + 1, NULL );
            err = xTaskCreate(vChatTask,  "chat", 256, &cdc_rw, tskIDLE_PRIORITY + 1, NULL );
            err = xTaskCreate(vAlarmTask, "alarm", 64, NULL,    tskIDLE_PRIORITY + 1, NULL );
            vTaskStartScheduler();

            while(1);
            }
        else {
            uint32_t tm = 0;
            // Pull the PSM DOWN
            TPS_PSM_LOW();
            // Switch ON the SYS_LED
            //SYS_LED_HIGH();
            // Get the Schedule
            ReadProgramSettings();
            // GetTime
            tm = GetTime();
            // decide, whether it should measure anything?
            if ((tm > (loggerSettings.start - 2)) & (tm < (loggerSettings.finish + 2))) {
                MakeMeasurement();
                }
            // Set the next WAKEUP time
            SetWakeUp(24);
            // Switch OFF the SYS_LED
            SYS_LED_LOW();
            }
        // power off
        POWER_OFF_HIGH();
        }
    }

void vApplicationStackOverflowHook(xTaskHandle xTask, signed portCHAR *pcTaskName) {
    while(1);
    }

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
/* User can add his own implementation to report the file name and line number,
 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
        ;
}
#endif