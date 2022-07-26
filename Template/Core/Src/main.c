/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include "USART.h"
#include "delay.h"
#include "defines.h"
/* Private includes ----------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
/**
 * 0: printf mediante el usart2
 * 1: printf mediante el SWO
 */
#define USE_SWV			0

#define USART2_TX		A, 2
#define USART2_RX		A, 3
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
uint8_t txbuffer[100];
uint8_t rxbuffer[100];
uint16_t len;

USART_Handle_t usart2;

/* Private function prototypes -----------------------------------------------*/
void USART2_GPIOInit(void);
void UART_Printf(char *format,...);

/* Private user code ---------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  delay_init_it();
  USART2_GPIOInit();
  USART_Init(USART2, 16E+6, 115200, 0);

  RCC->AHB1ENR |= GPIOX_CLOCK(LED);
  GPIOX_MODER(MODE_OUT, LED);
  RCC->AHB1ENR |= GPIOX_CLOCK(BUTTON);
  GPIOX_MODER(MODE_DIGITAL_INPUT, BUTTON);
//  //->IT
//  USART2->CR1 |=  USART_CR1_RXNEIE;
//  NVIC_EnableIRQ(USART2_IRQn);
//
//  len = sprintf((char*)txbuffer, "hola mundo IT\r\n");
//
//  USART2->CR1 |= USART_CR1_TXEIE;
  usart2.pUSARTx = USART2;
  USART_IRQInterruptConfig(USART2_IRQn, ENABLE);
  USART_IRQPriorityConfig(USART2_IRQn, 1);

  USART_ReceiveDataIT(&usart2, rxbuffer,1);
  /* Infinite loop */
  while (1)
  {
    if(GPIOX_IDR(BUTTON) == 0){
    	printf("1\r\n");
    }else{
    	printf("0\r\n");
    }
    delay_ms(50);
  }
}


void USART2_GPIOInit(void){
	RCC->AHB1ENR |= GPIOX_CLOCK(USART2_TX);

	GPIOX_MODER(MODE_ALTER, USART2_TX);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH, USART2_TX);
	GPIOX_AFR(7U, USART2_TX);

	GPIOX_MODER(MODE_ALTER, USART2_RX);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH, USART2_RX);
	GPIOX_AFR(7U, USART2_RX);
	return;
}


void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t event){
	if(event == USART_EVENT_RX_CMPLT){
		//todo
		USART_ReceiveDataIT(&usart2, rxbuffer, 1);
		if(rxbuffer[0] == '1'){
			GPIOX_ODR(LED) = 1;
		}else{
			GPIOX_ODR(LED) = 0;
		}
	}
}



void UART_Printf(char *format,...){
    char str[100];
    /*Extract the the argument list using VA apis */
    va_list args;
    va_start(args, format);
    vsprintf(str, format,args);
    USART_SendData(USART2, (uint8_t*)str, strlen(str));
    va_end(args);
}


/************************************************************/
int __io_putchar(int ch){
#if (USE_SWV== 1)
	ITM_SendChar((uint32_t)ch);
#else
	uint8_t c = ch & 0xFF;
	while(!(USART2->SR & USART_SR_TXE));  //espera hasta que usart este listo para transmitir otro byte
	USART2->DR = c;
#endif
	return ch;
}
