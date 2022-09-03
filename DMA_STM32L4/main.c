

#include "stm32l4xx.h"                  // Device header
#include "RCC.h"
#include "USART.h"
#include "Delay.h"
#include "ADC.h"
#include "TIMER.h"
#include "DMA.h"
#include <string.h>

#include <stdio.h>
/*VARIABLES GLOBALES*/
uint16_t dataAdc[5];
char txt[100];
uint8_t ADC_TransmitComplet;
/**
  *@brief Configura los pines seleccinados a su función analogico
	* modificar esta funcion de acuerdo a los pines usados
	*/
void ADC_Pins_Init(void);


/**
  *@brief Establece el tiempo de muestreo para cada canal configurado
  * Modificar esta función de acuerdo a los canales usados
	*@param ADCx: ADC1/ADC2 que se desea configurar
	*/
void ADC_SetSamplingTime(ADC_TypeDef * ADCx);


/**
  *@brief Establece la secuencia de conversion
  * Modificar esta función de acuerdo a los canales usados
	*@param ADCx: ADC1/ADC2 que se desea configurar
	*/
void ADC_SetConversionSqc(ADC_TypeDef * ADCx);


/**
  * @brief configura la interrupciones de los canales configurados
	*/
void DMA_Channel_IT_Enable(void);

int main(void){
	/*RCC configuration*/
	PLL_Config();													//80MHZ
	/*delay init*/
	Delay_Init();
	/*usart2 config*/
	USART2_Config(SystemCoreClock,USART_OVER8_16,115200);
	
	/*************************ADC*******************************/
	//1. ADC Init
	ADC1_Init();
	//2. ADC pinout configuration
	ADC_Pins_Init();
	//3.Set CHANEL sampling time 
	ADC_SetSamplingTime(ADC1);
	//4.Set the conversion sequence of the ADC
	ADC_SetConversionSqc(ADC1);
	//5.Set ADC trigger
	ADC_ExternalTrigger_Init(ADC1,EXT_TIM6_TRGO,HARDWARE_TRIGGER_RISING);
	//6.configure chosen ADC trigger source
	TIMER6_BaseTime(800,5000-1,TIM_IT_DISABLE);
	TIM6->CR2 &=~ TIM_CR2_MMS;
	TIM6->CR2 |= TIM_CR2_MMS_1; //The update event is selected as trigger output (TRGO)
	//7. Enable interrupt
//	ADC1->IER |= ADC_IER_EOCIE | ADC_IER_EOSIE;
//	NVIC_EnableIRQ(ADC1_2_IRQn);
//	NVIC_SetPriority(ADC1_2_IRQn,2);
	//8. enabla ADC
	ADC_Enable(ADC1);
	/*******************DMA************************************/
	//AD1
	DMA_Channel_Init(DMA2,DMA2_Channel3, (uint32_t) &ADC1->DR, (uint32_t)dataAdc,5);
	DMA_Channel_Setting(DMA2_Channel3,CHANNEL_PL_VERY_HIGH,CHANNEL_PERIPH_TO_MEM, DMA_CCR_CIRC,0,\
											DMA_CCR_MINC,CHANNEL_PSIZE_16B,CHANNEL_MSIZE_16B);
	//USART_TX										
	DMA_Channel_Init(DMA1,DMA1_Channel7, (uint32_t) &USART2->TDR,	(uint32_t) txt,100);
	DMA_Channel_Setting(DMA1_Channel7,CHANNEL_PL_HIGH,CHANNEL_MEM_TO_PERIPH,0,0,\
											 DMA_CCR_MINC,CHANNEL_PSIZE_8B,CHANNEL_MSIZE_8B);

	//USART2}
	/*CONFIGURAR LA INTERRUPCION*/
	DMA_Channel_IT_Enable();
	/*configurar  Solicitudes de DMA para cada canal*/
	DMA2_CSELR->CSELR &=~ DMA_CSELR_C3S; 											//ADC1
	DMA1_CSELR->CSELR &=~ DMA_CSELR_C7S;
	DMA1_CSELR->CSELR |= 0x2U<<DMA_CSELR_C7S_Pos;
	
	/*habilitar los canales configurados*/
	DMA_Channel_Enable(DMA2_Channel3);
	/**********************************************************/
	
	/********************DMA REQUEST***************************/
	ADC_DMA_Enable(ADC1,ADC_CFGR_DMACFG);
	ADC1->CR |= ADC_CR_ADSTART;
	/**********************************************************/
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	GPIOB->MODER &=~ GPIO_MODER_MODE3;
	GPIOB->MODER |= GPIO_MODER_MODE3_0;
	while(1){

		if(ADC_TransmitComplet){
			///
			GPIOB->ODR ^=1<<3;
			ADC_TransmitComplet = 0;
			
			sprintf(txt,"IN5->%d , IN6->%d , IN8->%d ,IN9->%d ,IN10->%d \n\r",dataAdc[0], \
						dataAdc[1],dataAdc[2],dataAdc[3],dataAdc[4]);
			DMA_Channel_Disable(DMA1_Channel7);
			DMA1_Channel7->CNDTR = strlen(txt);
			DMA_Channel_Enable(DMA1_Channel7);
		
			USART2->CR3 |= USART_CR3_DMAT;
		}
		
	}
}

/**
  *@brief Configura los pines seleccinados a su función analogico
	* modificar esta funcion de acuerdo a los pines usados
	*/
void ADC_Pins_Init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	//PA0, PA1, PA3, PA4, PA5 -> analogico
	GPIOA->MODER |= GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE3 | \
									GPIO_MODER_MODE4 | GPIO_MODER_MODE5;
	GPIOA->PUPDR &=~ (GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD3 | \
										GPIO_PUPDR_PUPD4 	| GPIO_PUPDR_PUPD5);
}

/**
  *@brief Establece el tiempo de muestreo para cada canal configurado
  * Modificar esta función de acuerdo a los canales usados
	*@param ADCx: ADC1/ADC2 que se desea configurar
	*/
void ADC_SetSamplingTime(ADC_TypeDef * ADCx){
	ADCx->SMPR1 |= ADC_SMPR1_SMP5_1;    //IN5 -> 12.5 ADC clock cycles
	ADCx->SMPR1 |= ADC_SMPR1_SMP6_1;    //IN6 -> 12.5 ADC clock cycles
	ADCx->SMPR1 |= ADC_SMPR1_SMP8_1;    //IN8 -> 12.5 ADC clock cycles
	ADCx->SMPR1 |= ADC_SMPR1_SMP9_1;    //IN9 -> 12.5 ADC clock cycles
	ADCx->SMPR2 |= ADC_SMPR2_SMP10_1;    //IN10 -> 12.5 ADC clock cycles
	
}

/**
  *@brief Establece la secuencia de conversion
  * Modificar esta función de acuerdo a los canales usados
	*@param ADCx: ADC1/ADC2 que se desea configurar
	*/
void ADC_SetConversionSqc(ADC_TypeDef * ADCx){
	ADCx->SQR1  = 0;
	ADCx->SQR1 |= (0x4)<<ADC_SQR1_L_Pos;
	/*configurar la secuencia de la conversion*/
	ADCx->SQR1 |= ADC_IN5 << ADC_SQR1_SQ1_Pos;
	ADCx->SQR1 |= ADC_IN6 << ADC_SQR1_SQ2_Pos;
	ADCx->SQR1 |= ADC_IN8 << ADC_SQR1_SQ3_Pos;
	ADCx->SQR1 |= ADC_IN9 << ADC_SQR1_SQ4_Pos;
	ADCx->SQR2 |= ADC_IN10 << ADC_SQR2_SQ5_Pos;
}

/**
  * @brief configura la interrupciones de los canales configurados
	*/
void DMA_Channel_IT_Enable(void){
	//DMA1 CHANNEL 7
	DMA1_Channel7->CCR |= DMA_CCR_TCIE;
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	
	//DMA2 CHANNEL 3
	DMA2_Channel3->CCR |= DMA_CCR_TCIE;
	NVIC_EnableIRQ(DMA2_Channel3_IRQn);
	return;
}


/***********CALLBACK FUNCTION***************/

/**
  * @brief funcion que se ejecutará luego de una interrucpion-> transmit complet
  * @param DMA_Channelx: canal que genera la interrupcion
  */
void TC_Complete_callback(DMA_Channel_TypeDef * DMA_Channelx){
	if(DMA_Channelx == DMA1_Channel7){

	
		USART2->CR3 &=~ USART_CR3_DMAT;
	}
	else if(DMA_Channelx == DMA2_Channel3){
		
		ADC_TransmitComplet  = 1;
	}
	
	return;
	
}
