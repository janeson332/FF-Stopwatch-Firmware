/**
  ******************************************************************************
  * @file    stm32f10x_uart1.c
  * @author  Jahn Stefan
  * @version V1.0
  * @date    03.12.2017
  * @brief   functions to operate with the usart1 as uart
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <assert.h>

#include "Utils/Ringbuffer.h"
#include "stm32f40x_uart1.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USARTx_TX_PIN                    GPIO_Pin_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_SOURCE                 GPIO_PinSource9

#define USARTx_RX_PIN                    GPIO_Pin_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_SOURCE                 GPIO_PinSource10


#define TX_BUFFER_SIZE 256
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t tx_buffer[TX_BUFFER_SIZE] = {0};
static CircularBufferType_t txBufferStruct;

/* Private function prototypes -----------------------------------------------*/
static void DummyFunc(uint8_t ch){return;}
static void (*RCParserFunc)(uint8_t ch) = &DummyFunc;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief interrupt handler for the usart1
  */
void USART1_IRQHandler(void){
    uint16_t txChar = 0;
    //transmit
    if((USART1->SR & USART_SR_TXE) && !Ringbuffer_IsEmpty(&txBufferStruct)){
        if (Ringbuffer_Pop(&txBufferStruct,&txChar)){
        	USART1->DR = txChar;
        }
    }

    if(Ringbuffer_IsEmpty(&txBufferStruct)){
        USART1->CR1 &= ~USART_CR1_TXEIE;
    }

    //receive
    if((USART1->SR & USART_SR_RXNE)){
        char ch = USART1->DR;
        (*RCParserFunc)(ch);
    }
}

/**
  * @brief initializes the uart1
  */
void UART1_Init(void){
    Ringbuffer_Init(&txBufferStruct,tx_buffer,TX_BUFFER_SIZE,sizeof(uint8_t));

    // enable clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN,ENABLE);



    GPIO_InitTypeDef gpioInitStruct = {0};
    USART_InitTypeDef uartInitStruct = {0};

    //initialize gpios
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF;
    gpioInitStruct.GPIO_OType= GPIO_OType_PP;
    gpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioInitStruct.GPIO_Pin = USARTx_TX_PIN;
    GPIO_Init(GPIOA, &gpioInitStruct);

    gpioInitStruct.GPIO_Pin =  USARTx_RX_PIN;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &gpioInitStruct);

    GPIO_PinAFConfig(USARTx_TX_GPIO_PORT,USARTx_TX_SOURCE,GPIO_AF_USART1);
    GPIO_PinAFConfig(USARTx_RX_GPIO_PORT,USARTx_RX_SOURCE,GPIO_AF_USART1);

    uartInitStruct.USART_BaudRate =  115200;
    uartInitStruct.USART_WordLength = USART_WordLength_8b;
    uartInitStruct.USART_StopBits = USART_StopBits_1;
    /* When using Parity the word length must be configured to 9 bits */
    uartInitStruct.USART_Parity = USART_Parity_No;
    uartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &uartInitStruct);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    NVIC_SetPriority(USART1_IRQn,5);
    NVIC_EnableIRQ(USART1_IRQn);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief sends 1 char
  * @param ch: char to send
  */
void UART1_SendChar(uint8_t ch){


    if((USART1->SR & USART_SR_TXE) == USART_SR_TXE && Ringbuffer_IsEmpty(&txBufferStruct)){
    	USART1->DR = (uint16_t)(ch);
    }
    else{
        __disable_irq();
        USART1->CR1 |= USART_CR1_TXEIE;
        Ringbuffer_Push(&txBufferStruct,&ch);
        __enable_irq();
    }
}

/**
  * @brief sends a string (without \0)
  * @param str: string to send
  */
void UART1_SendString(char const *str){
    assert(str != 0);
    uint32_t strLength = strlen(str);

    for(uint32_t i = 0; i<strLength;++i){
        UART1_SendChar(str[i]);
    }
}

/**
  * @brief sets a receiver function
  * @param ParserFunc: function pointer to call after interrupt
  */
void UART1_SetReceiveParser(void (*ParserFunc)(uint8_t ch)){
    if(ParserFunc == 0){
        return;
    }
    RCParserFunc=ParserFunc;
}

