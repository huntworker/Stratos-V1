#include "stm32f2xx_i2c.h"
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_rcc.h"

#include "i2c.h"

void P_I2C_timeout(I2C_TypeDef* I2Cx);

static uint8_t ret_wert[4] = {0};

void i2c_init_tmp(I2C_TypeDef* I2Cx)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);


	I2C_DeInit(I2Cx);
	I2C_InitTypeDef I2CInitStruct;
	I2CInitStruct.I2C_Ack = I2C_Ack_Enable;
	I2CInitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2CInitStruct.I2C_ClockSpeed = 20000;
	I2CInitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2CInitStruct.I2C_Mode = I2C_Mode_I2C;
	I2CInitStruct.I2C_OwnAddress1 = 0x88;
	I2C_Init(I2Cx, &I2CInitStruct);

	I2C_Cmd(I2Cx, ENABLE);

	I2C_AcknowledgeConfig(I2Cx, ENABLE);
}

//--------------------------------------------------------------
// Auslesen einer Adresse per I2C von einem Slave
// slave_adr => I2C-Basis-Adresse vom Slave
//
// Return_wert :
//  0...0xFFFF , Bytewert der gelesen wurde
//  < 0        , Error
//--------------------------------------------------------------
uint8_t* UB_I2C_Read(I2C_TypeDef* I2Cx, uint8_t slave_adr)
{
    uint32_t timeout = I2C_TIMEOUT;

  // Start-Sequenz
    I2C_GenerateSTART(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Slave-Adresse senden (read)
    I2C_Send7bitAddress(I2Cx, slave_adr, I2C_Direction_Receiver);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
    {
        if(timeout!=0)
            timeout--;
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ADDR-Flag löschen
    I2Cx->SR2;

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    ret_wert[0] = I2C_ReceiveData(I2Cx);

    I2C_AcknowledgeConfig(I2Cx, DISABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Stop-Sequenz
    I2C_GenerateSTOP(I2Cx, ENABLE);

    // Daten auslesen
    ret_wert[1] = I2C_ReceiveData(I2Cx);

    // ACK enable
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    return(ret_wert);
}

//--------------------------------------------------------------
// Auslesen einer Adresse per I2C von einem Slave
// slave_adr => I2C-Basis-Adresse vom Slave
// adr       => Register Adresse die gelesen wird
//
// Return_wert :
//  0...255 , Bytewert der gelesen wurde
//  < 0     , Error
//--------------------------------------------------------------
uint8_t* UB_I2C_ReadByte(I2C_TypeDef* I2Cx, uint8_t slave_adr, uint8_t adr)
{
    uint32_t timeout=I2C_TIMEOUT;

    // Start-Sequenz
    I2C_GenerateSTART(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ACK disable
    I2C_AcknowledgeConfig(I2Cx, DISABLE);

    // Slave-Adresse senden (write)
    I2C_Send7bitAddress(I2Cx, slave_adr, I2C_Direction_Transmitter);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ADDR-Flag löschen
    I2Cx->SR2;

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Adresse senden
    I2C_SendData(I2Cx, adr);

    timeout=I2C_TIMEOUT;
    while ((!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF)))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Start-Sequenz
    I2C_GenerateSTART(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Slave-Adresse senden (read)
    I2C_Send7bitAddress(I2Cx, slave_adr, I2C_Direction_Receiver);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ADDR-Flag löschen
    I2Cx->SR2;

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Stop-Sequenz
    I2C_GenerateSTOP(I2Cx, ENABLE);

    // Daten auslesen
    ret_wert[0]=I2C_ReceiveData(I2Cx);

    // ACK enable
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    return(ret_wert);
}

//--------------------------------------------------------------
// Auslesen einer Adresse per I2C von einem Slave
// slave_adr => I2C-Basis-Adresse vom Slave
// adr       => Register Adresse die gelesen wird
//
// Return_wert :
//  0...0xFFFF , Bytewert der gelesen wurde
//  < 0        , Error
//--------------------------------------------------------------
uint8_t* UB_I2C_ReadWord(I2C_TypeDef* I2Cx, uint8_t slave_adr, uint8_t adr)
{
    uint32_t timeout = I2C_TIMEOUT;

    // Start-Sequenz
    I2C_GenerateSTART(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ACK disable
    I2C_AcknowledgeConfig(I2Cx, DISABLE);

    // Slave-Adresse senden (write)
    I2C_Send7bitAddress(I2Cx, slave_adr, I2C_Direction_Transmitter);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ADDR-Flag löschen
    I2Cx->SR2;

    /*timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE)) {
        if(timeout!=0) timeout--; else return(P_I2C_timeout(-3));
    }*/

    // Adresse senden
    I2C_SendData(I2Cx, adr);

    timeout=I2C_TIMEOUT;
    while ((!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF)))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Start-Sequenz
    I2C_GenerateSTART(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Slave-Adresse senden (read)
    I2C_Send7bitAddress(I2Cx, slave_adr, I2C_Direction_Receiver);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ADDR-Flag löschen
    I2Cx->SR2;

    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    ret_wert[0] = I2C_ReceiveData(I2Cx);

    I2C_AcknowledgeConfig(I2Cx, DISABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Stop-Sequenz
    I2C_GenerateSTOP(I2Cx, ENABLE);

    // Daten auslesen
    ret_wert[1] = I2C_ReceiveData(I2Cx);

    // ACK enable
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    return(ret_wert);
}

//--------------------------------------------------------------
// Auslesen einer Adresse per I2C von einem Slave
// slave_adr => I2C-Basis-Adresse vom Slave
//
// Return_wert :
//  0...0xFFFF , Bytewert der gelesen wurde
//  < 0        , Error
//--------------------------------------------------------------
uint8_t* UB_I2C_Read24b(I2C_TypeDef* I2Cx, uint8_t slave_adr)
{
    uint32_t timeout = I2C_TIMEOUT;

    // Start-Sequenz
    I2C_GenerateSTART(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Slave-Adresse senden (read)
    I2C_Send7bitAddress(I2Cx, slave_adr, I2C_Direction_Receiver);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ADDR-Flag löschen
    I2Cx->SR2;

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    ret_wert[0] = I2C_ReceiveData(I2Cx);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    ret_wert[1] = I2C_ReceiveData(I2Cx);

    I2C_AcknowledgeConfig(I2Cx, DISABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Stop-Sequenz
    I2C_GenerateSTOP(I2Cx, ENABLE);

    // Daten auslesen
    ret_wert[2] = I2C_ReceiveData(I2Cx);

    // ACK enable
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    return(ret_wert);
}



//--------------------------------------------------------------
// Beschreiben einer Adresse per I2C von einem Slave
// slave_adr => I2C-Basis-Adresse vom Slave
// adr       => Register Adresse die beschrieben wird
// wert      => Bytewert der geschrieben wird
//
// Return_wert :
//    0   , Ok
//  < 0   , Error
//--------------------------------------------------------------
uint8_t UB_I2C_WriteByte(I2C_TypeDef* I2Cx, uint8_t slave_adr, uint8_t adr, uint8_t wert)
{
    uint8_t ret_wert=0;
    uint32_t timeout=I2C_TIMEOUT;

    // Start-Sequenz
    I2C_GenerateSTART(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Slave-Adresse senden (write)
    I2C_Send7bitAddress(I2Cx, slave_adr, I2C_Direction_Transmitter);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ADDR-Flag löschen
    I2Cx->SR2;

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Adresse senden
    I2C_SendData(I2Cx, adr);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Daten senden
    I2C_SendData(I2Cx, wert);

    timeout=I2C_TIMEOUT;
    while ((!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF)))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Stop-Sequenz
    I2C_GenerateSTOP(I2Cx, ENABLE);

    ret_wert=0; // alles ok

    return(ret_wert);
}

//--------------------------------------------------------------
// Senden eines Kommandos per I2C
// slave_adr => I2C-Basis-Adresse vom Slave
// cmd       => Kommando
//
// Return_wert :
//    0   , Ok
//  < 0   , Error
//--------------------------------------------------------------
uint8_t UB_I2C_WriteCommand(I2C_TypeDef* I2Cx, uint8_t slave_adr, uint8_t cmd)
{
    int16_t ret_wert=0;
    uint32_t timeout=I2C_TIMEOUT;

    // Start-Sequenz
    I2C_GenerateSTART(I2Cx, ENABLE);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Slave-Adresse senden (write)
    I2C_Send7bitAddress(I2Cx, slave_adr, I2C_Direction_Transmitter);

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // ADDR-Flag löschen
    I2Cx->SR2;

    timeout=I2C_TIMEOUT;
    while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Adresse senden
    I2C_SendData(I2Cx, cmd);

    timeout=I2C_TIMEOUT;
    while ((!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF)))
    {
        if(timeout!=0)
        {
            timeout--;
        }
        else
        {
            P_I2C_timeout(I2Cx);
            return(0);
        }
    }

    // Stop-Sequenz
    I2C_GenerateSTOP(I2Cx, ENABLE);

    ret_wert=0; // alles ok

    return(ret_wert);
}


//--------------------------------------------------------------
// kleine Pause (ohne Timer)
//--------------------------------------------------------------
void UB_I2C_Delay(volatile uint32_t nCount)
{
    while(nCount--)
    {
    }
}

//--------------------------------------------------------------
// interne Funktion
// wird bei einem Timeout aufgerufen
// Stop, Reset und reinit der I2C-Schnittstelle
//--------------------------------------------------------------
void P_I2C_timeout(I2C_TypeDef* I2Cx)
{
    // Stop und Reset
    I2C_GenerateSTOP(I2Cx, ENABLE);
    //I2C_SoftwareResetCmd(I2Cx, ENABLE);
    //I2C_SoftwareResetCmd(I2Cx, DISABLE);

    //for (uint16_t i = 0; i < 0x2000; i++);

    // I2C deinit
    //I2C_DeInit(I2Cx);
    // I2C init
    i2c_init_tmp(I2Cx);
}
