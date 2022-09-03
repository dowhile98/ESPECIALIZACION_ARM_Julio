
/**
 *@uator QUINO B. JEFFRY
 *@version v. 1.0
 */
#ifndef __ADC_H
#define __ADC_H

/*includes*/
#include "stm32l4xx.h"                  // Device header

/*macros*/
#define ADC_ADCALDIF_EN				1U<<30
#define ADC_ADCALDIF_DI				0x0<<30

/*ADC_CHANELS*/

#define ADC_IN0							0x0U			//Internal voltage reference (VREFINT)
#define ADC_IN1							0x1U			//PC0
#define ADC_IN2							0x2U			//PC1
#define ADC_IN3							0x3U			//PC2
#define ADC_IN4							0x4U			//PC3
#define ADC_IN5							0x5U			//PA0	(solo ADC1)
#define ADC_IN6							0x6U			//PA1 (solo ADC1)
#define ADC_IN7							0x7U			//PA2 
#define ADC_IN8							0x8U			//PA3
#define ADC_IN9							0x9U			//PA4
#define ADC_IN10						0xAU			//PA5
#define ADC_IN11						0xBU			//PA6
#define ADC_IN12						0xCU			//PA7
#define ADC_IN13						0xDU			//PC4
#define ADC_IN14						0xEU			//PC5
#define ADC_IN15						0xFU			//PB0
#define ADC_IN16						0x10U			//PB1
#define ADC_IN17						0x11U			//Temperature sensor
#define ADC_IN18						0x12U			//VBAT battery voltage monitoring


/*ADC1 and ADC2 - External triggers for regular channels*/
#define EXT_TIM1_CH1				0x0<<6
#define EXT_TIM1_CH2				0x1U<<6
#define EXT_TIM1_CH3				0x2U<<6
#define EXT_TIM2_CH2				0x3U<<6
#define EXT_TIM3_TRGO				0x4U<<6
#define EXT_EXTI11					0x6U<<6
#define EXT_TIM1_TRGO				0x9U<<6
#define EXT_TIM1_TRGO2			0xAU<<6
#define EXT_TIM2_TRGO				0xBU<<6
#define EXT_TIM6_TRGO				0xDU<<6
#define EXT_TIM15_TGRO			0xEU<<6

/*Configuring the trigger polarity for regular external triggers*/
#define HARDWARE_TRIGGER_DI					0x0<<10
#define HARDWARE_TRIGGER_RISING			0x1U<<10
#define HARDWARE_TRIGGER_FALLLING		0x2U<<10


/*prototipo de las funciones*/
/**
 *@brief CONFIGURA EL ADC1
 */
void ADC1_Init(void);

/**
 *@brief CALIBRA EL ADC (Single-ended / differential input)
 *@param ADCx : ADC1/ADC2 que se desea calibrar
 *@param ADCALDIF: conversion Single-ended / differential input
 */
void ADC_Calibration(ADC_TypeDef * ADCx,uint32_t ADCALDIF);


/**
 *@brief habilita el adc
 *@param ADCx: ADC1/ADC2 que se desea habilitar
 */
 void ADC_Enable(ADC_TypeDef * ADCx);


  /**
 *@brief deshabilita el adc
 *@param ADCx: ADC1/ADC2 que se desea deshablitar
 */
void ADC_Disable(ADC_TypeDef * ADCx);

/**
 *@brief Lectura de un canal de un canal del adc
 *@param ADCx: ADC1/ADC2 usado para la conversion
 *@param INx: Numero del canal a leer
 *@return ADCx_DR: valor de la conversion
 */
uint16_t ADC_GetVal(ADC_TypeDef *ADCx, uint32_t INx);

/**
 *@brief Configuracion de disparo del timer y la interrupcion
 *@param ADCx: ADC1/ADC2 usado para la conversion
 *@param EXTx: external trigger Source
 *@param POLARITY: flanco activo de disparo de la conversion adc
 */
void ADC_ExternalTrigger_Init(ADC_TypeDef *ADCx,uint32_t EXTx, uint32_t POLARITY);

/**
  * @brief Habilita el ADC Request para el DMA
	* @param ADCx: ADC1/ADC2 usado para la conversion
	* @param DMACFG: modo circular = ADC_CFGR_DMACFG
	*/
	
void ADC_DMA_Enable(ADC_TypeDef *ADCx, uint32_t DMACFG);

#endif /*__ADC_H*/
