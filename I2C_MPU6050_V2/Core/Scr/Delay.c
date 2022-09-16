/*
 * Delay.c
 *
 *  Created on: 31 ene. 2022
 *      Author: Lenovo
 */


#include "Delay.h"

#if USE_DELAY_US == 1
/**
 * @brief funcion que va generar retardos en microsegundos
 */
void delay_us(uint32_t delay){
	uint32_t i;
	/*poner el regitro VAL*/
	SysTick->VAL = 0;
	for(i = 0;i<delay;i++){
		while(!(SysTick->CTRL & 1U<<16));			//retardo de un microsegundo
	}
	return;

}


#endif
/**
 * delay init
 */
void delay_Init(uint32_t ticks){
	/*deshabilitar la systick*/
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
	/*cargar el valor de reload al registro LOAD*/
	SysTick->LOAD = ticks - 1;   //2^24 - SystemCoreClock * delay/1000
	/*selecciona la fuente de reloj*/
	SysTick->CTRL |= 1u<<2;
	/*elegir la fuente de reloj Habilitar el conteo*/
	SysTick->CTRL |=  1;				//SYSCLK
}
/**
 * @brief funcion que va generar retardos en milisegundos
 */
void delay_ms(uint32_t delay){
	uint32_t i;
#if USE_DELAY_US == 1
	for(i = 0;i<delay;i++){
		delay_us(1000);
	}
#else
	/*poner el regitro VAL*/
	SysTick->VAL = 0;

	/*Habilitar el conteo*/
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	for(i = 0;i<delay;i++){
		while(!(SysTick->CTRL & 1U<<16));			//retardo de un milisegundo
	}
#endif
	return;
}
