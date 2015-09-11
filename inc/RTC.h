#include "stm32f2xx.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_pwr.h"
#include "stm32f2xx_rtc.h"

typedef struct
{
  uint8_t std;     // studen   [0...23]
  uint8_t min;     // minuten  [0...59]
  uint8_t sek;     // sekunden [0...59]
  uint8_t tag;     // tag      [1...31]
  uint8_t monat;   // monat    [1...12]
  uint8_t jahr;    // jahr     [0...99]
  uint8_t wtag;   // wochentag [1...7] 1=Montag
}RTC_t;


void my_RTC_Init(void);
void RTC_Set(RTC_t newTime);
RTC_t RTC_Get(void);
