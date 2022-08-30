/*
 * USART.c
 *
 *  Created on: 19 mar. 2022
 *      Author: Lenovo
 */

#include "USART.h"


/**
 * @brief configura el USARTx como:
 * 1-bit stop
 * sin paridad
 * 8-bit de datos
 */
void USART_Init(USART_TypeDef * USARTx,uint32_t baud){
	/*habilitar el reloj*/
	if(USARTx == USART1){
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}else if(USARTx == USART6){
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	}else{
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	}
	/*Configurar los parametros de la comunicacion*/
	USARTx->CR1 &=~ (USART_CR1_M);				//8-bit datos
	USARTx->CR1 &=~ (USART_CR1_OVER8);			//OVERSAMPLING POR 16
	//Configuracion de la generacion de baudios
	USARTx->BRR = UART_BRR_SAMPLING16(16E+6, baud);
	USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;	//habilita RX y TX
	USARTx->CR2 &=~(USART_CR2_STOP);			//1-bit de stop
	USARTx->CR1 &=~(USART_CR1_PCE);				//sin paridad

	/*habilitar el USART*/
	USARTx->CR1 |= USART_CR1_UE;				//Habilita el usart
}

/**
 * @brief enviar N data por el USART
 */
void USART_SendData(USART_TypeDef * USARTx,uint8_t *pTxBuffer, uint32_t Size){
	uint16_t *pdata;
	for(uint32_t i= 0; i<Size;i++){
		//verificar el flag TXE
		while(!(USARTx->SR & USART_SR_TXE));

		//verificar si se ve transmitir en modo de 8bit o 9bit
		if(USARTx->CR1 & USART_CR1_M){
			//enviar el dato
			pdata = (uint16_t*) pTxBuffer;
			USARTx->DR = (*pdata & (uint16_t)0x1FF);
			//Verificar la paridad
			if(!(USARTx->CR1 & USART_CR1_PCE )){
				pTxBuffer++;
				pTxBuffer++;
			}
			else{
				pTxBuffer++;
			}
		}
		else{		//modo de 8bit
			USARTx->DR = (*pTxBuffer & (uint8_t)0xFF);
			pTxBuffer++;
		}
	}
	/*esperamos que TC se ponga a 1*/
	while(!(USARTx->SR & USART_SR_TC));

	return;
}

/**
 * @brief enviar N data por el USART
 */
void USART_ReceiveData(USART_TypeDef * USARTx,uint8_t *pRxBuffer, uint32_t Size){
	for(uint32_t i= 0; i<Size;i++){
		//verificar el flag RXNE
		while(!(USARTx->SR & USART_SR_RXNE));
		//verificar si se va recibir en modo de 8bit o 9bit
		if(USARTx->CR1 & USART_CR1_M){

			/*VERIFICAR LA PARIDAD*/
			if(!(USARTx->CR1 & USART_CR1_PCE )){	//sin paridad
				*((uint16_t*) pRxBuffer) = USARTx->DR & (uint16_t) 0x1FF;
				pRxBuffer++;
				pRxBuffer++;
			}
			else{	//paridad
				*pRxBuffer = USARTx->DR & (uint8_t) 0xFF;
				pRxBuffer++;
			}
		}
		else{
			/*VERIFICAR LA PARIDAD*/
			if(!(USARTx->CR1 & USART_CR1_PCE)){
				*pRxBuffer = USARTx->DR & (uint8_t) 0xFF;
			}
			else{
				*pRxBuffer = USARTx->DR & (uint8_t) 0x7F;
			}
			pRxBuffer++;
		}
	}
	return;
}

void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi){

	if(EnorDi == ENABLE){
		NVIC_EnableIRQ(IRQNumber);
	}else{
		NVIC_DisableIRQ(IRQNumber);
	}
	return;
}

void USART_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority){

	NVIC_SetPriority(IRQNumber,IRQPriority);

	return;
}

/**
 * @brief enviar datos por USART mediante generacion de interrupciones
 */
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer, uint32_t Size){
	uint8_t txSatate = pUSARTHandle->TxBusyState;

	if(txSatate != USART_BUSY_IN_TX){
		pUSARTHandle->TxLen = Size;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxBusyState = USART_BUSY_IN_TX;
		/*habilita la interrupcion para TXE*/
		pUSARTHandle->pUSARTx->CR1 |= USART_CR1_TXEIE;
		/*interrupcion para TC*/
		pUSARTHandle->pUSARTx->CR1 |= USART_CR1_TCIE;
	}
	return txSatate;
}

/**
 * @brief recibir datos mediante interrupcion
 */
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer, uint32_t Size){
	uint8_t rxState = pUSARTHandle->RxBusyState;

	if(rxState != USART_BUSY_IN_RX){
		pUSARTHandle->RxLen = Size;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxBusyState = USART_BUSY_IN_RX;

		(void)pUSARTHandle->pUSARTx->DR;
		/*habilitar la interrupcion para RXNE*/
		pUSARTHandle->pUSARTx->CR1 |= USART_CR1_RXNEIE;
	}

	return rxState;

}

void USART_IRQHandling(USART_Handle_t *pUSARTHandle){

	uint32_t temp1 , temp2, temp3;

	uint16_t *pdata;

/*************************Check for TC flag ********************************************/
	//TC check
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_TC;
	//TCIE check
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_TCIE;

	if(temp1 && temp2){

		//cierre la transmisiÃ³n y la devoluciÃ³n de llamada de la aplicaciÃ³n de llamada si TxLen es cero
		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX){
			//Compruebe el TxLen
			if(! pUSARTHandle->TxLen){
				//borrar el flag TC
				pUSARTHandle->pUSARTx->SR &=~ USART_SR_TC;
				pUSARTHandle->pUSARTx->CR1 &=~ USART_CR1_TCIE;

				pUSARTHandle->TxBusyState = USART_READY;

				pUSARTHandle->pTxBuffer = NULL;

				//Restablecer la longitud a cero
				pUSARTHandle->TxLen = 0;

				//Call the application call back with event USART_EVENT_TX_CMPLT
				USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_TX_CMPLT);
			}
		}
	}

/*************************Check for TXE flag ********************************************/
	//Check TXE
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_TXE;
	//check TXIE
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_TXEIE;

	if(temp1 && temp2){

		if(pUSARTHandle->TxBusyState == USART_BUSY_IN_TX){

			if(pUSARTHandle->TxLen > 0){

				//verificar el M bit (9bit o 8bit)
				if(pUSARTHandle->pUSARTx->CR1 & USART_CR1_M){
					pdata = (uint16_t*)pUSARTHandle->pTxBuffer;
					pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x1FF);

					//verificar los bits de paridad
					if(!(pUSARTHandle->pUSARTx->CR1 & USART_CR1_PCE)){
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen -= 2;
					}
					else{
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->TxLen -= 1;
					}
				}
				else{
					pUSARTHandle->pUSARTx->DR = (*pUSARTHandle->pTxBuffer & (uint8_t) 0xFF);

					pUSARTHandle->pTxBuffer++;
					pUSARTHandle->TxLen -=1;
				}
			}
			if(pUSARTHandle->TxLen == 0){
				//TxLen es cero
				pUSARTHandle->pUSARTx->CR1 &=~ USART_CR1_TXEIE;
			}
		}
	}
/*************************Check for RXNE flag ********************************************/
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_RXNE;
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_RXNEIE;

	if(temp1 && temp2){
		if(pUSARTHandle->RxBusyState == USART_BUSY_IN_RX){

			if(pUSARTHandle->RxLen > 0){

				//verificar el M bit
				if(pUSARTHandle->pUSARTx->CR1 & USART_CR1_M){

					//verificamos la paridad
					if(!(pUSARTHandle->pUSARTx->CR1 & USART_CR1_PCE)){

						*((uint16_t*)pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint16_t)0x1FF);
						//incrementar el buffer y decrementar
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen -= 2;
					}
					else{
						*pUSARTHandle->pRxBuffer = (pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->RxLen -= 1;
					}
				}
				else{
					if(!(pUSARTHandle->pUSARTx->CR1 & USART_CR1_PCE)){
						*pUSARTHandle->pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR & (uint8_t) 0xFF);
					}
					else{
						*pUSARTHandle->pRxBuffer = (uint8_t)(pUSARTHandle->pUSARTx->DR & (uint8_t)0x7F);
					}

					pUSARTHandle->pRxBuffer++;
					pUSARTHandle->RxLen -= 1;

				}
			}
			if(!(pUSARTHandle->RxLen)){
				pUSARTHandle->pUSARTx->CR1 &=~ USART_CR1_RXNEIE;
				pUSARTHandle->RxBusyState = USART_READY;
				USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_RX_CMPLT);
			}
		}
	}

/*************************Check for CTS flag ********************************************/
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_CTS;
	temp2 = pUSARTHandle->pUSARTx->CR3 & USART_CR3_CTSE;
	temp3 = pUSARTHandle->pUSARTx->CR3 & USART_CR3_CTSIE;

	if(temp1 && temp2){
		pUSARTHandle->pUSARTx->SR &=~ USART_SR_CTS;
		//this interrupt is because of cts
		USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_CTS);
	}
/*************************Check for IDLE detection flag ********************************************/
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_IDLE;
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_IDLEIE;
	if(temp1 && temp2){
		temp1 = pUSARTHandle->pUSARTx->SR &=~ USART_SR_IDLE;

		//this interrupt is because of idle
		USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_IDLE);
	}
/*************************Check for Overrun detection flag ********************************************/
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_ORE;
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_RXNEIE;

	if(temp1 && temp2){
		//this interrupt is because of Overrun error
		USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_ORE);
	}
/*************************Check for Error Flag ********************************************/

//Noise Flag, Overrun error and Framing Error in multibuffer communication
//We dont discuss multibuffer communication in this course. please refer to the RM
//The blow code will get executed in only if multibuffer mode is used.
	temp3 = pUSARTHandle->pUSARTx->CR3 & USART_CR3_EIE;
	if(temp3){
		temp1 = pUSARTHandle->pUSARTx->SR;
		if(temp1 & USART_SR_FE){
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_FE);
		}

		if(temp1 & USART_SR_NE){
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_NE);
		}
		if(temp1 & USART_SR_ORE){
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERR_ORE);
		}
	}
}


__weak void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t event)
{

}

