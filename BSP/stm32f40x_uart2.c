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
#include "stm32f40x_uart2.h"
#include "Utils/Ringbuffer.h"
#include <string.h>
#include <assert.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USARTx_TX_PIN                    GPIO_Pin_2
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_SOURCE                 GPIO_PinSource2

#define USARTx_RX_PIN                    GPIO_Pin_3
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_SOURCE                 GPIO_PinSource3


#define TX_BUFFER_SIZE 256
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t tx_buffer[TX_BUFFER_SIZE] = {0};
static tCircularBuffer txBufferStruct;

/* Private function prototypes -----------------------------------------------*/
static void DummyFunc(uint8_t ch){return;}
static void (*RCParserFunc)(uint8_t ch) = &DummyFunc;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief interrupt handler for the usart1
  */
void USART2_IRQHandler(void){
    uint16_t txChar = 0;
    //transmit
    if((USART2->SR & USART_SR_TXE) && !Ringbuffer_IsEmpty(&txBufferStruct)){
        if (Ringbuffer_Pop(&txBufferStruct,&txChar)){
        	USART2->DR = txChar;
        }
    }

    if(Ringbuffer_IsEmpty(&txBufferStruct)){
        USART2->CR1 &= ~USART_CR1_TXEIE;
    }

    //receive
    if((USART2->SR & USART_SR_RXNE)){
        char ch = USART2->DR;
        (*RCParserFunc)(ch);
    }
}

/**
  * @brief initializes the uart1
  */
void UART2_Init(void){
    Ringbuffer_Init(&txBufferStruct,tx_buffer,TX_BUFFER_SIZE,sizeof(uint8_t));

    // enable clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN,ENABLE);


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

    //GPIO_PinAFConfig(USARTx_TX_PIN,USARTx_TX_SOURCE,GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
    //GPIO_PinAFConfig(USARTx_RX_PIN,USARTx_RX_SOURCE,GPIO_AF_USART2);

    uartInitStruct.USART_BaudRate =  115200;
    uartInitStruct.USART_WordLength = USART_WordLength_8b;
    uartInitStruct.USART_StopBits = USART_StopBits_1;
    /* When using Parity the word length must be configured to 9 bits */
    uartInitStruct.USART_Parity = USART_Parity_No;
    uartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &uartInitStruct);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    NVIC_EnableIRQ(USART2_IRQn);

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);
}

/**
  * @brief sends 1 char
  * @param ch: char to send
  */
void UART2_SendChar(uint8_t ch){


    if((USART2->SR & USART_SR_TXE) == USART_SR_TXE && Ringbuffer_IsEmpty(&txBufferStruct)){
    	USART2->DR = (uint16_t)(ch);
    }
    else{
        __disable_irq();
        USART2->CR1 |= USART_CR1_TXEIE;
        Ringbuffer_Push(&txBufferStruct,&ch);
        __enable_irq();
    }
}

/**
  * @brief sends a string (without \0)
  * @param str: string to send
  */
void UART2_SendString(char const *str){
    assert(str != 0);
    uint32_t strLength = strlen(str);

    for(uint32_t i = 0; i<strLength;++i){
        UART2_SendChar(str[i]);
    }
}

/**
  * @brief sets a receiver function
  * @param ParserFunc: function pointer to call after interrupt
  */
void UART2_SetReceiveParser(void (*ParserFunc)(uint8_t ch)){
    if(ParserFunc == 0){
        return;
    }
    RCParserFunc=ParserFunc;
}

