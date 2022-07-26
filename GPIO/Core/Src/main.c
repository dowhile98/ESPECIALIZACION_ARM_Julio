/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
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

#include <stdint.h>
#include <stm32f4xx.h>
#include <stdio.h>

#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))


int main(void)
{
	uint16_t bit;

	ITM_Port32(0) = 1;
	//HABILITAR EL RELOJ
	printf("HABILITANDO EL RELOJ\r\n");
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;
	//LED -> PA5
	//MODO
	printf("CONFIGURANDO EL PIN PA5\r\n");
	GPIOA->MODER &=~ (GPIO_MODER_MODE5);
	GPIOA->MODER |= GPIO_MODER_MODE5_0;			//salida de proposito general
	/*****EN CASO DE QUE SE CONFIGURÓ COMO SALIDA***/
	GPIOA->OTYPER &=~ (GPIO_OTYPER_OT5);		//push pull
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;		//very high speed
	//ENTRADA O SALIDA
	GPIOA->PUPDR &=~ (GPIO_PUPDR_PUPD5);		//no pull up /down

	//SW -> PC13
	printf("CONFIGURANDO EL PIN PC13\r\n");
	GPIOC->MODER &=~ (GPIO_MODER_MODE13);		//entrada
	GPIOC->PUPDR &=~ (GPIO_PUPDR_PUPD13);
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_0;		//pull up
    /* Loop forever */
	for(;;){
		bit = (GPIOC->IDR>>13) & 0x1U;			//extrae el bit individual
		GPIOA->ODR &=~ (1<<5);
		GPIOA->ODR |= bit<<5;
//		GPIOA->BSRR |= GPIO_BSRR_BR5;
//		GPIOA->BSRR |= bit<<5;
	}
}

int _write(int file, char *ptr, int len)
{
 int DataIdx;

  for(DataIdx=0; DataIdx<len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}
