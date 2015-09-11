#include "DAC.h"

#include "stm32f2xx_dac.h"
#include "stm32f2xx_dma.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_tim.h"
#include "stm32f2xx_rcc.h"

extern const uint16_t sine_table[];

DAC_InitTypeDef DAC_InitStruct;  // MUSS Global sein (BUGFIX)

void my_DAC_Init(void)
{
    //GPIO Config
    GPIO_InitTypeDef GPIO_InitStruct;

    // Ausgang DAC
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    //Trigger Timer Config
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); // 16MHz

    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Period = 15-1;   //alt: 15
    TIM_TimeBaseInitStruct.TIM_Prescaler = 1-1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
    TIM_ARRPreloadConfig(TIM6, ENABLE);

    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    TIM_Cmd(TIM6, ENABLE);

    //DAC Config
    //DAC_InitTypeDef MUSS Global sein sonst kaputt!

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStruct);

    DAC_Cmd(DAC_Channel_1, ENABLE);

    //startSine();

    // PTT
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_ResetBits(GPIOA, GPIO_Pin_5);    // Disable PTT

}

void startSine()
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    DMA_Cmd(DMA1_Stream5, DISABLE);
    DMA_DeInit(DMA1_Stream5);
    DMA_InitStruct.DMA_Channel = DMA_Channel_7;
    DMA_InitStruct.DMA_PeripheralBaseAddr = DAC_BASE + 0x08;    // Addresse von DAC_DHR12R1
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)&sine_table;
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    //DMA_InitStruct.DMA_BufferSize = sizeof(sine_table)/sizeof(uint16_t);   //Länge der Sinustabelle
    DMA_InitStruct.DMA_BufferSize = 738;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStruct);

    DMA_Cmd(DMA1_Stream5, ENABLE);

    DAC_DMACmd(DAC_Channel_1, ENABLE);
}

void stopSine()
{
    DMA_Cmd(DMA1_Stream5, DISABLE);

    DAC_DMACmd(DAC_Channel_1, DISABLE);
}

void transmit(uint8_t value)
{
    if (value)
        TIM_SetAutoreload(TIM6, 15-1); // alt: 15
    else
        TIM_SetAutoreload(TIM6, 17-1); // alt: 17
}
