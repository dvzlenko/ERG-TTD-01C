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
    // Reset if necessary
    //if (GetReset() != 1) {
    //    SaveReset(1);
    //    SYS_Reset();
    //    }
    //else {
    //    SaveReset(0);
    //    }
    // Set SYSCLK to 2MHz
    SetFreqLow();
    // Initialize the GPIO periphery
    MY_GPIO_Init();
    // Activate RTC and Backup Domain access
    RTC_Init();

    while (1) {
        // Verify if USB cable is attached 
        if (GPIO_ReadInputDataBit(USB_DTC_GPIO, USB_DTC_PIN)) {
            // Put TPS63001 to the normal operation mode
            TPS_PSM_HIGH();
            // Set SYSCLK to 72MHz
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
            // Blink cycle
            for (uint32_t i = 0; i < 100; i++) {Blink_Toggle();}
            // Get the Schedule
            ReadProgramSettings();
            // GetTime
            tm = GetTime();
            // decide, whether it should measure anything?
            if ((tm > (loggerSettings.start - 2)) & (tm < (loggerSettings.finish + 2))) {
                MakeMeasurement();
                }
            // Set the next WAKEUP time
            SetWakeUp(NUM_BYTES);
            // Blink cycle
            for (uint32_t i = 0; i < 5; i++) {Blink_Toggle();}
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
