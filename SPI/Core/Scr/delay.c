/*
 * delay.c
 *
 *  Created on: 18 feb. 2022
 *      Author: Lenovo
 */

#include "delay.h"
/**
 * @brief delay
 */
void delay_ms(uint32_t delay){
	SysTick->CTRL = 0;
	SysTick->LOAD = 16000-1;
	SysTick->VAL = 0;
	SysTick->CTRL |= 1U<<2 | 1U;
	for(uint32_t i = 0;i<delay;i++){
		while(!(SysTick->CTRL & 1u<<16));
	}
	SysTick->CTRL = 0;
	return;
}
