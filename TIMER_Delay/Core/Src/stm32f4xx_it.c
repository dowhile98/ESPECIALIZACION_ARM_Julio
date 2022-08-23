/*
 * stm32f4xx_it.c
 *
 *  Created on: 15 ago. 2022
 *      Author: jeffr
 */


#include <stm32f4xx.h>
#include <USART.h>


extern volatile uint32_t uwTick;

extern volatile uint32_t uwTick_tim;
//extern uint8_t txbuffer[100];
//uint16_t i = 0;
//extern uint16_t len;

/*ISR---------------------------------------*/

void SysTick_Handler(void){
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		uwTick += 1;
	}
	return;
}

void TIM4_IRQHandler(void){

	if(TIM4->SR & TIM_SR_UIF){
		TIM4->SR &=~ TIM_SR_UIF;
		//todo
		uwTick_tim += 1;
	}
	return;
}

