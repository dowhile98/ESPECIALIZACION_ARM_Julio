/*
 * delay.c
 *
 *  Created on: 6 mar. 2022
 *      Author: Lenovo
 */


#include "delay.h"
//#include "Config.h"

/**
 * delay init
 */
void delay_Init(void){
	uint32_t temp;
#if USE_DELAY_US == 1
	temp = SystemCoreClock / 1000000;
#else
	temp = SystemCoreClock / 1000;
#endif
	/*1. deshabilitar la systick*/
	//BITBAND_ACCESS(SysTick->CTRL,0) = 0;
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
	/*2. establecer el valor del reload al registro LOAD*/
	SysTick->LOAD = temp - 1U;
	/*3. elegir la fuente de reloj*/
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		//SYSCLK
	/*4. habilitar el conteo	 */
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	return;
}

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
 * @brief funcion que va generar retardos en milisegundos
 */
void delay_ms(uint32_t delay){
	uint32_t i;

#if USE_DELAY_US == 1
	for(i = 0;i<delay;i++){
		delay_us(1000);			//retardo de 1ms
	}
#else
	/*poner el valor del registro VAL a cero*/
	SysTick->VAL = 0;

	for(i = 0; i<delay; i++){
		while(!(SysTick->CTRL & 1U<<16));	//retardo de 1ms
	}
#endif

	return;
}
