#include <inttypes.h>

#include "stm32f2xx_iwdg.h"

uint8_t watchdog_init(uint16_t time_ms)
{
    uint8_t ret_val = 0;

    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        // Letzter Reset durch Watchdog ausgelöst!
        RCC_ClearFlag();

        ret_val = 1;
    }


    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    // Clock source = LSI = 32.768 kHz; Vorteiler = 32 -> Clock ca. 1 ms

    IWDG_SetPrescaler(IWDG_Prescaler_32);

    IWDG_SetReload(time_ms);

    IWDG_ReloadCounter();

    IWDG_Enable();

    return ret_val;
}

void watchdog_trigger()
{
    IWDG_ReloadCounter();
}


