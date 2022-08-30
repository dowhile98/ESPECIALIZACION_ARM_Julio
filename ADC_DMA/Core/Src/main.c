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

#define ADC1_IN0		A , 0
#define ADC1_IN1		A , 1
#define ADC1_IN4		A , 4
#define ADC1_IN8		B , 0
#define ADC1_IN10		C , 0
#define ADC1_IN11		C , 1
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
uint8_t txbuffer[100];
uint8_t rxbuffer[100];
uint16_t len;


/* Private function prototypes -----------------------------------------------*/
void USART2_GPIOInit(void);
void UART_Printf(char *format,...);

void ADC1_Init(void);
void ADC1_CHannelsInit(void);
/* Private user code ---------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	uint16_t data[3];
	delay_init_it();
	USART2_GPIOInit();
	USART_Init(USART2, 16E+6, 115200, 0);
	RCC->AHB1ENR |= GPIOX_CLOCK(LED);
	GPIOX_MODER(MODE_OUT, LED);
	RCC->AHB1ENR |= GPIOX_CLOCK(BUTTON);
	GPIOX_MODER(MODE_DIGITAL_INPUT, BUTTON);
	/***************************************************/
	ADC1_CHannelsInit();
	ADC1_Init();
	//TRIGGER

	/* Infinite loop */
	while (1)
	{
		//software trigger
		ADC1->CR2 |= ADC_CR2_SWSTART;
		//polling ->1st
		while(!(ADC1->SR & ADC_SR_EOC));		//wait
		data[0] = ADC1->DR;
		//polling -> 2nd
		while(!(ADC1->SR & ADC_SR_EOC));		//wait
		data[1] = ADC1->DR;
		//polling -> 3th
		while(!(ADC1->SR & ADC_SR_EOC));		//wait
		data[2] = ADC1->DR;
		printf("%u,%u,%u\r\n",data[0],data[1],data[2]);
		delay_ms(100);
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


void ADC1_Init(void){
	/*habilitar el reloj*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	/*configurar el adc */
	ADC1_COMMON->CCR &=~ADC_CCR_ADCPRE;		//PCLK2 / 2 = 8MHZ
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;
	//modo
	ADC1->CR1 |= ADC_CR1_SCAN;				//modo escaneo
	ADC1->CR2 |= ADC_CR2_EOCS;				//flag para cada conversion
	//secuencia de las conversion
	//numero
	ADC1->SQR1 = (3 - 1)<<20;
	//orden
	ADC1->SQR3 |= 0<<10 | 1<<5 | 4<<0;		//CH4->CH1->CH0
	//SAMPLING
	ADC1->SMPR2 = 0x3U;						//56 CYCLES DE RELOJ
	ADC1->SMPR2 |= 0x5U<<12;
	//HABILITAR EL ADC
	ADC1->CR2 |= ADC_CR2_ADON;
	return;
}
void ADC1_CHannelsInit(void){
	RCC->AHB1ENR |= GPIOX_CLOCK(ADC1_IN0) | GPIOX_CLOCK(ADC1_IN1) | GPIOX_CLOCK(ADC1_IN4);
	//FUNCION ANALOGICA
	GPIOX_MODER(MODE_ANALOG_INPUT, ADC1_IN0);
	GPIOX_MODER(MODE_ANALOG_INPUT, ADC1_IN1);
	GPIOX_MODER(MODE_ANALOG_INPUT, ADC1_IN4);
	//NO PULL UP / PULL DOWN
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN0);
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN1);
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN4);
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
