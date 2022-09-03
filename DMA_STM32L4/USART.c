#include "USART.h"

/*FUNCIONES PRIVADAS*/

/**
 * @brief CONFIGURA LOS PINEX TX/RX DEL USART1
 */
static void USART1_Pins_Config(void){
	/*USART1_TX -> PB6
	* USART1_RX -> PB7*/
	/*enable clock*/
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	/*PB6, PB7: modo funcion alternativa*/
	GPIOB->MODER &=~ (GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOB->MODER |= GPIO_MODER_MODE6_1; 						//funcion alternativa
	GPIOB->MODER |= GPIO_MODER_MODE7_1;							//FUNCION ALTERNATIVA 
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7; //Very high speed
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0; //pull-up
	GPIOB->AFR[0] &=~ (GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7);
	GPIOB->AFR[0] |= 0x7U<<24;											//PB6->TX
	GPIOB->AFR[0] |= 0x7U<<28;											//PB7->RX
}

/**
 * @brief CONFIGURA LOS PINEX TX/RX DEL USART1
 */
static void USART2_Pins_Config(void){
	/*USART2_TX -> PA2
	* USART2_RX -> PA15*/
	/*enable clock*/
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	/*PA2->UART2_TX, PA15->UART2_RX*/
	GPIOA->MODER &=~(GPIO_MODER_MODE2);
	GPIOA->MODER |= GPIO_MODER_MODE2_1;
	GPIOA->MODER &=~(GPIO_MODER_MODE15);
	GPIOA->MODER |= GPIO_MODER_MODE15_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED15;				//very high speed
	GPIOA->PUPDR &=~ (GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD15);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD15_0;
	GPIOA->AFR[0] &=~ GPIO_AFRL_AFRL2;
	GPIOA->AFR[1] &=~ GPIO_AFRH_AFRH7;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_0;
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL15_1 | GPIO_AFRH_AFSEL15_0;										
}


/**
 * @brief ESTABLECE EL VALOR DEL REGISTRO BRR
 * @param USARTx: Insatancia al usart deseado
 * @param fck: La freq. para generacion de baudios
 * @param txRxBaud: La taza de baudios deseada
 * @return nada
 */

static void setUSARTDIV(USART_TypeDef *USARTx,uint32_t fck, uint32_t txRxBaud){
	uint16_t temp;
	if(USARTx->CR1 & USART_CR1_OVER8){ //oversampling 8
		temp = (uint16_t)(2 * fck ) / txRxBaud;
		temp = (temp & 0xFFFFFFF0) | ((temp & 0xF)>>1);
	}
	else{
		temp = fck / txRxBaud;
	}
	USARTx->BRR = temp;
	return;
}


/**
 * @brief CONFIGURA EL USART1
 * @param fck: La freq. para generacion de baudios
 * @param txRxBaud: La taza de baudios deseada
 * @return nada
 */
void USART1_Config(uint32_t fck, uint32_t OVER8,uint32_t txRxBaud){
	/*configuracion de pines*/
	USART1_Pins_Config();
	/*enable clock*/
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	/*1. configurar la trama de datos*/
	USART1->CR1 = 0;   //8-BIT DE DATOS
	/*2. Configurar la velocidad en baudios*/
	USART1->CR1 |= OVER8;
	setUSARTDIV(USART1,fck,txRxBaud);
	/*3. Configurar los bits de parada*/
	USART1->CR2  = 0;  //1-BIT DE PARADA
	/*4. habilitar la transmision y recepcion*/
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
	/********OPCIONAL*********/
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_SetPriority(USART1_IRQn,2);
	NVIC_EnableIRQ(USART1_IRQn);
	/*5. habilitar el usart*/
	USART1->CR1 |= USART_CR1_UE;
}

/*Tx byte UART1*/
void USART1_putc(char ch){
	while(!(USART1->ISR & USART_ISR_TXE));
	USART1->TDR = ch;
}
/*Rx byte UART1*/
char USART1_getc(void){
	while(!(USART1->ISR & USART_ISR_RXNE));
	return USART1->RDR;
}

/**
 * @brief CONFIGURA EL USART2
 * @param fck: La freq. para generacion de baudios
 * @param txRxBaud: La taza de baudios deseada
 * @return nada
 */
void USART2_Config(uint32_t fck, uint32_t OVER8,uint32_t txRxBaud){
	/*configuracion de pines*/
	USART2_Pins_Config();
	/*enable clock*/
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	/*1. configurar la trama de datos*/
	USART2->CR1 = 0;   //8-BIT DE DATOS
	/*2. Configurar la velocidad en baudios*/
	USART2->CR1 |= OVER8;
	setUSARTDIV(USART2,fck,txRxBaud);
	/*3. Configurar los bits de parada*/
	USART2->CR2  = 0;  //1-BIT DE PARADA
	/*4. habilitar la transmision y recepcion*/
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
	/********OPCIONAL*********/
	USART2->CR1 |= USART_CR1_RXNEIE;
	NVIC_SetPriority(USART2_IRQn,2);
	NVIC_EnableIRQ(USART2_IRQn);
	/*5. habilitar el usart*/
	USART2->CR1 |= USART_CR1_UE;
}

/*Tx byte UART2*/
void USART2_putc(char ch){
	while(!(USART2->ISR & USART_ISR_TXE));
	USART2->TDR = ch;
}
/*Rx byte UART2*/
char USART2_getc(void){
	while(!(USART2->ISR & USART_ISR_RXNE));
	return USART2->RDR;
}

