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
uint8_t txbuffer[50];
uint8_t rxbuffer[100];
uint16_t len;



/* Private function prototypes -----------------------------------------------*/
void USART2_GPIOInit(void);
void UART_Printf(char *format,...);
void DMA1_Stream6_Init(void);
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
  DMA1_Stream6_Init();
  printf("dma1\r\n");
  sprintf((char*)txbuffer, "hola mundo desde dma1\r\n");
  //iniciar la comunicacion
  USART2->CR3 |= USART_CR3_DMAT;
  /* Infinite loop */
  while (1)
  {

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





void UART_Printf(char *format,...){
    char str[100];
    /*Extract the the argument list using VA apis */
    va_list args;
    va_start(args, format);
    vsprintf(str, format,args);
    USART_SendData(USART2, (uint8_t*)str, strlen(str));
    va_end(args);
}
void DMA1_Stream6_Init(void){
	/*Habilitar el reloj*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	//STREAM 6 -> CH4
	DMA1_Stream6->CR = 0;
	//Configurar la direccion fuente de la transferencia
	DMA1_Stream6->M0AR = (uint32_t)txbuffer;
	//configura la direccion destino
	DMA1_Stream6->PAR = (uint32_t) &USART2->DR;
	//Configurar la longitud de datos a transmitir
	DMA1_Stream6->NDTR = sizeof(txbuffer);
	//configurar el modo de la transferencia
	DMA1_Stream6->CR |= 0x1<<6;				//mem - periph
	//programar el tamaño de datos a transmitir
	DMA1_Stream6->CR &=~(uint32_t)(0x3<<13);		//Byte (8-bit)
	DMA1_Stream6->CR &=~(uint32_t)(0x3<<11);		//Byte (8-bit)
	//modo de transferencia (direct - fifo)
	DMA1_Stream6->FCR = 0;							//direct mode
	DMA1_Stream6->FCR |= 1<<2;
	//se asigna el canal con el que se va trabajar
	DMA1_Stream6->CR &=~ 0x7u<<25;
	DMA1_Stream6->CR |= 0x4U<<25;					//ch4
	//incremento de la memoria
	DMA1_Stream6->CR |= DMA_SxCR_MINC;				//incremto del MoAR
/**************************************************/
	DMA1_Stream6->CR |= DMA_SxCR_TCIE | DMA_SxCR_DMEIE | DMA_SxCR_TEIE;
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
/**************************************************/
	//habilitar el DMA
	DMA1_Stream6->CR |= 0x1<<6;				//mem - periph
	DMA1_Stream6->CR |= DMA_SxCR_EN;
	return;
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
