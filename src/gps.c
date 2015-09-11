#include "stm32f2xx_gpio.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_usart.h"
#include "misc.h"
#include "RingBuffer.h"

#include "gps.h"

#include <string.h>

void GPS_Decode(char* str);


RingBuffer_t GPS_USART_RxRingBuffer;

static GPS_Message_t GPS_Message, GPS_temp;

void GPS_Init()
{
    //RingBuffer Init
    InitRingBuffer(&GPS_USART_RxRingBuffer);

    // Serial Interface Init
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = 38400;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART6, &USART_InitStruct);

    USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);

    USART_Cmd(USART6, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
}

void GPS_UART_Capture()
{
    static char rxString[100];
    static uint8_t rxStringWritePos = 0;

    char tmpchar = '\0';
    if (RingBufferRead(&GPS_USART_RxRingBuffer, &tmpchar) == NO_ERROR)
    {
        if (tmpchar == '$')
        {
            rxStringWritePos = 0;
        }
        else if (tmpchar == '*')
        {
            rxStringWritePos = 0;
            // kompletter String empfangen
            GPS_Decode(rxString);
        }
        else
        {
            // einzelnes Datenzeichen Empfangen
            // anhängen des Zeichens an rxString
            rxString[rxStringWritePos++] = tmpchar;
        }
    }
}

void GPS_Decode(char* str)
{
    //char nmeaType[6] = "NONE";
    const char delimiter[] = ",";
    char *ptr;

    // initialisieren und ersten Abschnitt erstellen
    ptr = strtok(str, delimiter);

    if (strncmp("GPGGA", ptr, 5) == 0)
    {
        // GPGGA folgt

        strncpy(GPS_temp.valid, "0", 1);

        char tmpvalid[2] = "0";

        ptr = strtok(NULL, delimiter);  // delete sentence_type

        strncpy(GPS_temp.time, ptr, 6);      // Capture Time
        // naechsten Abschnitt erstellen
        ptr = strtok(NULL, delimiter);

        strncpy(GPS_temp.latitude, ptr, 9);
        ptr = strtok(NULL, delimiter);
        //strncpy(GPS_temp.latitude_dir, ptr, 1);
        ptr = strtok(NULL, delimiter);
        strncpy(GPS_temp.longitude, ptr, 10);
        ptr = strtok(NULL, delimiter);
        //strncpy(GPS_temp.longitude_dir, ptr, 1);
        ptr = strtok(NULL, delimiter);
        strncpy(tmpvalid, ptr, 1);
        ptr = strtok(NULL, delimiter);
        strncpy(GPS_temp.satelites, ptr, 2);
        ptr = strtok(NULL, delimiter);
        ptr = strtok(NULL, delimiter);  // delete precisition
        strncpy(GPS_temp.altitude, ptr, 7);

        const char delimiter_point[] = ".";
        ptr = strtok(GPS_temp.altitude, delimiter_point);
        strncpy(GPS_temp.altitude, ptr, 7);

        strncpy(GPS_temp.valid, tmpvalid, 1);
    }
}

GPS_Message_t GPS_GetPosition()
{

    if(GPS_temp.valid[0] != '0')
    {
        // Nachricht gültig
        strncpy(GPS_Message.altitude, GPS_temp.altitude, 7);
        strncpy(GPS_Message.latitude, GPS_temp.latitude, 9);
        strncpy(GPS_Message.latitude_dir, GPS_temp.latitude_dir, 1);
        strncpy(GPS_Message.longitude, GPS_temp.longitude, 10);
        strncpy(GPS_Message.longitude_dir, GPS_temp.longitude_dir, 1);
        strncpy(GPS_Message.satelites, GPS_temp.satelites, 2);
        strncpy(GPS_Message.speed, GPS_temp.speed, 4);
        strncpy(GPS_Message.time, GPS_temp.time, 6);
        strncpy(GPS_Message.valid, GPS_temp.valid, 1);
    }
    return GPS_Message;
}

void USART6_IRQHandler()
{
	if (USART_GetITStatus(USART6,USART_IT_RXNE) != RESET)
    {
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);

		RingBufferWrite(&GPS_USART_RxRingBuffer, (char)USART_ReceiveData(USART6));
	}
}

void USART6SendChar(char c)
{
	USART_SendData(USART6, (uint16_t) c);
	while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
}

void USART6SendString(char* str)
{
	while(*str)
    {
		USART6SendChar(*str++);
	}

}
