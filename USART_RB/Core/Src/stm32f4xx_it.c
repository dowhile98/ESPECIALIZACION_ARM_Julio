/*
 * stm32f4xx_it.c
 *
 *  Created on: 15 ago. 2022
 *      Author: jeffr
 */


#include <stm32f4xx.h>
#include <USART.h>


extern volatile uint32_t uwTick;
extern USART_Handle_t usart2;
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
void USART2_IRQHandler(void){
//	if(USART2->SR & USART_SR_TXE){
//		if(i<len){
//			USART2->DR = txbuffer[i];
//			i++;
//		}else{
//			i = 0;
//			USART2->CR1 &=~ USART_CR1_TXEIE;		//deshabilita la IT para TX
//		}
//	}else if(USART2->SR & USART_SR_RXNE){
//		byte = USART2->DR;
//	}
	USART_IRQHandling(&usart2);
}
