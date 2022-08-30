/*
 * stm32f4xx_it.c
 *
 *  Created on: 27 mar. 2022
 *      Author: Lenovo
 */
#include "stm32f4xx.h"


extern void ADC_CvsCpltCallback(void);


void ADC_IRQHandler(void){
	if(ADC1->SR & ADC_SR_EOC){
		ADC_CvsCpltCallback();
	}
}
