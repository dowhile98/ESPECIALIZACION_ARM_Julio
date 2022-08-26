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
#define TIMER2_CH1		A, 0
#define TIMER3_CH2		B, 5
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
uint8_t txbuffer[100];
uint8_t rxbuffer[100];
uint16_t len;

float duty;
float freq;

/* Private function prototypes -----------------------------------------------*/
void USART2_GPIOInit(void);

void TIMER2_IC_PWM_Mode(void);

void get_duty_freq(void);

void TIMER3_CH2_PWM(void);
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
  TIMER2_IC_PWM_Mode();
  TIMER3_CH2_PWM();
  RCC->AHB1ENR |= GPIOX_CLOCK(LED);
  GPIOX_MODER(MODE_OUT, LED);
  RCC->AHB1ENR |= GPIOX_CLOCK(BUTTON);
  GPIOX_MODER(MODE_DIGITAL_INPUT, BUTTON);

  /* Infinite loop */
  while (1)
  {
	  TIM3->CCR2 = 200;
	  get_duty_freq();
	  delay_ms(100);

	  TIM3->CCR2 = 1000;
	  get_duty_freq();
	  delay_ms(100);

	  TIM3->CCR2 = 10000;
	  get_duty_freq();
	  delay_ms(100);

	  TIM3->CCR2 = 15000;
	  get_duty_freq();
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



void TIMER2_IC_PWM_Mode(void){
	//configurar el pin asignado al canal 1
	RCC->AHB1ENR |= GPIOX_CLOCK(TIMER2_CH1);
	GPIOX_MODER(MODE_ALTER, TIMER2_CH1);
	GPIOX_AFR(1, TIMER2_CH1);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH, TIMER2_CH1);
	//configurar el timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->CR1 = 0;
	TIM2->PSC = 4 - 1;
	TIM2->ARR = 0xFFFFFFFF; 		//Valor maximo de conteo de 32-bits
	//configurar el modo de funcionamiento del canal
	//CCMRx (x = 1, 2) -> CCER -> DIER
	TIM2->CCMR1 &=~ TIM_CCMR1_CC1S;
	TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;	//TI1 -> CCR1
	TIM2->CCER &=~ (TIM_CCER_CC1P | TIM_CCER_CC1NP);	//RISING
	TIM2->CCMR1 &=~ TIM_CCMR1_CC2S;
	TIM2->CCMR1 |= TIM_CCMR1_CC2S_1;	//TI1 -> CCR2
	TIM2->CCER |= TIM_CCER_CC1P;		//FALLING
	TIM2->SMCR = 0x5<<4;
	TIM2->SMCR |= 0x4U;					//RESET MODE
	/*Habilitar los canales*/
	TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC1E;
/****************************************/

/****************************************/
	//HABILITAR EL CONTEO

}

void get_duty_freq(void){
	uint32_t capture[2];
	//habilitamos el conteo
	TIM2->SR &=~ (TIM_SR_CC1IF | TIM_SR_CC2IF);		//clear
	TIM2->CR1 |= TIM_CR1_CEN;
	//se espera el primer flanco asendente
	while(!(TIM2->SR & TIM_SR_CC1IF));
	TIM2->SR &=~ (TIM_SR_CC1IF);		//clear
	//Se espera el flanco descendente
	while(!(TIM2->SR & TIM_SR_CC2IF));
	TIM2->SR &=~ (TIM_SR_CC2IF);		//clear
	capture[0] = TIM2->CCR2;
	//se espera el flanco ascendente
	while(!(TIM2->SR & TIM_SR_CC1IF));
	TIM2->SR &=~ (TIM_SR_CC1IF);		//clear
	capture[1] = TIM2->CCR1;
	//duty
	duty = ((float)capture[0]/capture[1]) * 100;
	//frecuencia
	freq = ((float)SystemCoreClock /(4* capture[1])) ;
	//DESHABILITAR EL TIMER
	TIM2->CR1 &=~ TIM_CR1_CEN;
	return;
}
void TIMER3_CH2_PWM(void){
	//configurar el pin
	RCC->AHB1ENR |= GPIOX_CLOCK(TIMER3_CH2);
	GPIOX_MODER(MODE_ALTER, TIMER3_CH2);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH, TIMER3_CH2);
	GPIOX_AFR(2, TIMER3_CH2);
	//configurar el timer
	/**
	 * F = CK_PSC / (PSC + 1) (ARR + 1)
	 * F = 50 ->
	 * ARR = 16E+6 /(16) (50) - 1
	 * ARR = 20000 - 1
	 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 = 0;
	TIM3->PSC = 16 - 1;
	TIM3->ARR = 20000 - 1;
	//configurar el canal
	TIM3->CCMR1 &=~ TIM_CCMR1_CC1S;			//SALIDA
	TIM3->CCMR1 &=~ TIM_CCMR1_OC2M;			//CLEAR
	TIM3->CCMR1 |= 0x6U<<12;				//PWM MODO 1
	TIM3->CCER |= TIM_CCER_CC2E;			//se habilita el canal

	TIM3->CR1 |= TIM_CR1_CEN;
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
