#include "alarm.h"
#include "logger.h"


void vAlarmTask(void *vpars) {
    extern LoggerSettings loggerSettings;
	uint32_t tm, wkptm, delay, freq;
	// enable access to RTC
	RTC_Init();
	/// reset the operation correction number
	SaveNumberCorrection(0);
    // get program settings
    ReadProgramSettings();
    // delay definition
    if (loggerSettings.freq < 60)
        delay = 120;
    else
        delay = freq;

	while (1) {
		tm = RTC_GetCounter();
		wkptm = SetWakeUp();
        if ((wkptm < tm + delay) & (wkptm != 0))
            RTC_SetAlarm(wkptm + delay);
        POWER_OFF_HIGH();
        vTaskDelay(1000);
	}
}

