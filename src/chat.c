#include "chat.h"

// structures definition
extern LoggerSettings  loggerSettings;
extern TimeSettings    timeSettings;
extern DeviceInfo      deviceInfo;

char data[SECTOR_SIZE];

enum {
    CMD_HELP = 0,
    CMD_Hello,
    CMD_SetDevInfo,
    CMD_SetTimePrescaler,
    CMD_SetTime,
    CMD_GetTime,
    CMD_SetProgramm,
    CMD_GetProgramm,
    CMD_SendDataToSTM,
    CMD_SendDataToX86,
    CMD_GetData,
    CMD_TestSPI,
    CMD_sleep,

    CMD_LAST
};

char *cmd_list[CMD_LAST] = {
    [CMD_HELP]             = "help",
    [CMD_Hello]            = "hello",
    [CMD_SetDevInfo]       = "SetDevInfo",
    [CMD_SetTimePrescaler] = "SetTimePrescaler",
    [CMD_SetTime]          = "SetTime",
    [CMD_GetTime]          = "GetTime",
    [CMD_SetProgramm]      = "SetProgramm",
    [CMD_GetProgramm]      = "GetProgramm",
    [CMD_SendDataToSTM]    = "SendDataToSTM",
    [CMD_SendDataToX86]    = "SendDataToX86",
    [CMD_GetData]          = "get",
    [CMD_TestSPI]          = "test",
    [CMD_sleep]            = "sleep",
    
};

void vChatTask(void *vpars)
{
    char s[64];
    char cmd[64];
    char *c;
    char *tk;
    int i = 0;
    int echo = 1;
    struct chat_rw_funcs *rw = vpars;

    while (1) {
        memset(cmd, 0, sizeof(cmd));
        c = cmd;

        while (1) {
            i = rw->read(c, 1);
            if (i) {
                if (echo)
                    rw->write(c, 1);
            } else {
                vTaskDelay(10);
                continue;
            }
            if (*c == '\r') {
                if (echo)
                    rw->write("\n", 1);
                break;
            }
            if (*c == 8) { /* backspace */
                *c = 0;
                if (c > cmd)
                    c -= 1;
                continue;
            }
            if (c + 1 < cmd + sizeof(cmd))
                c += 1;
        };

        sniprintf(s, sizeof(s), "\r\n");
        tk = _strtok(cmd, " \n\r");

        if (strcmp(tk, cmd_list[CMD_HELP]) == 0) {
            int i;

            for (i = 0; i < CMD_LAST; i++) {
                char *_s = cmd_list[i];

                rw->write(_s, strlen(_s));
                rw->write("\r\n", 2);
            }

        } else if (strcmp(tk, cmd_list[CMD_Hello]) == 0) {
            uint32_t coef;
            // hello reply, just a huge and unique number
            cdc_write_buf(&cdc_out, "167321907\r\n", 0, 1);
            // device info
            ReadDevInfo();
            // print it out
            cdc_write_buf(&cdc_out, deviceInfo.model,   32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.serial,  32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.mcu,     32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.adc,     32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.flash,   32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.sensorT, 32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.sensorP, 32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.sensorH, 32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.A1,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.A2,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.A3,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.R0,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.TD,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.alpha,   32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.zero,    32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.K1,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.X01,     32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.r1,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.dt1,     32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.dv1,     32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.K2,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.X02,     32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.r2,      32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.dt2,     32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            cdc_write_buf(&cdc_out, deviceInfo.dv2,     32, 1);
            cdc_write_buf(&cdc_out, "\r\n", 0, 1);
            // OK
            sniprintf(s, sizeof(s), "OK\n\r");

        } else if (strcmp(tk, cmd_list[CMD_SetDevInfo]) == 0) {
            uint8_t i, rs;
            ReadDevInfo();
            /* get the DEVICE model */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.model[rs], 32);
            /* get the DEVICE serial number */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.serial[rs], 32);
            /* get the MCU name */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.mcu[rs], 32);
            /* get the ADC name */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.adc[rs], 32);
            /* get the FLASH name */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.flash[rs], 32);
            /* get the TEMPERATURE SENSOR name */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.sensorT[rs], 32);
            /* get the PRESSURE SENSOR name */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.sensorP[rs], 32);
            /* get the TURBIDITY SENSOR name */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.sensorH[rs], 32);
            /* get the A1 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.A1[rs], 32);
            /* get the A2 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.A2[rs], 32);
            /* get the A3 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.A3[rs], 32);
            /* get the R0 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.R0[rs], 32);
            /* get the TD coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.TD[rs], 32);
            /* get the ALPHA coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.alpha[rs], 32);
            /* get the ZERO coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.zero[rs], 32);
            /* get the K1 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.K1[rs], 32);
            /* get the X01 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.X01[rs], 32);
            /* get the r1 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.r1[rs], 32);
            /* get the dt1 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.dt1[rs], 32);
            /* get the dv1 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.dv1[rs], 32);
            /* get the K2 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.K2[rs], 32);
            /* get the X02 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.X02[rs], 32);
            /* get the r2 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.r2[rs], 32);
            /* get the dt2 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.dt2[rs], 32);
            /* get the dv2 coefficient */
            rs = 0;
            while (rs < 16)
                rs += cdc_read_buf(&cdc_in, &deviceInfo.dv2[rs], 32);
            // WRITE the DEVICE INFO
            WriteDevInfo();
            // OK
            sniprintf(s, sizeof(s), "OK\n\r");

        } else if (strcmp(tk, cmd_list[CMD_SetTimePrescaler]) == 0) {
            ReadTimeSettings();
            // obtain the TimePrescaler form the host
            tk = _strtok(NULL, " \n\r");
            if (!tk) {
                cdc_write_buf(&cdc_out, "Err - Provide the integer value for TimePrescaler in ppb - parts per billion\r\n", 0, 1);
                sniprintf(s, sizeof(s), "");
                goto out;
            }   
            timeSettings.prescaler = atof(tk);
            // save new TimePrescaler value
            WriteTimeSettings();
            // OK
            sniprintf(s, sizeof(s), "OK\n\r");

        } else if (strcmp(tk, cmd_list[CMD_SetTime]) == 0) {
            uint8_t i = 0;
            uint32_t epoch;
            ReadTimeSettings();
            // initialize RTC
            RTC_Init();
            cdc_write_buf(&cdc_out, "ready\r\n", 0, 1);
            // real time receive form the host
            while (i < 4)
                i += cdc_read_buf(&cdc_in, &data[i], 4);
            // set RTC counter value
            epoch = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
            RTC_SetCounter(epoch);
            timeSettings.start = epoch;
            // save TimeSettings
            WriteTimeSettings();
            // OK
            sniprintf(s, sizeof(s), "OK\n\r");

        } else if (strcmp(tk, cmd_list[CMD_GetTime]) == 0) {
            uint32_t epoch, corr_epoch, prscl;

            RTC_Init();
            ReadTimeSettings();
            epoch = RTC_GetCounter();
            corr_epoch = GetTime();
            prscl = 10000000*timeSettings.prescaler;

            sniprintf(s, sizeof(s), "%d\r\n", timeSettings.start);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            sniprintf(s, sizeof(s), "%d\r\n", epoch);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            sniprintf(s, sizeof(s), "%d\r\n", corr_epoch);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            sniprintf(s, sizeof(s), "%d\r\n", prscl);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            sniprintf(s, sizeof(s), "");

        } else if (strcmp(tk, cmd_list[CMD_SetProgramm]) == 0) {
            // RTC enable
            RTC_Init();
            // get the frequency of data acquiring as "every X seconds"
            tk = _strtok(NULL, " \n\r");
            if (!tk) {
                cdc_write_buf(&cdc_out, "Err - Provide the DATA acquiring frequency\r\n", 0, 1);
                sniprintf(s, sizeof(s), "");
                goto out;
            }
            loggerSettings.freq = atoi(tk);

            // get the start time for data acquire in seconds since epoch 
            tk = _strtok(NULL, " \n\r");
            if (!tk) {
                cdc_write_buf(&cdc_out, "Err - Provide the data acquiring start time\r\n", 0, 1);
                sniprintf(s, sizeof(s), "");
                goto out;
            }
            loggerSettings.start = atoi(tk);

            // get the finish time for data acquire in seconds since epoch 
            tk = _strtok(NULL, " \n\r");
            if (!tk) {
                cdc_write_buf(&cdc_out, "Err - Provide the data acquiring finish time\r\n", 0, 1);
                sniprintf(s, sizeof(s), "");
                goto out;
            }
            loggerSettings.finish = atoi(tk);

            // save the address to the BKP registers
            SaveAddress(0x000000);
            // set number corrections to zero
            SaveNumberCorrection(0);
            // save the settings to the MCU FLASH
            WriteProgramSettings();
            //
            sniprintf(s, sizeof(s), "OK\r\n");

        } else if (strcmp(tk, cmd_list[CMD_GetProgramm]) == 0) {
            uint32_t address;
            // RTC enable
            RTC_Init();
            // get the address from the BKP registers
            address = GetAddress();
            // get the settings from the MCU FLASH
            ReadProgramSettings();
            // TODO it is necessary to understand, why it MUST be somewhere here BEFORE the data output
            // otherwise the PySerial does not read the transmitted data, but minicom do :(
            vTaskDelay(10);
            // ADDRESS
            sniprintf(s, sizeof(s), "%x\r\n", address);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            // SCHEDULE
            sniprintf(s, sizeof(s), "%d\r\n", loggerSettings.start);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            sniprintf(s, sizeof(s), "%d\r\n", loggerSettings.finish);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            // FREQUENCY
            sniprintf(s, sizeof(s), "%d\r\n", loggerSettings.freq);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            //
            sniprintf(s, sizeof(s), "OK\r\n");

        } else if (strcmp(tk, cmd_list[CMD_SendDataToSTM]) == 0) {
            uint32_t sz = 0, rsz = 0, wr = 0, rs = 0;
            uint32_t address;
            // data ammount to receive from the host
            tk = _strtok(NULL, " \n\r");
            if (!tk) {
                cdc_write_buf(&cdc_out, "Err1 - Provide the data amount in bytes!\r\n", 0, 1);
                sniprintf(s, sizeof(s), "");
                goto out;
            }
            sz = atoi(tk);
            // address to write in
            tk = _strtok(NULL, " \n\r");
            if (!tk) {
                cdc_write_buf(&cdc_out, "Err2 - Provide the address to write to!\r\n", 0, 1);
                sniprintf(s, sizeof(s), "");
                goto out;
            }
            address = strtol(tk, NULL, 16); // it must point at the sector begining!!! like this XXX000 or XXXXX000 for MX25L256
            // turn SPI ON
            MY_SPI_Init(1);
            while (rsz < sz) {
                // erasing if adress points to the sector begining
                if ((address & 0x000FFF) == 0) {
                    MEM_EraseSector(address);
                }
                // number of bytes to write to FLASH. MX25L256 allows writing of not more than one page per cycle
                wr = PAGE_SIZE*(rsz/PAGE_SIZE < sz/PAGE_SIZE) + sz%PAGE_SIZE*(rsz/PAGE_SIZE == sz/PAGE_SIZE);
                // declare readiness
                cdc_write_buf(&cdc_out, "ready\r\n", 0, 1);
                // read data fron USB IO bufer
                rs = 0;
                while (rs < wr)
                    rs += cdc_read_buf(&cdc_in, &data[rs], wr);
                rsz += rs;
                // write the data to the flash 
                MEM_WriteData(data, address, wr);
                // increment the address
                address += wr;
            }
            // turn the SPI OFF
            MY_SPI_DeInit();
            // declare finish
            cdc_write_buf(&cdc_out, "done\r\n", 0, 1);
            sniprintf(s, sizeof(s), "");

        } else if (strcmp(tk, cmd_list[CMD_SendDataToX86]) == 0) {
            uint32_t sz = 0, sd = 0, rd = 0;
            uint32_t address;
            // set fast blink mode
            SaveBlinkMode(1);
            // amount of data to send to the host, bytes
            tk = _strtok(NULL, " \n\r");
            if (!tk) {
                cdc_write_buf(&cdc_out, "Err1 - Provide the data amount in bytes!\r\n", 0, 1);
                sniprintf(s, sizeof(s), "");
                goto out;
            }
            sz = atoi(tk);
            // address to read from
            tk = _strtok(NULL, " \n\r");
            if (!tk) {
                cdc_write_buf(&cdc_out, "Err2 - Provide the address to read from!\r\n", 0, 1);
                sniprintf(s, sizeof(s), "");
                goto out;
            }
            address = strtol(tk, NULL, 16);
            // turn the SPI on
            MY_SPI_Init(1);
            while (sd < sz) {
                // number of bytes to read from the FLASH
                rd = SECTOR_SIZE*(sd/SECTOR_SIZE < sz/SECTOR_SIZE) + sz%SECTOR_SIZE*(sd/SECTOR_SIZE == sz/SECTOR_SIZE);
                // read data from the FLASH to the data bufer
                MEM_ReadData(data, address, rd);
                // write the data to the USB IO bufer
                sd += cdc_write_buf(&cdc_out, data, rd, 1);
                // increment the address 
                address += rd;
            }
            // turn the FLASH off
            MY_SPI_DeInit();
            sniprintf(s, sizeof(s), "");
            // set slow blink mode
            SaveBlinkMode(0);

        } else if (strcmp(tk, cmd_list[CMD_GetData]) == 0) {
            uint8_t SR, NUM;
            uint16_t CR, MR;
            uint32_t VREF, Q;
            long V;

            NUM  = 16;
            VREF = 3000000000;
            /*
            MY_SPI_Init(1);
            ADC_Reset();
            ADC_SetConfig(ADC_CR_BM | ADC_CR_Gain1 | ADC_CR_RD | ADC_CR_BUF | ADC_CR_CH1);
            Q = ADC_ReadDataSingle(ADC_MR_FS_4);
            sniprintf(s, sizeof(s), "CH1 = 0x%06x\r\n", Q);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            ADC_SetConfig(ADC_CR_BM | ADC_CR_Gain1 | ADC_CR_RD | ADC_CR_BUF | ADC_CR_CH2);
            Q = ADC_ReadDataSingle(ADC_MR_FS_4);
            sniprintf(s, sizeof(s), "CH2 = 0x%06x\r\n", Q);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            ADC_SetConfig(ADC_CR_BM | ADC_CR_Gain1 | ADC_CR_RD | ADC_CR_BUF | ADC_CR_CH3);
            Q = ADC_ReadDataSingle(ADC_MR_FS_4);
            sniprintf(s, sizeof(s), "CH3 = 0x%06x\r\n", Q);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);

            MR = ADC_ReadMode();
            CR = ADC_ReadConfig();
            SR = ADC_ReadStatus();
            sniprintf(s, sizeof(s), "ADC:  MR - 0b%04x, CR - 0x%04x, SR - 0x%02x\r\n", MR, CR, SR);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            
           
            int i, j;
            uint32_t DD[NUM];
            float coef;
            float D1, D2;

            MY_SPI_Init(1);
            ADC_Reset();
            ADC_SetConfig(ADC_CR_BM | ADC_CR_Gain1 | ADC_CR_RD | ADC_CR_BUF | ADC_CR_CH1);
            ADC_ReadDataCont(DD, sizeof(DD) / 4, ADC_MR_FS_4);
            MY_SPI_DeInit();
            coef = 1.0;//(1 << (GAIN >> 8));
            for (i = 0; i < sizeof(DD) / 4; i++) {
                D1 = DD[i];
                D2 += (VREF / 8388608.0) * (D1 - 8388608.0) / coef;
                //sniprintf(s, sizeof(s), "D1 = %x, D2 = %d\r\n", D1, D2);
                j = D1;
                sniprintf(s, sizeof(s), "D1 = %x\r\n", j);
                cdc_write_buf(&cdc_out, s, strlen(s), 1);
                j = D2 / 1000;
                sniprintf(s, sizeof(s), "D2 = %d\r\n", j);
                cdc_write_buf(&cdc_out, s, strlen(s), 1);
            }
            // voltage as INTEGER in the NANO-volts
            coef = NUM;
            V = D2 / coef;
            */
            // get temperature 
            V = MakePreciseMeasurement(ADC_CR_CH2, ADC_CR_Gain1, NUM, ADC_MR_FS_19, VREF);
            sniprintf(s, sizeof(s), "%d\r\n", V / 1000);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            //get pressure
            V = MakePreciseMeasurement(ADC_CR_CH1, ADC_CR_Gain1, NUM, ADC_MR_FS_19, VREF);
            sniprintf(s, sizeof(s), "%d\r\n", V / 1000);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            //get turbidity at A = 0ma
            V = MakePreciseMeasurement(ADC_CR_CH3, ADC_CR_Gain1, NUM, ADC_MR_FS_19, VREF);
            sniprintf(s, sizeof(s), "%d\r\n", V / 1000);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            //get turbidity at A = 10ma
            LED_ON_LOW();
            MY_SPI_Init(0);
            Set_DAC_Output(DAC_NM, 273);
            MY_SPI_DeInit();
            V = MakePreciseMeasurement(ADC_CR_CH3, ADC_CR_Gain1, NUM, ADC_MR_FS_19, VREF);
            sniprintf(s, sizeof(s), "%d\r\n", V / 1000);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            //get turbidity at A = 80ma
            LED_ON_LOW();
            MY_SPI_Init(0);
            Set_DAC_Output(DAC_NM, 2184);
            MY_SPI_DeInit();
            V = MakePreciseMeasurement(ADC_CR_CH3, ADC_CR_Gain1, NUM, ADC_MR_FS_19, VREF);
            sniprintf(s, sizeof(s), "%d\r\n", V / 1000);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            // switch the DAC off
            LED_ON_HIGH();
            MY_SPI_Init(0);
            Set_DAC_Output(DAC_PD1, 0);
            MY_SPI_DeInit();
            // OK
            sniprintf(s, sizeof(s), "OK\n\r");

        } else if (strcmp(tk, cmd_list[CMD_TestSPI]) == 0) {
            char D[16];
            int V1, V2, V3;
            uint8_t SR;
            uint16_t CR, MR;
            uint32_t ID, address, DD, VREF = 3000000000;
            // ADC 
            MY_SPI_Init(1);
            ADC_Reset();
            ADC_SetConfig(ADC_CR_BM | ADC_CR_Gain1 | ADC_CR_RD | ADC_CR_BUF | ADC_CR_CH1);
            // read config
            ID = ADC_ReadID();
            MR = ADC_ReadMode();
            CR = ADC_ReadConfig();
            SR = ADC_ReadStatus();
            sniprintf(s, sizeof(s), "ADC:  ID - %6x, MR - %4x, CR - %4x, SR - %4x\r\n", ID, MR, CR, SR);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            MY_SPI_DeInit();

            // measurements
            V1 = MakePreciseMeasurement(ADC_CR_CH1, ADC_CR_Gain1, 16, ADC_MR_FS_12, VREF);
            V2 = MakePreciseMeasurement(ADC_CR_CH2, ADC_CR_Gain1, 16, ADC_MR_FS_12, VREF);
            V3 = MakePreciseMeasurement(ADC_CR_CH3, ADC_CR_Gain1, 16, ADC_MR_FS_12, VREF);

            // ADC final state
            MY_SPI_Init(1);
            ID = ADC_ReadID();
            MR = ADC_ReadMode();
            CR = ADC_ReadConfig();
            SR = ADC_ReadStatus();
            MY_SPI_DeInit();
            sniprintf(s, sizeof(s), "ADC_TEMP: %12d\r\n", V2);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            sniprintf(s, sizeof(s), "ADC_PRES: %12d\r\n", V1);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            sniprintf(s, sizeof(s), "ADC_TURB: %12d\r\n", V3);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            sniprintf(s, sizeof(s), "ADC:  ID - %6x, MR - %4x, CR - %4x, SR - %4x\r\n", ID, MR, CR, SR);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);

            // MEMORY
            MY_SPI_Init(1);
            ID = MEM_ReadID();
            SR = MEM_ReadSR();
            sniprintf(s, sizeof(s), "MEM: ID - %6x; SR - %2x\r\n", ID, SR);
            cdc_write_buf(&cdc_out, s, strlen(s), 1);
            MY_SPI_DeInit();
            //
            MakeMeasurement();
            address = GetAddress() - 24;
            for (int i = 0; i < 6; i++) {
                MY_SPI_Init(1);
                MEM_ReadData(D, address, 4);
                MY_SPI_DeInit();
                address += 4;
                V1 = D[0] << 24 | D[1] << 16 | D[2] << 8 | D[3];
                sniprintf(s, sizeof(s), "MEMORY: %1d -> %12d\r\n", i, V1);
                cdc_write_buf(&cdc_out, s, strlen(s), 1);
                }

            sniprintf(s, sizeof(s), "");
            // OK
            sniprintf(s, sizeof(s), "OK\n\r");
        } else if (strcmp(tk, cmd_list[CMD_sleep]) == 0) {
            POWER_OFF_HIGH();

        } else
            sniprintf(s, sizeof(s), "E: try `help`\r\n");
out:
        rw->write(s, strlen(s));
    }
}

