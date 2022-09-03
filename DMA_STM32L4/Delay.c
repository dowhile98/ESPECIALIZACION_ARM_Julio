#include "Delay.h"

/*VARIABLES PRIVADAS*/
uint32_t uwTick;			//variable que incrementa cada 1ms

/*********FUNCIONES PRIVADAS**************/


/**
 * @brief Retorna el valor de la variabla ticks
 * @param none
 * @return ticks
 */
uint32_t GetTick(void){
	
	return uwTick;
}

/*DEFINICION DE FUNCIONES*/
/**
 * @brief Configura el SysTick
 * Esta funcion configura el SysTick para generar
 * interrupciones cada 1ms.
 * @param none
 * @return none
 */
void Delay_Init(void){
	__disable_irq();
	/*1. deshabilitar el systick*/
	SysTick->CTRL  = 0;
	/*2. cofigurar el valor de reload*/
	SysTick->LOAD = SystemCoreClock / 1000 - 1;
	/*3. seleccionar la fuente de reloj*/
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
	/*4. poner el registro val*/
	SysTick->VAL = 0;
	/*5. habilitar la interrupcion y el conteo*/
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | \
									 SysTick_CTRL_ENABLE_Msk;	
	/*6. (opcional) cambiar la prioridad*/
	NVIC_SetPriority(SysTick_IRQn,15);
  __enable_irq();	
	return;
}


/**
 * @brief Genera retardos en ms
 * Esta funcion genera retardos en ms 
 * @param delay: el tiempo en ms del retardo deseado
 * @return none
 */
void delay_ms(uint32_t delay){
	
	uint32_t tickstart = GetTick();
  uint32_t wait = delay;
	
	/*delay*/
  while ((GetTick() - tickstart) < wait);
	
	return;
}

/*ISR*/
void SysTick_Handler(void){
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
		uwTick+= 1;   //se incrementa el valor de la variable en 1
	
	return;
}

