/*
 * delay.c
 *
 *  Created on: 15 ago. 2022
 *      Author: jeffr
 */


#include "delay.h"

/*global variables ------------------------------------*/

volatile uint32_t uwTick;

/**
 * @brief Retorna el valor de la variabla ticks
 * @param none
 * @return ticks
 */
static uint32_t GetTick(void){

	return uwTick;
}

/*Function defines ------------------------------------*/
void delay_init_it(void){
	uint32_t temp;
	/*deshabilitar la systick*/
	SysTick->CTRL = 0;
	SystemCoreClockUpdate();
	temp = SystemCoreClock / 1000UL;
	SysTick->LOAD = temp - 1;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;	//SYSCLK

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;		//habilitando la IT del systick

	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	return;
}

/**
 * @brief funcion encargada de generar retardos en ms
 * @param [delay]: tiempo de retado en ms deseado
 */
void delay_ms(uint32_t delay){
	uint32_t tickstart = GetTick();
	uint32_t wait = delay;
	//generacion del retardo
	while((GetTick() - tickstart)<wait);

	return;
}

/**
 * @brief funcion similar a la funcion de arduino
 */
uint32_t millis(void){
	return GetTick();
}
