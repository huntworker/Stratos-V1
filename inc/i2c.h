#include <stdint.h>

#include "stm32f2xx_i2c.h"

#define I2C_TIMEOUT     0x0500

void i2c_init_tmp(I2C_TypeDef* I2Cx);
uint8_t* UB_I2C_Read(I2C_TypeDef* I2Cx, uint8_t slave_adr);
uint8_t* UB_I2C_ReadByte(I2C_TypeDef* I2Cx, uint8_t slave_adr, uint8_t adr);
uint8_t* UB_I2C_ReadWord(I2C_TypeDef* I2Cx, uint8_t slave_adr, uint8_t adr);
uint8_t* UB_I2C_Read24b(I2C_TypeDef* I2Cx, uint8_t slave_adr);
uint8_t UB_I2C_WriteCommand(I2C_TypeDef* I2Cx, uint8_t slave_adr, uint8_t cmd);
uint8_t UB_I2C_WriteByte(I2C_TypeDef* I2Cx, uint8_t slave_adr, uint8_t adr, uint8_t wert);
void UB_I2C_Delay(volatile uint32_t nCount);

