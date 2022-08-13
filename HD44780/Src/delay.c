/*
 * delay.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */


#include "delay.h"


/*VARIABLES PRIVADAS*/
uint32_t uwTick;				//incrementar 1us/ms


/**
 * @brief Retorna el valor de la variabla ticks
 * @param none
 * @return ticks
 */
static uint32_t GetTick(void){

	return uwTick;
}


/**
 * @brief inicializa la funcion que genera retardos en milisegundos/microsegundos
 */
void delay_init(void){
	uint32_t temp;
#if USE_DELAY_US
	temp = SystemCoreClock / 1000000;
#else
	temp = SystemCoreClock / 1000;
#endif
	SysTick->LOAD = temp - 1;
	SysTick->VAL  = 0;
	SysTick->CTRL = 0;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		//use SYSCLK
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;			//enable systick interrupt
	/***************************************************/
	NVIC_SetPriority(SysTick_IRQn,7);					//priority change
	/***************************************************/
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;			//enable count systick
}
/**
 * @brief retardos en us
 */
#if USE_DELAY_US == 1

void delay_us(uint32_t delay){
	int32_t tickstart = GetTick();
	uint32_t wait = delay;
	while ((GetTick() - tickstart) < wait);//retardo en us
	return;
}
#endif
/**
 * @brief retardos en ms
 */
void delay_ms(uint32_t delay){
#if USE_DELAY_US == 1
	for(uint32_t i = 0; i<delay; i++){
		delay_us(1000);				//1ms de retardo
	}
#else
	int32_t tickstart = GetTick();
	uint32_t wait = delay;
	while ((GetTick() - tickstart) < wait);//retardo en ms
#endif

}
