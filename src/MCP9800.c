#include "i2c.h"
#include "MCP9800.h"

void MCP_9800_init(void)
{
	//UB_I2C_WriteByte(I2C1, 0x90, 0x01, 0x00);	//config register: default
}

int16_t MCP9800_get_tmp(uint8_t address)
{
    int16_t temp = -99;
    uint8_t* get = UB_I2C_ReadWord(I2C1, address, 0x00);	//Temperatur auslesen
    if(get)
    {
        temp = (uint16_t)get[0] | ((uint16_t)get[1] << 8);
    }

    return temp;
}
