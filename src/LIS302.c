#include "i2c.h"

#define LIS302_I2C_ADDRESS 0b00111000



void LIS302_Init()
{
    uint8_t* temp;
    temp = UB_I2C_ReadByte(I2C1, LIS302_I2C_ADDRESS, 0x0F);    // Read ID

    if (temp[0] != 0b00111011)
    {
        // Sensor ID nicht gültig
        return;
    }

    UB_I2C_WriteByte(I2C1, LIS302_I2C_ADDRESS, 0x20, 0x67); // Set active mode, 8G, x, y, z enable
}

void LIS302_GetAcc(int16_t* x, int16_t* y, int16_t* z)
{
    uint8_t* temp;

    temp = UB_I2C_ReadByte(I2C1, LIS302_I2C_ADDRESS, 0x29);
    if (temp[0] & 0x80)
    {
        // negativ
        temp[0] = ~temp[0];
        temp[0]++;
        *x = temp[0]*72;
    }
    else
    {
        *x = temp[0]*72;
    }

    temp = UB_I2C_ReadByte(I2C1, LIS302_I2C_ADDRESS, 0x2B);
    if (temp[0] & 0x80)
    {
        // negativ
        temp[0] = ~temp[0];
        temp[0]++;
        *y = temp[0]*72;
    }
    else
    {
        *y = temp[0]*72;
    }

    temp = UB_I2C_ReadByte(I2C1, LIS302_I2C_ADDRESS, 0x2D);
    if (temp[0] & 0x80)
    {
        // negativ
        temp[0] = ~temp[0];
        temp[0]++;
        *z = temp[0]*72;
    }
    else
    {
        *z = temp[0]*72;
    }
}
