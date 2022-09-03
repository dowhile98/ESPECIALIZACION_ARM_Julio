
/**
 * Timer.c V.10
 * @autor QUINO B. JEFFRY
 */
 
 /*inclusiones*/
 #include "Timer.h"

 /**
  * @brief configura el pin asignado la CH1 del timer2
	* PA0 -> TIM2_CH1 (AF1)
	* PA5 -> TIM2_CH1	(AF1)
	* PA15-> TIM2_CH1	(AF1)
	*/
static void TIMER2_CH1_Pin_Config(void){
	 /*Enable clock*/
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	 /*PA5*/
	 GPIOA->MODER &=~ GPIO_MODER_MODE5;
	 GPIOA->MODER |= GPIO_MODER_MODE5_1;			//alternate function
	 GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;	//very high speed
	 GPIOA->AFR[0] &=~ GPIO_AFRL_AFRL5;
	 GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;			//PA5 -> AF1
	return;
 }

 /**
  * @brief Configura el pin asignado al CH1 del timer16
  * PA6 -> TIM16_CH1 	(AF14)
  * PB6 -> TIM16_CH1N (AF14)
  * PB8 -> TIM16_CH1 	(AF14)
  */
 static void TIMER16_CH1_Pin_Config(void){
	 /*enable clk*/
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	 /*PA6*/
	 GPIOA->MODER &=~ GPIO_MODER_MODE6;
	 GPIOA->MODER |= GPIO_MODER_MODE6_1;			//alternate function
	 GPIOA->AFR[0] &=~ GPIO_AFRL_AFRL6;
	 GPIOA->AFR[0] |= 14<<GPIO_AFRL_AFSEL6_Pos;//PA6-> AF14
	 return;
 }
/**
 * @brief Configura el TIMER6 para generar base de tiempo
 * @param psc: Valor del prescaler para el timer
 * @param arr: Periodo de conteo del timer 
 * @param IT: si se quiere generar interrupciones se le pasa un valor
 */
void TIMER6_BaseTime(uint32_t psc, uint32_t arr, uint32_t IT){
	/*1. Habilitar el reloj*/
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
	/*2. configurar los paremetros del timer*/
	TIM6->ARR = arr;
	TIM6->PSC = psc - 1;
	/*3. CONFIGURACION DE INTERRUPCION*/
	if(IT == TIM_IT_ENABLE){
		TIM6->DIER |= TIM_DIER_UIE;
		NVIC_SetPriority(TIM6_IRQn,3);
		NVIC_EnableIRQ(TIM6_IRQn);
	}
	else
		TIM6->DIER &=~ TIM_DIER_UIE;
	
	/*4. habilitar el conteo*/
	TIM6->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Configura el CH1 del TIMER2 como captura de entrada
 * @param psc: Valor del prescaler para el timer
 * @param arr: Periodo de conteo del timer 
 * @param IC1F: valor del filtro para la entrada
 * @param IC1PSC: valor del prescaler para la entrada
 * @param POLARITY: flanco activa para la capture de entrada
 * @param IT: si se quiere generar interrupciones se le pasa un valor
 */
void TIMER2_CH1_IC_Start(uint32_t psc, uint32_t arr,uint32_t IC1F,uint32_t IC1PSC, uint32_t POLARITY, uint32_t IT){
	/*configuracion del canal 1*/
	TIMER2_CH1_Pin_Config();
	/*enable clock*/
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	/*configurar parametros del timer*/
	TIM2->PSC = psc - 1;
	TIM2->ARR = arr;
	TIM2->CR1 &=~ TIM_CR1_DIR;														//UP COUNTER
	/*1. Seleccionar la entrada TI*/
	TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;											//CC1 channel is configured as input, IC1 is mapped on TI1.
	/*2. Duracion del filtro de entrada*/
	TIM2->CCMR1 &=~ TIM_CCMR1_IC1F;
	TIM2->CCMR1 |= IC1F;
	/*3. Seleccionar el flanco de transicion activa*/
	TIM2->CCER &=~ (TIM_CCER_CC1NP | TIM_CCER_CC1P);			//Rising edge
	TIM2->CCER |= POLARITY;
	/*4. Seleccionar el precaler de la entrada*/
	TIM2->CCMR1 &=~ TIM_CCMR1_IC1PSC;											//no prescaler
	TIM2->CCMR1 |= IC1PSC;																	
	/*5. habilitar la capture de entrada*/
	TIM2->CCER |= TIM_CCER_CC1E;
	/*6. Habilitar le interrupcion si en caso es nesario*/
	if(IT == TIM_IT_ENABLE){
		TIM2->DIER |= TIM_DIER_CC1IE;
		NVIC_SetPriority(TIM2_IRQn,1);
		NVIC_EnableIRQ(TIM2_IRQn);
	}
	else
		TIM2->DIER &=~ TIM_DIER_CC1IE;
	/*habilitar el conteo del timer*/
	TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Configura el CH1 del TIMER2 como comparacion de salida
 * @param psc: Valor del prescaler para el timer
 * @param arr: Periodo de conteo del timer 
 * @param dutty: ciclo de trabajo del PWM
 */
void TIMER16_CH1_OC_PWM_Start(uint32_t psc, uint32_t arr,uint32_t duty){
	/*1. configurar el TIM16_CH1*/
	TIMER16_CH1_Pin_Config();
	/*2. seleccione el reloj del contador(interno, prescaler)*/
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
	TIM16->PSC = psc - 1;
	/*3. escriba los valores deseados en TIM16_ARR y TIM_CCR1*/
	TIM16->ARR = arr;
	TIM16->CCR1 = duty;
	
	/*4. configurar el bit CC1IE si se desea generar interrupciones*/
	
	/*5. seleccionar el modo de salida (PWM)*/
	TIM16->CCMR1 &=~ TIM_CCMR1_OC1M;
	TIM16->CCMR1 |= 0x6<<4;
	TIM16->CCER &=~ TIM_CCER_CC1P;
	TIM16->CCMR1 &=~ TIM_CCMR1_CC1S;
	TIM16->CCER |= TIM_CCER_CC1E;
	
	TIM16->BDTR |= TIM_BDTR_BKP;
	TIM16->BDTR |= TIM_BDTR_MOE;
	/*6. habilitar el contador*/
	TIM16->CR1 |= TIM_CR1_CEN;
}


