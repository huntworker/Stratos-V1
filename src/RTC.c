
#include "RTC.h"





void my_RTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div20);
	
	RCC_RTCCLKCmd(ENABLE);
	
	RTC_WaitForSynchro();
	
	//HSE = 8 MHz, RTC_DIV = 20
	// -> RTC_In = 400 kHz
	// Div_Async = 128  -> 3125 Hz
	// Div_Sync  = 3125 -> 1 Hz
	
	RTC_InitTypeDef RTC_InitStructure;
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv = 0x0C34;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);
	
	RTC_t time;
	time.std = 0;
	time.min = 0;
	time.sek = 0;
	time.tag = 1;
	time.monat = 1;
	time.jahr = 0;
	time.wtag = 1;
	RTC_Set(time);
}

void RTC_Set(RTC_t newTime)
{
	if(newTime.std>23) newTime.std=23;
	if(newTime.min>59) newTime.min=59;
	if(newTime.sek>59) newTime.sek=59;
	
	if(newTime.tag<1) newTime.tag=1;
	if(newTime.tag>31) newTime.tag=31;
	if(newTime.monat<1) newTime.monat=1;
  if(newTime.monat>12) newTime.monat=12;
  if(newTime.jahr>99) newTime.jahr=99;
  if(newTime.wtag<1) newTime.wtag=1;
  if(newTime.wtag>7) newTime.wtag=7;
	
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_TimeStructure.RTC_Hours = newTime.std;
	RTC_TimeStructure.RTC_Minutes = newTime.min;
	RTC_TimeStructure.RTC_Seconds = newTime.sek;
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
	
	RTC_DateTypeDef RTC_DateStructure;
	RTC_DateStructure.RTC_Date = newTime.tag;
	RTC_DateStructure.RTC_Month = newTime.monat;
	RTC_DateStructure.RTC_Year = newTime.jahr;
	RTC_DateStructure.RTC_WeekDay = newTime.wtag;
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
}

RTC_t RTC_Get()
{
	RTC_t time;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	
	time.std = RTC_TimeStructure.RTC_Hours;
  time.min = RTC_TimeStructure.RTC_Minutes;
  time.sek = RTC_TimeStructure.RTC_Seconds;
	
	
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	
	time.tag = RTC_DateStructure.RTC_Date;
  time.monat = RTC_DateStructure.RTC_Month;
  time.jahr = RTC_DateStructure.RTC_Year;
  time.wtag = RTC_DateStructure.RTC_WeekDay;
	
	return time;
}
