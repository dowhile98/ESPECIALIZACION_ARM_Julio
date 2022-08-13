/*
 * delay.c
 *
 *  Created on: 12 ago. 2022
 *      Author: jeffr
 */

#include "delay.h"


void delay_init(void){
	uint32_t temp;
	/*deshabilitar la systick*/
	SysTick->CTRL = 0;
	SystemCoreClockUpdate();
#if(DELAY_MODE == 0)
	temp = SystemCoreClock / 1000000UL;
#elif (DELAY_MODE == 1)
	temp = SystemCoreClock / 1000UL;
#else
#error "ELECCION INCORRECTA"
#endif
	SysTick->LOAD = temp - 1;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;	//SYSCLK
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	return;
}

#if (DELAY_MODE == 0)
/**
 * @brief funcion encargada de generar retardos en us
 * @param [delay]: tiempo de retado en us deseado
 */
void delay_us(uint32_t delay){
	uint32_t i;
	SysTick->VAL = 0;
	for(i = 0; i<delay; i++){
		while(!(SysTick->CTRL & 1U<<16));	//genera un retardo de 1us
	}
	return;
}
#endif
/**
 * @brief funcion encargada de generar retardos en ms
 * @param [delay]: tiempo de retado en ms deseado
 */
void delay_ms(uint32_t delay){
	uint32_t i;
#if (DELAY_MODE == 0)
	for(i = 0; i<delay; i++){
		delay_us(1000);
	}
#elif (DELAY_MODE == 1)
	SysTick->VAL = 0;
	for(i = 0; i<delay; i++){
		while(!(SysTick->CTRL & 1U<<16));	//genera un retardo de 1us
	}
#else
#error "ERROR DE ELECCION"
#endif
	return;
}
