#include "stm32l4xx.h"                  // Device header
#include <stdio.h>


extern uint16_t dataAdc[5];
uint8_t i;
extern void TC_Complete_callback(DMA_Channel_TypeDef * DMA_Channelx);

void ADC1_2_IRQHandler(void){
	
	if(ADC1->ISR & ADC_ISR_EOC){
		dataAdc[i] = ADC1->DR;
		i++;
	}
	else if(ADC1->ISR & ADC_ISR_EOS){
		ADC1->ISR |= ADC_ISR_EOS;
		i = 0;
		printf("IN5->%d , IN6->%d , IN8->%d ,IN9->%d ,IN10->%d \n\r",dataAdc[0], \
						dataAdc[1],dataAdc[2],dataAdc[3],dataAdc[4]);
	}
	
}


void DMA2_Channel3_IRQHandler(void){
	if(DMA2->ISR & DMA_ISR_TCIF3){
		//clear flag
		DMA2->IFCR |= DMA_IFCR_CTCIF3;
		//codigo
		TC_Complete_callback(DMA2_Channel3);
	}
	
}

void DMA1_Channel7_IRQHandler(void){
	if(DMA1->ISR & DMA_ISR_TCIF7){
		//clear flag
		DMA1->IFCR |= DMA_IFCR_CTCIF7;
		//CALBACK
		TC_Complete_callback(DMA1_Channel7);
	}
	
}

