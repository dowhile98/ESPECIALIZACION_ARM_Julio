/*
 * RCC.c
 *
 *  Created on: 23 ene. 2022
 *      Author: Lenovo
 */

#include "RCC.h"


void flash_cofig(void){
	FLASH->ACR |= 1<<9;							//1: Instruction cache is enabled
	FLASH->ACR |= 1<<10;						//1: Data cache is enabled
	FLASH->ACR |= 1<<8;							//1: Prefetch is enabled
	//SE HABILITAN ALGUNOS CLOCK
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
}

void HSE_Config(void){
	RCC->CR |= RCC_CR_HSEON | RCC_CR_HSEBYP;	//se habilita el HSE en modo BYPASS
	while(!(RCC->CR & RCC_CR_HSERDY));			//se espera que el HSE este listo
	RCC->CFGR = RCC_CFGR_SW_HSE;				//Se selecciona el HSE Como fuente sysclk
	while(!(RCC->CFGR & RCC_CFGR_SWS_HSE));		//se espera que se haga el cambio
	SystemCoreClockUpdate();
}


/**
 * @brief PLL config
 * configura el PLL  a 84MHz
 */
void PLL_Config(void){
	HSE_Config();
	RCC->CR &=~ (1U<<24);
	while(RCC->CR & 1U<<25);
	/*configure PL before enabling it*/
	//1. PLL clock source
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;	//select HSE CLOCK SOURCE
	//2. cofigure division factor (M, N, P , Q)
	/**
	 * 	f(VCO clock) = f(PLL clock input) Ã— (PLLN / PLLM)	(192 and 432 MHz)
	 * 	f(PLL general clock output) = f(VCO clock) / PLLP  	<= 84MHz
	 */
	//M
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= 4U;							//VCO input frequency (1MHz to 2MHz)
	//N
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= 168U<<6;					//168MHZ
	//P
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLP;
	RCC->PLLCFGR |= 1U<<16;						//84MHz
	//3. enable PLL
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));			//Waint until
	/*configure AHBx, APBx precaler*/
	RCC->CFGR = 0x00000000;						//reset
	RCC->CFGR |= 0b100<<10;						//100: AHB clock divided by 2
	/*configure flash latency*/
	FLASH->ACR &=~ FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
	/*select system clock source*/
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));		//wait PLL ready as system clock
	SystemCoreClockUpdate();
	return;
}

