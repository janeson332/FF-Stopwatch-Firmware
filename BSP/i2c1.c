/*******************************************************************************
File    : i2c.c
Purpose : I2c 3 to communicate with the sensors
Author  : 
********************************** Includes ***********************************/

#include <i2c1.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h" 
#include "stm32f4xx_i2c.h" 
#include "stm32f4xx_rcc.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "FreeRTOS.h"

/********************************* Defines ************************************/

#define I2Cx_SPEED                 100000
#define I2Cx_OWN_ADDRESS           0x00

#define I2Cx_Peripheral               I2C1
#define I2Cx_FLAG_TIMEOUT             ((uint32_t) 900) //0x1100
#define I2Cx_LONG_TIMEOUT             ((uint32_t) (300 * I2Cx_FLAG_TIMEOUT)) //was300
 

#define I2Cx_SCL_GPIO_PORT         GPIOB
#define I2Cx_SCL_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define I2Cx_SCL_GPIO_PIN          GPIO_Pin_6
#define I2Cx_SCL_GPIO_PINSOURCE    GPIO_PinSource6
 
#define I2Cx_SDA_GPIO_PORT         GPIOB
#define I2Cx_SDA_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define I2Cx_SDA_GPIO_PIN          GPIO_Pin_7
#define I2Cx_SDA_GPIO_PINSOURCE    GPIO_PinSource7

#define I2Cx_RCC_CLK               RCC_APB1Periph_I2C1
#define I2Cx_AF                    GPIO_AF_I2C1


#define WAIT_FOR_FLAG(flag, value, timeout, errorcode)  I2CTimeout = timeout;\
          while(I2C_GetFlagStatus(I2Cx_Peripheral, flag) != value) {\
            if((I2CTimeout--) == 0) return I2Cx_TIMEOUT_UserCallback(errorcode); \
          }\
  
#define CLEAR_ADDR_BIT      I2C_ReadRegister(I2Cx_Peripheral, I2C_Register_SR1);\
                            I2C_ReadRegister(I2Cx_Peripheral, I2C_Register_SR2);\
                               
/********************************* Globals ************************************/

/********************************* Prototypes *********************************/
static void Set_I2C_Retry(unsigned short ml_sec);
static unsigned short Get_I2C_Retry();
static unsigned long ST_Sensors_I2C_WriteRegister(unsigned char Address, unsigned char RegisterAddr, unsigned short RegisterLen, const unsigned char *RegisterValue);
static unsigned long ST_Sensors_I2C_ReadRegister(unsigned char Address, unsigned char RegisterAddr, unsigned short RegisterLen, unsigned char *RegisterValue);
/*******************************  Function ************************************/

static void mdelay(uint32_t msec){
//	uint32_t now = Systick_GetMillis();
//	while(Systick_GetMillis()-now != msec);
	vTaskDelay(msec);
}

void I2C1_Init(void)
{
	mdelay(50);
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;

    /* Enable I2Cx clock */
  RCC_APB1PeriphClockCmd(I2Cx_RCC_CLK, ENABLE);

  /* Enable I2C GPIO clock */
  RCC_AHB1PeriphClockCmd(I2Cx_SCL_GPIO_CLK | I2Cx_SDA_GPIO_CLK, ENABLE);

  /* Configure I2Cx pin: SCL ----------------------------------------*/
  GPIO_InitStructure.GPIO_Pin =  I2Cx_SCL_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
 // GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  /* Connect pins to Periph */
  GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_GPIO_PINSOURCE, I2Cx_AF);
  GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);

  /* Configure I2Cx pin: SDA ----------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = I2Cx_SDA_GPIO_PIN;

  /* Connect pins to Periph */
  GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_GPIO_PINSOURCE, I2Cx_AF);
  GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);
  
  I2C_DeInit(I2Cx_Peripheral);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2Cx_SPEED;
    
  /* Initialize the I2C peripheral */
  I2C_Init(I2Cx_Peripheral, &I2C_InitStructure);

  /* Enable the I2C peripheral */
  I2C_Cmd(I2Cx_Peripheral, ENABLE);
    
  

  return;
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
static uint32_t I2Cx_TIMEOUT_UserCallback(char value)
{

  /* The following code allows I2C error recovery and return to normal communication
     if the error source doesn’t still exist (ie. hardware issue..) */
  I2C_InitTypeDef I2C_InitStructure;
  
  I2C_GenerateSTOP(I2Cx_Peripheral, ENABLE);
  I2C_SoftwareResetCmd(I2Cx_Peripheral, ENABLE);
  I2C_SoftwareResetCmd(I2Cx_Peripheral, DISABLE);
  
   I2C_DeInit(I2Cx_Peripheral);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2Cx_SPEED;

  /* Initialize the I2C peripheral */
  I2C_Init(I2Cx_Peripheral, &I2C_InitStructure);

  /* Enable the I2C peripheral */
  I2C_Cmd(I2Cx_Peripheral, ENABLE);
    
  return 1;
}


int I2C1_WriteRegister(unsigned char slave_addr,
                                        unsigned char reg_addr,
                                        unsigned short len,
                                        const unsigned char *data_ptr)
{
  char retries=0;
  int ret = 0;
  unsigned short retry_in_mlsec = Get_I2C_Retry();
                              
tryWriteAgain:  
  ret = 0;
  ret = ST_Sensors_I2C_WriteRegister( slave_addr, reg_addr, len, data_ptr); 

  if(ret && retry_in_mlsec)
  {
    if( retries++ > 4 )
        return ret;
    
    mdelay(retry_in_mlsec);
    goto tryWriteAgain;
  }
  return ret;  
}

int I2C1_ReadRegister(unsigned char slave_addr,
                                       unsigned char reg_addr,
                                       unsigned short len,
                                       unsigned char *data_ptr)
{
  char retries=0;
  int ret = 0;
  unsigned short retry_in_mlsec = Get_I2C_Retry();
  
tryReadAgain:  
  ret = 0;
  ret = ST_Sensors_I2C_ReadRegister( slave_addr, reg_addr, len, data_ptr);

  if(ret && retry_in_mlsec)
  {
    if( retries++ > 0 )
        return ret;
    
    mdelay(retry_in_mlsec);
    goto tryReadAgain;
  } 
  return ret;
}


/**
  * @brief  Writes a Byte to a given register to the sensors through the 
            control interface (I2C)
  * @param  RegisterAddr: The address (location) of the register to be written.
  * @param  RegisterValue: the Byte value to be written into destination register.
  * @retval 0 if correct communication, else wrong communication
  */
static unsigned long ST_Sensors_I2C_WriteRegister(unsigned char Address, unsigned char RegisterAddr, unsigned short RegisterLen, const unsigned char *RegisterValue)
{
  uint32_t  result = 0;
  uint32_t  i = 0; // i = RegisterLen;
  __IO uint32_t  I2CTimeout = I2Cx_LONG_TIMEOUT;
  
//  RegisterValue = RegisterValue + (RegisterLen - 1);

  /* Wait for the busy flag to be cleared */
  WAIT_FOR_FLAG (I2C_FLAG_BUSY, RESET, I2Cx_LONG_TIMEOUT, 1);
  
  /* Start the config sequence */
  I2C_GenerateSTART(I2Cx_Peripheral, ENABLE);

  /* Wait for the start bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_SB, SET, I2Cx_FLAG_TIMEOUT, 2);

  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2Cx_Peripheral, (Address<<1), I2C_Direction_Transmitter);
  
  /* Wait for address bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_ADDR, SET, I2Cx_FLAG_TIMEOUT, 3);
  
  /* clear the ADDR interrupt bit  - this is done by reading SR1 and SR2*/
  CLEAR_ADDR_BIT
  
  /* Wait for address bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_TXE, SET, I2Cx_FLAG_TIMEOUT, 4);

  /* Transmit the first address for write operation */
  I2C_SendData(I2Cx_Peripheral, RegisterAddr);

//  while (i--)
//  {
//    /* Wait for address bit to be set */
//    WAIT_FOR_FLAG (I2C_FLAG_TXE, SET, I2Cx_FLAG_TIMEOUT, 5);
//  
//    /* Prepare the register value to be sent */
//    I2C_SendData(SENSORS_I2C, *RegisterValue--);
//  }
  
  for(i=0; i<(RegisterLen); i++)
  {
    /* Wait for address bit to be set */
    WAIT_FOR_FLAG (I2C_FLAG_TXE, SET, I2Cx_FLAG_TIMEOUT, 5);
  
    /* Prepare the register value to be sent */
    I2C_SendData(I2Cx_Peripheral, RegisterValue[i]);
  }  
   
  /* Wait for address bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_BTF, SET, I2Cx_FLAG_TIMEOUT, 6);
  
  /* End the configuration sequence */
  I2C_GenerateSTOP(I2Cx_Peripheral, ENABLE);
  
  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return result;  
}

int I2C1_Read(unsigned char Adress, unsigned char data){
	return ST_Sensors_I2C_ReadRegister(Adress,data,0,0);
}

static unsigned long ST_Sensors_I2C_ReadRegister(unsigned char Address, unsigned char RegisterAddr, unsigned short RegisterLen, unsigned char *RegisterValue)
{
  uint32_t i=0, result = 0;
  __IO uint32_t  I2CTimeout = I2Cx_LONG_TIMEOUT;
   
  /* Wait for the busy flag to be cleared */
  WAIT_FOR_FLAG (I2C_FLAG_BUSY, RESET, I2Cx_LONG_TIMEOUT, 7);
  
  /* Start the config sequence */
  I2C_GenerateSTART(I2Cx_Peripheral, ENABLE);

  /* Wait for the start bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_SB, SET, I2Cx_FLAG_TIMEOUT, 8);
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2Cx_Peripheral, (Address<<1), I2C_Direction_Transmitter);

  /* Wait for the start bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_ADDR, SET, I2Cx_FLAG_TIMEOUT, 9);

  /* clear the ADDR interrupt bit  - this is done by reading SR1 and SR2*/
  CLEAR_ADDR_BIT;
  
  /* Wait for address bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_TXE, SET, I2Cx_FLAG_TIMEOUT, 10);
  
  /* Transmit the register address to be read */
  I2C_SendData(I2Cx_Peripheral, RegisterAddr);
  
  /* Wait for address bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_TXE, SET, I2Cx_FLAG_TIMEOUT, 11);  

  /*!< Send START condition a second time */  
  I2C_GenerateSTART(I2Cx_Peripheral, ENABLE);
  
  /* Wait for the start bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_SB, SET, I2Cx_FLAG_TIMEOUT, 12);
  
  /*!< Send address for read */
  I2C_Send7bitAddress(I2Cx_Peripheral, (Address<<1), I2C_Direction_Receiver);
  
  /* Wait for the start bit to be set */
  WAIT_FOR_FLAG (I2C_FLAG_ADDR, SET, I2Cx_FLAG_TIMEOUT, 13);
  
  if (RegisterLen == 1) 
  {
    /*!< Disable Acknowledgment */
    I2C_AcknowledgeConfig(I2Cx_Peripheral, DISABLE);
    
    /* clear the ADDR interrupt bit  - this is done by reading SR1 and SR2*/
    CLEAR_ADDR_BIT;
    
    /*!< Send STOP Condition */
    I2C_GenerateSTOP(I2Cx_Peripheral, ENABLE);
    
    /* Wait for the RXNE bit to be set */
    WAIT_FOR_FLAG (I2C_FLAG_RXNE, SET, I2Cx_FLAG_TIMEOUT, 14);
    
    RegisterValue[0] = I2C_ReceiveData(I2Cx_Peripheral);
  } 
  else if( RegisterLen == 2) 
  {
     /*!< Disable Acknowledgment */
    I2C_AcknowledgeConfig(I2Cx_Peripheral, DISABLE);
    
   /* Set POS bit */ 
    I2Cx_Peripheral->CR1 |= I2C_CR1_POS;
   
   /* clear the ADDR interrupt bit  - this is done by reading SR1 and SR2*/
   CLEAR_ADDR_BIT; 
   
   /* Wait for the buffer full bit to be set */
   WAIT_FOR_FLAG (I2C_FLAG_BTF, SET, I2Cx_FLAG_TIMEOUT, 15);
   
   /*!< Send STOP Condition */
   I2C_GenerateSTOP(I2Cx_Peripheral, ENABLE);

   /* Read 2 bytes */
   RegisterValue[0] = I2C_ReceiveData(I2Cx_Peripheral);
   RegisterValue[1] = I2C_ReceiveData(I2Cx_Peripheral);
  } 
  else if( RegisterLen == 3)
  {
    CLEAR_ADDR_BIT;
    
    /* Wait for the buffer full bit to be set */
    WAIT_FOR_FLAG (I2C_FLAG_BTF, SET, I2Cx_FLAG_TIMEOUT, 16);
    /*!< Disable Acknowledgment */
    I2C_AcknowledgeConfig(I2Cx_Peripheral, DISABLE);
    /* Read 1 bytes */
    RegisterValue[0] = I2C_ReceiveData(I2Cx_Peripheral);
    /*!< Send STOP Condition */
    I2C_GenerateSTOP(I2Cx_Peripheral, ENABLE);
    /* Read 1 bytes */
    RegisterValue[1] = I2C_ReceiveData(I2Cx_Peripheral);
    /* Wait for the buffer full bit to be set */
    WAIT_FOR_FLAG (I2C_FLAG_RXNE, SET, I2Cx_FLAG_TIMEOUT, 17);
    /* Read 1 bytes */
    RegisterValue[2] = I2C_ReceiveData(I2Cx_Peripheral);
  }  
  else /* more than 2 bytes */
  { 
    /* clear the ADDR interrupt bit  - this is done by reading SR1 and SR2*/
    CLEAR_ADDR_BIT;
    
    for(i=0; i<(RegisterLen); i++)
    {
      if(i==(RegisterLen-3))
      {
        /* Wait for the buffer full bit to be set */
        WAIT_FOR_FLAG (I2C_FLAG_BTF, SET, I2Cx_FLAG_TIMEOUT, 16);
        
        /*!< Disable Acknowledgment */
        I2C_AcknowledgeConfig(I2Cx_Peripheral, DISABLE);
        
        /* Read 1 bytes */
        RegisterValue[i++] = I2C_ReceiveData(I2Cx_Peripheral);
        
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(I2Cx_Peripheral, ENABLE);
        
        /* Read 1 bytes */
        RegisterValue[i++] = I2C_ReceiveData(I2Cx_Peripheral);
        
        /* Wait for the buffer full bit to be set */
        WAIT_FOR_FLAG (I2C_FLAG_RXNE, SET, I2Cx_FLAG_TIMEOUT, 17);
        
        /* Read 1 bytes */
        RegisterValue[i++] = I2C_ReceiveData(I2Cx_Peripheral);
        goto endReadLoop;
      }
            
      /* Wait for the RXNE bit to be set */
      WAIT_FOR_FLAG (I2C_FLAG_RXNE, SET, I2Cx_FLAG_TIMEOUT, 18);
      RegisterValue[i] = I2C_ReceiveData(I2Cx_Peripheral);
    }   
  } 
  
endReadLoop:  
  /* Clear BTF flag */
  I2C_ClearFlag(I2Cx_Peripheral, I2C_FLAG_BTF);
  /* Wait for the busy flag to be cleared */
  WAIT_FOR_FLAG (I2C_FLAG_BUSY, RESET, I2Cx_LONG_TIMEOUT, 19);  
  /*!< Re-Enable Acknowledgment to be ready for another reception */
  I2C_AcknowledgeConfig(I2Cx_Peripheral, ENABLE);
  //Disable POS -- TODO
  I2Cx_Peripheral->CR1 &= ~I2C_CR1_POS;
     
  /* Return the byte read from sensor */
  return result;
}

static unsigned short RETRY_IN_MLSEC  = 55;

static void Set_I2C_Retry(unsigned short ml_sec)
{
  RETRY_IN_MLSEC = ml_sec;
}

static unsigned short Get_I2C_Retry()
{
  return RETRY_IN_MLSEC;
}
