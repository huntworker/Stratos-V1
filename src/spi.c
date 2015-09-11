#include "stm32f2xx.h"
#include "stm32f2xx_spi.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_rcc.h"

#include "SPI.h"

void my_SPI_Init()
{
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); //SCK
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); //MISO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); //MOSI

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;		//!CS
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);


	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //60 MHz / 16 = 3,75 MHz
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;   //erste Flanke = sample
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low; // Clock Idle State = Low
	SPI_InitStruct.SPI_CRCPolynomial = 42;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI_InitStruct);

	SPI_Cmd(SPI1, ENABLE);


}

uint8_t SPI_RxTx(SPI_TypeDef* SPIx, uint8_t TxByte)
{
	uint8_t RxByte = 0;

	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

	SPI_I2S_SendData(SPIx, TxByte);

	//while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);

	//wait for Transmission!!
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);

	RxByte = SPI_I2S_ReceiveData(SPIx);

	return RxByte;
}
