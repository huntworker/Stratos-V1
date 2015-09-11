#include "i2c.h"


// Kalibrierwerte nur für diesen einen Chip
// Werden nicht dynamisch ausgelesen, um Fehler zu
// vermeiden
static /*const*/ uint32_t calibration[7] =
{
    /*0x00C0,
    0xA7C4,
    0x97C8,
    0x65A9,
    0x5D2B,
    0x79A4,
    0x6A93*/
};


// dynamisches auslesen der Kalibrierwerte
// wird hier nicht verwendet
void MS5607_init(void)
{

    // Read Calibration from Chip
    uint8_t* raw = 0;

    UB_I2C_WriteCommand(I2C1, 0xEE, 0xA0);
    raw = UB_I2C_Read(I2C1, 0xEE);
    if(raw)
        calibration[0] = ((uint16_t)raw[0] << 8) | (uint16_t)raw[1];
    UB_I2C_WriteCommand(I2C1, 0xEE, 0xA2);
    raw = UB_I2C_Read(I2C1, 0xEE);
    if(raw)
        calibration[1] = ((uint16_t)raw[0] << 8) | (uint16_t)raw[1];
    UB_I2C_WriteCommand(I2C1, 0xEE, 0xA4);
    raw = UB_I2C_Read(I2C1, 0xEE);
    if(raw)
        calibration[2] = ((uint16_t)raw[0] << 8) | (uint16_t)raw[1];
    UB_I2C_WriteCommand(I2C1, 0xEE, 0xA6);
    raw = UB_I2C_Read(I2C1, 0xEE);
    if(raw)
        calibration[3] = ((uint16_t)raw[0] << 8) | (uint16_t)raw[1];
    UB_I2C_WriteCommand(I2C1, 0xEE, 0xA8);
    raw = UB_I2C_Read(I2C1, 0xEE);
    if(raw)
        calibration[4] = ((uint16_t)raw[0] << 8) | (uint16_t)raw[1];
    UB_I2C_WriteCommand(I2C1, 0xEE, 0xAA);
    raw = UB_I2C_Read(I2C1, 0xEE);
    if(raw)
        calibration[5] = ((uint16_t)raw[0] << 8) | (uint16_t)raw[1];
    UB_I2C_WriteCommand(I2C1, 0xEE, 0xAC);
    raw = UB_I2C_Read(I2C1, 0xEE);
    if(raw)
        calibration[6] = ((uint16_t)raw[0] << 8) | (uint16_t)raw[1];


    return;
}

void MS5607_get_pressure(uint16_t* pressure, int16_t* temperature) // P[mbar] = pressure
{
    UB_I2C_WriteCommand(I2C1, 0xEE, 0x48);   // Convert D1, max resolution

    // 10 ms warten, bis messung fertig
    for(volatile int i = 0; i < 0xFFFF; i++)
    {

    }

    UB_I2C_WriteCommand(I2C1, 0xEE, 0x00);   // Read ADC
    uint8_t* raw = UB_I2C_Read24b(I2C1, 0xEE);
    uint32_t D1 = 0;
    if(raw)
    {
        D1 = ((uint32_t)raw[0] << 16) | ((uint32_t)raw[1] << 8)| ((uint32_t)raw[2]);
    }



    UB_I2C_WriteCommand(I2C1, 0xEE, 0x58);   // Convert D2, max resolution

    // 10 ms warten, bis messung fertig
    for(volatile int i = 0; i < 0xFFFF; i++)
    {

    }

    UB_I2C_WriteCommand(I2C1, 0xEE, 0x00);   // Read ADC
    raw = UB_I2C_Read24b(I2C1, 0xEE);

    uint32_t D2 = 0;
    if(raw)
    {
        D2 = ((uint32_t)raw[0] << 16) | ((uint32_t)raw[1] << 8)| ((uint32_t)raw[2]);
    }

    int64_t dT = (int32_t)D2 - ((int32_t)calibration[5] * 256);
    int32_t TEMP = 2000 + dT * calibration[6] / (1 << 23);

    int64_t OFF  = (int64_t)calibration[2] * (1 << 17) + ((int32_t)calibration[4] * dT) / (1 << 6);
    int64_t SENS = (int64_t)calibration[1] * (1 << 16) + ((int32_t)calibration[3] * dT) / (1 << 7);


    int64_t T2 = 0;
    int32_t OFF2 = 0;
    int32_t SENS2 = 0;

    if (TEMP < 2000)
    {
        T2 = dT * dT / (1 << 31);
        OFF2 = 61 * (TEMP - 2000) * (TEMP - 2000) / (1 << 4);
        SENS2 = 2 * (TEMP - 2000) * (TEMP - 2000);

        if (TEMP < -1500)
        {
            OFF2 = OFF2 + 15 * (TEMP + 1500)* (TEMP + 1500);
            SENS2 = SENS2 + 8 * (TEMP + 1500)* (TEMP + 1500);
        }
    }

    TEMP = TEMP - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;


    int32_t P = (D1 * SENS / (1 << 21) - OFF) / (1 << 15);
    P/=100;
    *pressure = (uint16_t)P;
    *temperature = TEMP;

    /* return P; */
}
