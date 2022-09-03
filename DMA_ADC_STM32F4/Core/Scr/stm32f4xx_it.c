/*
 * stm32f4xx_it.c
 *
 *  Created on: 27 mar. 2022
 *      Author: Lenovo
 */

#include "stm32f4xx.h"
#include "USART.h"


extern void DMA_TrnsmitCpltCallback(void);

extern  USART_Handle_t usart2_handle;
void USART2_IRQHandler(void)
{
	USART_IRQHandling(&usart2_handle);
}

void DMA2_Stream4_IRQHandler(void){
	if(DMA2->HISR & 1U<<5){
		DMA2->HIFCR |= 1U<<5;

		DMA_TrnsmitCpltCallback();
	}
	if(DMA2->HISR & 1U<<4){
		DMA2->HIFCR |= 1U<<4;
	}
	if(DMA2->HISR & 1U<<3){
		DMA2->HIFCR |= 1U<<3;
	}
	if(DMA2->HISR & 1U<<2){
		DMA2->HIFCR |= 1U<<1;
	}
	return;
}
