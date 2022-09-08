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
