/*
 * delay_timer.c
 *
 *  Created on: 22 ago. 2022
 *      Author: jeffr
 */


#include <delay_timer.h>

volatile uint32_t uwTick_tim;

/**
 * @brief Retorna el valor de la variabla ticks
 * @param none
 * @return ticks
 */
static uint32_t GetTick(void){

	return uwTick_tim;
}

/**
 * @brief funcion para generar retardos en ms
 * @param [CK_PSC]: frecuencia de entrada al timer
 */
void delay_tim4_init(uint32_t ck_psc){
	uint32_t temp;
	//habilitar el reloj
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->CR1 = 0;
	//calculo del PSC
	temp = ck_psc / 1000000;
	TIM4->PSC = temp - 1;
	//Calculo del ARR
	TIM4->ARR = 1000 - 1;
	//IT
	TIM4->DIER 	|= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM4_IRQn);
	//ACTIVAR EL CONTEO
	TIM4->CR1 |= TIM_CR1_CEN;
}
/**
 * @brief genera retardos en ms
 * @param [ms]: retardo en ms deseado
 */
void delayms(uint32_t ms){
	uint32_t tickstart = GetTick();
	uint32_t wait = ms;
	//generacion del retardo
	while((GetTick() - tickstart)<wait);

	return;
}
