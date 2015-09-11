#include "i2c.h"


int16_t RRC_Bat_get(uint8_t reg)
{
    int16_t ret = 0;
    uint8_t* get = UB_I2C_ReadWord(I2C1, 0x16, reg);
    if(get)
    {
        ret = (uint16_t)get[0] | ((uint16_t)get[1] << 8);
    }
    return ret;
}
