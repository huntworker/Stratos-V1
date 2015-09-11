#include "stm32f2xx.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_rcc.h"

#include "RFM23.h"
#include "SPI.h"

void RFM23_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	// !SDN-Pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_SetBits(GPIOC, GPIO_Pin_5);    // Shutdown


	my_SPI_Init();

	// PA0=GPIO0
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(GPIOA, &GPIO_InitStruct);


	// PA1=RXON, PA2 = TXON
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_ResetBits(GPIOC, GPIO_Pin_5);    // !Shutdown

    //while(1)
    //{
    //    uint32_t devtype = RFM23_Read(0x00);
    //}


	//RFM23_Write(0x05, 0x00);	// no interrupt enable (default)
	RFM23_Write(0x06, 0x00);	// no interrupt enable
	RFM23_Write(0x07, 0x00);	// Standby-mode
	//RFM23_Write(0x08, 0x00);	// (default)
	RFM23_Write(0x09, 0x7F);	// XTAL load (default)
	//RFM23_Write(0x0A, 0x06);	// Clock output 1MHz (default)
	//RFM23_Write(0x0B, 0xD0);	// GPIO0 TX input (direct modulation)
	RFM23_Write(0x0B, 0x55);	// GPIO0 RX State
	//RFM23_Write(0x0C, 0x00);	// GPIO1 !POR (weak) (default)
	RFM23_Write(0x0C, 0x4F);	// GPIO1 Data CLK
	//RFM23_Write(0x0D, 0x00);	// GPIO2 Clock Output (weak) (default)
	RFM23_Write(0x0D, 0x52);	// GPIO2 TX State
	//RFM23_Write(0x0E, 0x00);	// GPIO Config (default)
	//RFM23_Write(0x0F, 0x00);	// ADC: none (default)
	//RFM23_Write(0x10, 0x00);	// ADC offset: 0 (default)
	//RFM23_Write(0x11, 0x00);	// ADC value (read only)
	RFM23_Write(0x12, 0x00);	// Tmp Sensor cal: none
	//RFM23_Write(0x13, 0x00);	// Tmp offset: 0 (default)
	//RFM23_Write(0x14, 0x03);	// Wakeup timer period (default)
	//RFM23_Write(0x15, 0x00);	// Wakeup timer period (default)
	//RFM23_Write(0x16, 0x01);	// Wakeup timer period (default)
	//RFM23_Write(0x17, 0x00);	// Wakeup timer value (read only)
	//RFM23_Write(0x18, 0x00);	// Wakeup timer value (read only)
	//RFM23_Write(0x19, 0x01);	// Low Duty cycle value (default)
	//RFM23_Write(0x1A, 0x14);	// Low Battery Threshold 2,7V (default)
	//RFM23_Write(0x1B, 0x00);	// V_Bat (read only)
	//RFM23_Write(0x1C, 0x01);	// IF Bandwidth 75,2kHz (default)
	//RFM23_Write(0x1D, 0x44);	// AFC enable (default)
	//RFM23_Write(0x1E, 0x0A);	// AFC Timing (default)
	//RFM23_Write(0x1F, 0x03);	// Clock recovery (default)
	//RFM23_Write(0x20, 0x64);	// Clock recovery oversampling (default)
	//RFM23_Write(0x21, 0x01);	// Clock recovery offset (default)
	//RFM23_Write(0x22, 0x87);	// Clock recovery offset (default)
	//RFM23_Write(0x23, 0xAE);	// Clock recovery offset (default)
	//RFM23_Write(0x24, 0x02);	// Clock recovery timing loop (default)
	//RFM23_Write(0x25, 0x8F);	// Clock recovery timing loop (default)
	//RFM23_Write(0x26, 0x00);	// RSSI (read only)
	//RFM23_Write(0x27, 0x1E);	// RSSI for clear channel (default)
	//RFM23_Write(0x28, 0x00);	// ANT1 diverity (read only)
	//RFM23_Write(0x29, 0x00);	// ANT2 diverity (read only)
	//RFM23_Write(0x2A, 0x00);	// AFC limiter (default)
	//RFM23_Write(0x2B, 0x00);	// AFC correction (read only)
	//RFM23_Write(0x2C, 0x18);	// OOK Counter (default)
	//RFM23_Write(0x2D, 0xBC);	// OOK Counter (default)
	//RFM23_Write(0x2E, 0x2C);	// (default)
	//RFM23_Write(0x2F, 0x00);	// not available
	RFM23_Write(0x30, 0x00);	// Data Access: none
	//RFM23_Write(0x31, 0x00);	// EZMAC Status (read only)
	RFM23_Write(0x32, 0x00);	// Header: none
	RFM23_Write(0x33, 0x00);	// Header: none
	RFM23_Write(0x34, 0x00);	// Preamble: length
	RFM23_Write(0x35, 0x00);	// Preamble: detection
	RFM23_Write(0x36, 0x00);	// Header: none
	RFM23_Write(0x37, 0x00);	// Sync Word
	RFM23_Write(0x38, 0x00);	// Sync Word
	RFM23_Write(0x39, 0x00);	// Sync Word
	RFM23_Write(0x3A, 0x00);	// Header
	RFM23_Write(0x3B, 0x00);	// Header
	RFM23_Write(0x3C, 0x00);	// Header
	RFM23_Write(0x3D, 0x00);	// Header
	RFM23_Write(0x3E, 0x00);	// Packet length
	RFM23_Write(0x3F, 0x00);	// Check Header
	RFM23_Write(0x40, 0x00);	// Check Header
	RFM23_Write(0x41, 0x00);	// Check Header
	RFM23_Write(0x42, 0x00);	// Check Header
	RFM23_Write(0x43, 0x00);	// Header enable
	RFM23_Write(0x44, 0x00);	// Header enable
	RFM23_Write(0x45, 0x00);	// Header enable
	RFM23_Write(0x46, 0x00);	// Header enable
	//RFM23_Write(0x47, 0x00);	// received Header (read only)
	//RFM23_Write(0x48, 0x00);	// received Header (read only)
	//RFM23_Write(0x49, 0x00);	// received Header (read only)
	//RFM23_Write(0x4A, 0x00);	// received Header (read only)
	//RFM23_Write(0x4B, 0x00);	// received Packet length (read only)
	//RFM23_Write(0x4C, 0x00);	// not available
	//RFM23_Write(0x4D, 0x00);	// not available
	//RFM23_Write(0x4E, 0x00);	// not available
	//RFM23_Write(0x4F, 0x10);	// ADC Control (default)

	//RFM23_Write(0x60, 0x00);	// Channel Filter (default)
	//RFM23_Write(0x61, 0x00);	// not available
	//RFM23_Write(0x62, 0x04);	// POR (default)

	RFM23_Write(0x69, 0x20);	// AGC disable

	//RFM23_Write(0x6D, 0x0F);	// TX Power max
	RFM23_Write(0x6D, (1<<3) | 3);	// TX Power
	RFM23_Write(0x6E, 0x00);	// TX data rate ~0,95bps
	RFM23_Write(0x6F, 0x02);	// TX data rate ~0,95bps
	RFM23_Write(0x70, 0x2C);	// modulation control
//	RFM23_Write(0x71, 0x31);	// OOK, PN-Folge
	RFM23_Write(0x71, 0x30);	// Unmodulated Carier
	RFM23_Write(0x72, 0x50);	// Frequency deviation 50kHz (n*625Hz)
	RFM23_Write(0x73, 0x00);	// Frequency offset 0
	RFM23_Write(0x74, 0x00);	// Frequency offset 0
	//RFM23_Write(0x75, 0x53);	// Frequency Band 430-440MHz
	RFM23_Write(0x75, 0x13);	// Frequency Band 430-440MHz
	RFM23_Write(0x76, 0x4B);	// nominal Carrier Frequency 433,0 MHz
	RFM23_Write(0x77, 0x00);	// nominal Carrier Frequency

	RFM23_Write(0x79, 0x00);	// no Frequency hopping
	RFM23_Write(0x7A, 0x00);	// no Frequency hopping

	//RFM23_Write(0x7C, 0x37);	// TX FIFO Control (default)
	//RFM23_Write(0x7D, 0x04);	// TX FIFO Control (default)
	//RFM23_Write(0x7E, 0x37);	// RX FIFO Control (default)
	//RFM23_Write(0x7F, 0x01);	// FIFO Access

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);  // !RXON
	GPIO_SetBits(GPIOA, GPIO_Pin_2);    // TXON

	RFM23_Write(0x07, (1<<3));	// TX ON

	uint32_t power = 1;
	uint32_t a = 1;

	while(1)
    {
        power = a;

        RFM23_Write(0x6D, (1<<3) | power);	// TX Power
    }
}

void RFM23_TX_High()
{
	//GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

void RFM23_TX_Low()
{
	//GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

void RFM23_TX_Toggle()
{
	//GPIO_ToggleBits(GPIOA, GPIO_Pin_0);
}

void RFM23_Write(uint8_t address, uint8_t data)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);	// !CS low
	SPI_RxTx(SPI1, address | 0x80);
	SPI_RxTx(SPI1, data);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);	// !CS high
}

uint8_t RFM23_Read(uint8_t address)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);		// !CS low
	SPI_RxTx(SPI1, address & 0x7F);
	uint8_t RxData = SPI_RxTx(SPI1, 0xFF);	// dummy-byte Senden
	GPIO_SetBits(GPIOA, GPIO_Pin_4);		// !CS high

	return RxData;
}
