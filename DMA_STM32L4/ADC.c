#include "ADC.h"



/**
 *@brief CONFIGURA EL ADC1
 */
void ADC1_Init(void){
	/*habilitar el reloj*/
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	/*configurar los pines anologicos*/
	//PA0,PA1,PA2 -> ANALOGICO
	GPIOA->MODER |= GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE3;
	GPIOA->PUPDR &=~ (GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD3);
	/*configurar los parameotros del registro CCR*/
	ADC12_COMMON->CCR &=~ ADC_CCR_CKMODE;			
	ADC12_COMMON->CCR &=~ ADC_CCR_PRESC;
	ADC12_COMMON->CCR &=~ ADC_CCR_DUAL;
	/*exit Deep-power-down mode*/
	ADC1->CR &=~ ADC_CR_DEEPPWD;
	ADC1->CR |= ADC_CR_ADVREGEN;
	/*CALIBRACION*/
	ADC_Calibration(ADC1,ADC_ADCALDIF_DI);
	/*Configurar los parametros del adc*/
	ADC1->CFGR &=~ ADC_CFGR_ALIGN;											//Right alignment
	ADC1->CFGR &=~ ADC_CFGR_CONT;												//Single conversion mode
	ADC1->CFGR &=~ ADC_CFGR_DISCEN;											//Discontinuous mode for regular channels disabled
	ADC1->CFGR &=~ ADC_CFGR_EXTEN;											//software trigger
	ADC1->CFGR &=~ ADC_CFGR_DMAEN;											//No dma request
	
	
}


/**
 *@brief CALIBRA EL ADC (Single-ended / differential input)
 *@param ADCALDIF: conversion Single-ended / differential input
 */
void ADC_Calibration(ADC_TypeDef * ADCx,uint32_t ADCALDIF){
	uint32_t delay = 20000;
	/*1. Ensure DEEPPWD=0, ADVREGEN=1 and that ADC 
	     voltage regulator startup time has elapsed.*/
	//ADC voltage regulator start-up time = 20us max
	while(delay--);
	/*2. Ensure that ADEN=0.*/
	ADCx->CR &=~ ADC_CR_ADEN;													//ADC1 off
	/*3. Select the input mode for this calibration 
			  by setting ADCALDIF=0 (single-ended input)
				or ADCALDIF=1 (differential input).*/
	ADCx->CR &=~ ADC_CR_ADCALDIF;
	ADCx->CR |= ADCALDIF;
	/*4. Set ADCAL=1.*/
	ADCx->CR |= ADC_CR_ADCAL;													//calibration in progress
	/*5.Wait until ADCAL=0.*/
	while(ADCx->CR & ADC_CR_ADCAL);										//wait
	return;
}

/**
 *@brief habilita el adc
 *@param ADCx: ADC1/ADC2 que se desea habilitar
 */
void ADC_Enable(ADC_TypeDef * ADCx){
	 /*1.Clear the ADRDY bit in the ADC_ISR register by writing ‘1’*/
	 ADCx->ISR |= ADC_ISR_ADRDY; 											//Clear flag
	 /*2. Set ADEN=1.*/
	 ADCx->CR |= ADC_CR_ADEN;
	 /*3.Wait until ADRDY=1 (ADRDY is set after the ADC startup time).*/
	 while(!(ADCx->ISR & ADC_ISR_ADRDY));
}
 
  /**
 *@brief deshabilita el adc
 *@param ADCx: ADC1/ADC2 que se desea deshablitar
 */
void ADC_Disable(ADC_TypeDef * ADCx){ 
	 /*1.Check that both ADSTART=0 and JADSTART=0 . 
		If required, stop any regular and injected conversion o
		ADSTP=1 and JADSTP=1 and then wait until ADSTP=0 and JADSTP=0.*/
	 while(ADCx->CR & ADC_CR_ADSTART);
	 while(ADCx->CR & ADC_CR_JADSTART);
	 ADCx->CR |= ADC_CR_ADSTP | ADC_CR_JADSTP;
	 while(ADCx->CR & ADC_CR_ADSTP);
	 /*2.Set ADDIS=1*/
	 ADCx->CR |= ADC_CR_ADDIS;
	 /*3. wait until ADEN=0*/
	 while(ADCx->CR & ADC_CR_ADEN);
}
 
/**
 *@brief Lectura de un canal de un canal del adc
 *@param ADCx: ADC1/ADC2 usado para la conversion
 *@param INx: Numero del canal a leer
 *@return ADCx_DR: valor de la conversion
 */
uint16_t ADC_GetVal(ADC_TypeDef *ADCx, uint32_t INx){
	/*set sequence conversion*/
	ADCx->SQR1 &=~ ADC_SQR1_SQ1;
	ADCx->SQR1 |= INx << ADC_SQR1_SQ1_Pos;		//conversion in regular sequence
	/*start the conversion*/
	ADCx->CR |= ADC_CR_ADSTART;
	/*************************************************
	EOC Flag will be set, once the conversion is finished
	*************************************************/
	while(!(ADCx->ISR & ADC_ISR_EOC));				// wait for EOC flag to set
	/*Read the Data Register*/
	return ADCx->DR;
}

/**
 *@brief Configuracion de disparo del timer y la interrupcion
 *@param ADCx: ADC1/ADC2 usado para la conversion
 *@param EXTx: external trigger Source
 *@param POLARITY: flanco activo de disparo de la conversion adc
 */
void ADC_ExternalTrigger_Init(ADC_TypeDef *ADCx,uint32_t EXTx, uint32_t POLARITY){
	/*1. External trigger selection for regular group*/
	ADCx->CFGR &=~ ADC_CFGR_EXTSEL;
	ADCx->CFGR |= EXTx;
	/*2. Configuring the trigger polarity for regular external triggers*/
	ADCx->CFGR &=~ ADC_CFGR_EXTEN;
	ADCx->CFGR |= POLARITY;
	
	/*3.set configuration the trigger source*/
	
	return;
}

/**
  * @brief Habilita el ADC Request para el DMA
	* @param ADCx: ADC1/ADC2 usado para la conversion
	* @param DMACFG: modo circular = ADC_CFGR_DMACFG
	*/
	
void ADC_DMA_Enable(ADC_TypeDef *ADCx, uint32_t DMACFG){
	ADCx->CFGR &=~ ADC_CFGR_DMACFG;
	ADCx->CFGR |= DMACFG;
	/*DMA enable*/
	ADCx->CFGR |= ADC_CFGR_DMAEN;
	return;
}
