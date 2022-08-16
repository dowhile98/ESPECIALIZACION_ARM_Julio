/*
 * stm32f4xx_it.c
 *
 *  Created on: 15 ago. 2022
 *      Author: jeffr
 */


#include <stm32f4xx.h>

extern volatile uint32_t uwTick;

extern void EXIT_IRQHandling(uint32_t pin_number);
/*ISR---------------------------------------*/

void SysTick_Handler(void){
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		uwTick += 1;
	}
	return;
}
/**
 * EXTI10-11-12-13-14-15
 */
void EXTI15_10_IRQHandler(void){
//	if(EXTI->PR & EXTI_PR_PR13){
//		//CLEAR
//		EXTI->PR |= EXTI_PR_PR13;
//		//todo
//
//	}
	EXIT_IRQHandling(13);
}

void EXTI4_IRQHandler(void){
	EXIT_IRQHandling(4);
}

void EXTI0_IRQHandler(void){
	EXIT_IRQHandling(0);
}
