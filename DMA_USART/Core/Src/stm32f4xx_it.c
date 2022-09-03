/*
 * stm32f4xx_it.c
 *
 *  Created on: 15 ago. 2022
 *      Author: jeffr
 */


#include <stm32f4xx.h>
#include <USART.h>


extern volatile uint32_t uwTick;



/*ISR---------------------------------------*/

void SysTick_Handler(void){
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
		uwTick += 1;
	}
	return;
}

void DMA1_Stream6_IRQHandler(void){
	//verificacion de las IT
	if(DMA1->HISR & DMA_HISR_TCIF6){		//transmision completa
		DMA1->HIFCR |= DMA_HIFCR_CTCIF6;	//clear
		//todo
		GPIOA->ODR ^= 1<<5;
	}
	if(DMA1->HISR & DMA_HISR_TEIF6){
		DMA1->HIFCR |= DMA_HIFCR_CTEIF6;
		//todo

	}
	if(DMA1->HISR & DMA_HISR_DMEIF6){
		DMA1->HIFCR |= DMA_HIFCR_CDMEIF6;
		//todo

	}
	return;
}
