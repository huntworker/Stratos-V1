
#include "usart.h"
#include "i2c.h"
#include "MCP9800.h"
#include "stm32_ub_fatfs.h"
#include "sdio.h"
#include "RTC.h"
#include "RFM23.h"
#include "DAC.h"
#include "stm32f2xx_RNG.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_gpio.h"
#include "ax25.h"
#include "gps.h"
#include "RRC_Bat.h"
#include "watchdog.h"
#include "MS5607.h"
#include "adc.h"
#include "LIS302.h"

#include "stm32f2xx.h"

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

uint32_t flags;
#define FLAG_LOGGING_ENABLE 0

void RNG_Init();
void SD_add_string(char* str);

static volatile uint32_t timeout = 60;


int main()
{
	SystemInit();


	char buffer[100];

	char temperature[4][4];
	char batt_abs_charge[7];
	char batt_avg_ttl[7];
	char pressure[5];

	uint8_t last_reset_by_watchdog = watchdog_init(2000);

	usart_init();
	//my_RTC_Init();

	my_DAC_Init();
	my_ADC_Init();

	//RNG_Init();

	ax25_init();

	GPS_Init();

    i2c_init_tmp(I2C1);
	MCP_9800_init();
    // RRC_Bat_init();
    MS5607_init();
    LIS302_Init();

    UB_Fatfs_Init();

    //Debug
	/*RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);*/
	// End Debug


	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency);

	//RFM23_Init(); //Nur mit Antenne/Dummy Load! TXON!

	usart_send_char(0);

	if (last_reset_by_watchdog)
    {
        snprintf(buffer, 40, "\r\n!!WATCHDOG CAUSED LAST RESET!!\r\n");
        usart_send_string(buffer);
        rf_send_string(buffer);
    }

	snprintf(buffer, 40, "USART Init abgeschlossen\r\n"); //_sprintf ohne gleitkomma mit max 40 Zeichen
	usart_send_string(buffer);



	GPS_Message_t GPS_Message;

    int16_t tmp_i16;
    uint16_t tmp_u16;
    uint32_t last_time = 0;

	while(1)
	{
	    watchdog_trigger();

	    //uint32_t zufallszahl = RNG_GetRandomNumber();

	    for (uint8_t i = 0; i < 255; i++)
	        GPS_UART_Capture();

        GPS_Message = GPS_GetPosition();

        uint32_t time = atoi(GPS_Message.time);

        /*
        uint32_t pressure = 0;
        int16_t MS5607_temp = 0;

        MS5607_get_pressure(&pressure, &MS5607_temp);

        snprintf(buffer, 100, "Pressure: %"PRIu32" mBar, Temperature: %"PRId16" deg.\r\n", pressure/100, MS5607_temp/100);
        usart_send_string(buffer);

        tmp_i16 = MCP9800_get_tmp(MCP9800_PCB_ADDRESS);
        snprintf(buffer, 4, "%"PRIi8"", (int8_t)(tmp_i16 & 0xFF));
        usart_send_string(buffer);
        */

        /*
        tmp_i16 = MCP9800_get_tmp(MCP9800_PCB_ADDRESS);
        snprintf(buffer, 4, "%"PRIi8"", (int8_t)(tmp_i16 & 0xFF));
        SD_add_string(buffer);
        usart_send_string(buffer);

        uint32_t pressure = 0;
        int16_t MS5607_temp = 0;

        MS5607_get_pressure(&pressure, &MS5607_temp);

        snprintf(buffer, 100, "Pressure: %"PRIu32" mBar, Temperature: %"PRId16" deg.\r\n", pressure/100, MS5607_temp/100);
        SD_add_string(buffer);
        usart_send_string(buffer);

*/
        /*for(int i = 0; i < 0x7FFFFF; i++)
        {
            __NOP();
        }*/

        int16_t x, y, z;

        LIS302_GetAcc(&x, &y, &z);



        if ((time != last_time) || (timeout > 70))
        {
            // time has changed
            // every second

            if (!(time % 10) || timeout > 70)
            {
                // every 10 sec

                last_time = time;

                tmp_i16 = MCP9800_get_tmp(MCP9800_PCB_ADDRESS);
                // Least Significant Byte = ganze Temperatur
                // Most Significant Byte = Teiler

                snprintf(temperature[0], 4, "%"PRIi8"", (int8_t)(tmp_i16 & 0xFF));

                tmp_i16 = MCP9800_get_tmp(MCP9800_IN1_ADDRESS);
                snprintf(temperature[1], 4, "%"PRIi8"", (int8_t)(tmp_i16 & 0xFF));

                tmp_i16 = MCP9800_get_tmp(MCP9800_IN2_ADDRESS);
                snprintf(temperature[2], 4, "%"PRIi8"", (int8_t)(tmp_i16 & 0xFF));

                tmp_i16 = MCP9800_get_tmp(MCP9800_EXT_ADDRESS);
                snprintf(temperature[3], 4, "%"PRIi8"", (int8_t)(tmp_i16 & 0xFF));

                MS5607_get_pressure(&tmp_u16, &tmp_i16);
                snprintf(pressure, sizeof(pressure) / sizeof(char), "%"PRIu16"", tmp_u16);

#ifndef RRC_AKKU
                tmp_u16 = (uint32_t)UB_ADC1_AV_Read(11, 255) * 4000 / 2918;
                snprintf(batt_abs_charge, sizeof(batt_abs_charge) / sizeof(char), "%"PRIu16"", tmp_u16);

                tmp_u16 = (uint32_t)UB_ADC1_AV_Read(10, 255) * 8000 / 3153;
                snprintf(batt_avg_ttl, sizeof(batt_avg_ttl) / sizeof(char), "%"PRIu16"", tmp_u16);
#else
                tmp_u16 = RRC_Bat_get(RRC_BAT_REG_ABS_CHARGE);
                //tmp_u16 = 98;
                snprintf(batt_abs_charge, sizeof(batt_abs_charge) / sizeof(char), "%"PRIu16"", tmp_u16);

                tmp_u16 = RRC_Bat_get(RRC_BAT_REG_AVG_TIME_TO_EMPTY);
                //tmp_u16 = 10000;
                snprintf(batt_avg_ttl, sizeof(batt_avg_ttl) / sizeof(char), "%"PRIu16"", tmp_u16);
#endif

                buffer[0] = '\0';   // buffer "leeren"
                strncat(buffer, "\r\n\r\nDK0HTW", sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, GPS_Message.time, sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, GPS_Message.latitude, sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                //strncat(buffer, GPS_Message.latitude_dir, sizeof(buffer)-strlen(buffer));
                //strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, GPS_Message.longitude, sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                //strncat(buffer, GPS_Message.longitude_dir, sizeof(buffer)-strlen(buffer));
                //strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, GPS_Message.altitude, sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, GPS_Message.satelites, sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, GPS_Message.valid, sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, temperature[0], sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, temperature[1], sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, temperature[2], sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, temperature[3], sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, pressure, sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, batt_abs_charge, sizeof(buffer)-strlen(buffer));
                strncat(buffer, ",", sizeof(buffer)-strlen(buffer));
                strncat(buffer, batt_avg_ttl, sizeof(buffer)-strlen(buffer));

                // Baudrate GPS Modul
                //strncat(buffer, "!DEBUG VERSION!", sizeof(buffer)-strlen(buffer));



                SD_add_string(buffer);

                strncat(buffer, "*\r\n\r\n", sizeof(buffer)-strlen(buffer));

                usart_send_string(buffer);

                if (!(time % 100) || timeout > 70)
                {
                    timeout = 0;
                    rf_send_string(buffer);
                }
            }

        }
	}
}

void SysTick_Handler(void) // every 1 s
{
    timeout++;

    //GPIO_ToggleBits(GPIOA, GPIO_Pin_15);
}

void SD_add_string(char* str)
{
    FIL myFile;   // Filehandler
    if(UB_Fatfs_CheckMedia(MMC_0) == FATFS_OK)
    {
        // Media mounten
        if(UB_Fatfs_Mount(MMC_0) == FATFS_OK)
        {
            // File zum schreiben im root öffnen bzw. neu anlegen
            if(UB_Fatfs_OpenFile(&myFile, "0:/Logger.txt", F_WR_NEW)==FATFS_OK)
            {
                UB_Fatfs_WriteString(&myFile,str);

                UB_Fatfs_CloseFile(&myFile);
            }
        }
        UB_Fatfs_UnMount(MMC_0);
    }
}

void RNG_Init()
{
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

    RNG_Cmd(ENABLE);

    while(RNG_GetFlagStatus(RNG_SR_DRDY)== RESET);
}

void HardFault_Handler()
{
    while(1);
}
