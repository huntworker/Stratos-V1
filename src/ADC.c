#include "stm32f2xx_adc.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_rcc.h"
#include "adc.h"

void my_ADC_Init()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);


    ADC_CommonInitTypeDef ADC_CommonInitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE);

    ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div8;
    ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStruct);

    ADC_StructInit(&ADC_InitStruct); // Struct leeren, sonst werden andere Bits überschrieben
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; // Dummy (=0)
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_Cmd(ADC1, ENABLE);
}

uint16_t UB_ADC1_AV_Read(uint8_t ADC_Channel, uint_fast16_t numOfSamples)
{
    uint32_t Sum = 0;

    for (int i = 0; i < numOfSamples; i++)
    {
        Sum += UB_ADC1_SINGLE_Read(ADC_Channel);
    }

    return (uint16_t)(Sum / numOfSamples);
}

uint16_t UB_ADC1_SINGLE_Read(uint8_t ADC_Channel)
{
    uint16_t messwert=0;

    // Messkanal einrichten
    ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_3Cycles);
    // Messung starten
    ADC_SoftwareStartConv(ADC1);
    // warte bis Messung fertig ist
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    // Messwert auslesen
    messwert=ADC_GetConversionValue(ADC1);

    return(messwert);
}
