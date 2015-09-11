#include "stm32f2xx_tim.h"
#include "misc.h"
#include "stm32f2xx_rcc.h"
#include "stm32f2xx_gpio.h"
#include "string.h"

#include "DAC.h"

static const uint8_t num_of_startbytes = 20;
static uint8_t txString_length = 7;
static char txString[200] = "\0\0DK0HTW SAAR STRATOS\r\n";

static uint8_t rf_on = 0;
/*
static const uint8_t ax25_header[] =
{
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x88, // 'D'
    0x96, // 'K'
    0x60, // '0'
    0x90, // 'H'
    0xA8, // 'T'
    0xAE, // 'W'
    0x60, // '0'
    0x88, // 'D'
    0x96, // 'K'
    0x60, // '0'
    0x90, // 'H'
    0xA8, // 'T'
    0xAE, // 'W'
    0x61, // '0' | 1
    0x03, // Control Field -> UI-Frame
    0xF0, // Protocol ID -> no Layer-3
};
*/
typedef enum
{
    header,
    data,
    trailer
} ax25_buffer_type_t;

typedef struct
{
    ax25_buffer_type_t ax25_buffer_type;
    uint16_t readposition;
} ax25_packet_t;

ax25_packet_t ax25_packet;


void ax25_init()
{
    ax25_packet.ax25_buffer_type = header;
    ax25_packet.readposition = 0;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); // 16MHz
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Period = 4-1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 13333-1;  // Baudrate = f_tim / Period / Prescaler = 300 + 6,25e-6
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM7, ENABLE);
}

void TIM7_IRQHandler() // every 20/6 ms == 300 Hz == 300 Baud
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

        //GPIO_ToggleBits(GPIOA, GPIO_Pin_15);

        /*
        static uint8_t txbyte = 0;
        static uint8_t bitposition = 0;
        if (bitposition > 7)
        {
            bitposition = 0;

            if (ax25_packet.ax25_buffer_type == header)
            {
                if (ax25_packet.readposition < sizeof(ax25_header)/sizeof(uint8_t))
                {
                    txbyte = ax25_header[ax25_packet.readposition++];
                }
                else
                {
                    ax25_packet.readposition = 0;
                    ax25_packet.ax25_buffer_type = data;
                }

            }

            if (ax25_packet.ax25_buffer_type == data)
            {
                if (ax25_packet.readposition < sizeof(ax25_data)/sizeof(uint8_t))
                {
                    txbyte = ax25_data[ax25_packet.readposition++];
                }
                else
                {
                    ax25_packet.readposition = 0;
                    ax25_packet.ax25_buffer_type = trailer;
                }
            }

            if (ax25_packet.ax25_buffer_type == trailer)
            {
                if (ax25_packet.readposition < sizeof(ax25_trailer)/sizeof(uint8_t))
                {
                    txbyte = ax25_trailer[ax25_packet.readposition++];
                }
                else
                {
                    ax25_packet.readposition = 0;
                    ax25_packet.ax25_buffer_type = header;
                }
            }
        }

        uint8_t txbit = (txbyte >> bitposition) & 1;       // LSB first

        transmit(txbit);

        bitposition++;
        */

        // TX String besteht aus: Startbits (1,0), Datenbits(xxxx xxxx)

        static uint8_t BytePosition = 0;
        static uint8_t BitPosition = 0;

        if (!rf_on) // -> rf_off
        {
            BytePosition = 0;
            BitPosition = 0;
        }
        else
        {
            uint8_t txByte = (uint8_t)txString[BytePosition];
            //uint8_t txByte = (uint8_t)ax25_header[BytePosition];



            if (BitPosition == 0)
            {
                transmit(0);
                //GPIO_ToggleBits(GPIOA, GPIO_Pin_15);
            }
            else if ((BitPosition == 9) || (BitPosition == 10))
            {
                transmit(1);
                //GPIO_ToggleBits(GPIOA, GPIO_Pin_15);
            }
            else
            {
                transmit((txByte >> (BitPosition-1)) & 1);       // LSB first
                //GPIO_ToggleBits(GPIOA, GPIO_Pin_15);
            }

            BitPosition++;

            if (BitPosition > 10)
            {
                BytePosition++;
                BitPosition = 0;

                //if (BytePosition >= (sizeof(txString) / sizeof(char)))
                if (BytePosition > txString_length)
                {
                    BytePosition = 0;
                    rf_on = 0;
                    stopSine();
                    GPIO_ResetBits(GPIOA, GPIO_Pin_2);    // Disable PTT
                }
            }
        }

        /*
        uint8_t BytePosition = BitPosition / 8;
        uint8_t InnerBitPosition = BitPosition % 8;

        uint8_t txByte = (uint8_t)txString[BytePosition];

        //uint8_t txBit = (txByte & (1 << InnerBitPosition)) ? 1 : 0;   //MSB first
        uint8_t txBit = (txByte >> InnerBitPosition) & 1;       // LSB first

        //transmit(RNG_GetRandomNumber() & 1);

        transmit(txBit);

        BitPosition++;

        BitPosition = BitPosition % (sizeof(txString) / sizeof(char) * 8);
        */
    }
}

void rf_send_string(char* buffer)
{
    uint8_t i = 0;

    uint8_t buffer_length = strlen(buffer);

    txString_length = buffer_length + num_of_startbytes;

    if (txString_length > 200)
        txString_length = 200;

    for (; i < num_of_startbytes; i++)
    {
        txString[i] = '\0';
    }

    for (; i < txString_length; i++)
    {
        txString[i] = buffer[i-num_of_startbytes];
    }

    txString[i++] = '\0';
    txString[i++] = '\0';
    txString[i++] = '\0';

    startSine();
    rf_on = 1;
    GPIO_SetBits(GPIOA, GPIO_Pin_2);    // Enable PTT
}
