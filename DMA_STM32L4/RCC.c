#include "RCC.h"

/**
 *
 * 		FUNCION PARA ESTABLECER LA LATENICIA DE LA FLASH (MSI)
 */
static RCC_Status_t MSI_FlashLatency(uint8_t MSIRange){
	if(MSIRange<=MSI_RANGE8_16MHz){
		FLASH->ACR &=~ FLASH_ACR_LATENCY;
		FLASH->ACR |= FLASH_ACR_LATENCY_0WS;
	}
	else if(MSIRange<=MSI_RANGE10_32MHz){
		FLASH->ACR &=~ FLASH_ACR_LATENCY;
		FLASH->ACR |= FLASH_ACR_LATENCY_1WS;
	}else if(MSIRange == MSI_RANGE11_48MHz){
		FLASH->ACR &=~ FLASH_ACR_LATENCY;
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
	}
	else{
		return RCC_ERROR;
	}
	return RCC_OK;
}

/**
 * @brief Configura la frecuencia del MSI CLOCK
 * @param MSIRange: Se define el rango de del MSI
 * @return : RCC_Status
 */
RCC_Status_t MSI_ConfigRange(uint8_t MSIRange){
	/*CAMBIAR LA FUENTE DE RELOJ DEL SISTEMA*/
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));
	RCC->CFGR &=~ RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while(!(RCC->CFGR & RCC_CFGR_SWS_HSI));
	/*1. Habilitar*/
	RCC->CR |= RCC_CR_MSION;
	while(!(RCC->CR & RCC_CR_MSIRDY));
	RCC->CFGR &=~ 0x3U;
	while(RCC->CFGR & 0x3U<<2);
	/*configurar el rango*/
	if(MSIRange<= MSI_RANGE11_48MHz){
		RCC->CR &=~ (RCC_CR_MSIRANGE);
		RCC->CR |= MSIRange;
		RCC->CR |= RCC_CR_MSIRGSEL;
	}
	
	/*CONFIGURAR LA LATENCIA DE LA FLASH*/
	if(MSI_FlashLatency(MSIRange) != RCC_OK)
		return RCC_ERROR;
	/*seleccionar la fuente de reloj*/
	RCC->CFGR &=~ RCC_CFGR_SW;
	/*APAGAR EL HSI16*/
	RCC->CR &=~ RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY));
	SystemCoreClockUpdate();
	return RCC_OK;
}


/**
 * @brief Configura la frecuencia del PLL clock
* @param :none
 * @return : RCC_Status
 */
RCC_Status_t PLL_Config(void ){
	/*
				F(vco_output_clock) = (16MHz/2)*20= 160MHz
				F(PLL_output) = 160MHz/2 = 80MHz
	*/
	/*CAMBAIR LA FUENTE DE RELOJ*/
	RCC->CFGR &=~ RCC_CFGR_SW;
	//while(!(RCC->CFGR & RCC_CFGR_SWS_MSI));
	/*1. Deshabilitar el pll (RCC_CR)*/
	RCC->CR &=~ RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY);
	/*2. configurar la fuente de reloj del PLL (RCC_PLLCFGR)*/
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
	
	/*3. configurar los parametros del pll (RCC_PLLCFGR)*/
	//M (1 a 8)
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_0;			//PLLM = 2 
	//N (8 a 86)
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= 20U<<8;									//PLLN = 20
	//R
	RCC->PLLCFGR &=~ RCC_PLLCFGR_PLLR;			//PLLR = 0
	/*4. Habilitar el bit PLLREN (RCC_PLLCFGR)*/
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
	/*5. Habilitar y esperar que el pll este listo (RCC_CR)*/
	RCC->CR |= RCC_CR_PLLON;					
	while(!(RCC->CR & RCC_CR_PLLRDY));
	
	/*6. opcional configurar los prescalers HHB, APBx (RCC_CFGR)*/
	
	/*7. configurar la latencia de la flash (FLASH_ACR)*/
	FLASH->ACR &=~ FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
	/*8. Seleccionar el reloj habilitado como la fuente del sistema (RCC_CFGR)*/
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));
	SystemCoreClockUpdate();
	return RCC_OK;
}
