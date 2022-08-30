/*
 * USART.h
 *
 *  Created on: 19 mar. 2022
 *      Author: Lenovo
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#include "stm32f4xx.h"
#include <stdlib.h>



/*
 * DEFINIR EL OVERSAMPLING
 */
#define OVERSAMPLING16 		0
#define OVERSAMPLING8  		1


/*
 * Application states
 */
#define USART_BUSY_IN_RX 			1
#define USART_BUSY_IN_TX 			2
#define USART_READY 				0
#define USART_EVENT_TX_CMPLT   		0
#define	USART_EVENT_RX_CMPLT   		1
#define	USART_EVENT_IDLE      		2
#define	USART_EVENT_CTS       		3
#define	USART_EVENT_PE        		4
#define	USART_ERR_FE     			5
#define	USART_ERR_NE    	 		6
#define	USART_ERR_ORE    			7

#define __weak __attribute__((weak))

/*
 * Handle structure for USARTx peripheral
 */
typedef struct
{
	USART_TypeDef *pUSARTx;
	uint8_t *pTxBuffer;
	uint8_t *pRxBuffer;
	uint32_t TxLen;
	uint32_t RxLen;
	uint8_t TxBusyState;
	uint8_t RxBusyState;
}USART_Handle_t;








/**
 * Calculo de BRR
 */
#define UART_DIV_SAMPLING16(_PCLK_, _BAUD_)            ((uint32_t)((((uint64_t)(_PCLK_))*25U)/(4U*((uint64_t)(_BAUD_)))))
#define UART_DIVMANT_SAMPLING16(_PCLK_, _BAUD_)        (UART_DIV_SAMPLING16((_PCLK_), (_BAUD_))/100U)
#define UART_DIVFRAQ_SAMPLING16(_PCLK_, _BAUD_)        ((((UART_DIV_SAMPLING16((_PCLK_), (_BAUD_)) - (UART_DIVMANT_SAMPLING16((_PCLK_), (_BAUD_)) * 100U)) * 16U)\
                                                         + 50U) / 100U)
/* UART BRR = mantissa + overflow + fraction
            = (UART DIVMANT << 4) + (UART DIVFRAQ & 0xF0) + (UART DIVFRAQ & 0x0FU) */
#define UART_BRR_SAMPLING16(_PCLK_, _BAUD_)            ((UART_DIVMANT_SAMPLING16((_PCLK_), (_BAUD_)) << 4U) + \
                                                        (UART_DIVFRAQ_SAMPLING16((_PCLK_), (_BAUD_)) & 0xF0U) + \
                                                        (UART_DIVFRAQ_SAMPLING16((_PCLK_), (_BAUD_)) & 0x0FU))

#define UART_DIV_SAMPLING8(_PCLK_, _BAUD_)             ((uint32_t)((((uint64_t)(_PCLK_))*25U)/(2U*((uint64_t)(_BAUD_)))))
#define UART_DIVMANT_SAMPLING8(_PCLK_, _BAUD_)         (UART_DIV_SAMPLING8((_PCLK_), (_BAUD_))/100U)
#define UART_DIVFRAQ_SAMPLING8(_PCLK_, _BAUD_)         ((((UART_DIV_SAMPLING8((_PCLK_), (_BAUD_)) - (UART_DIVMANT_SAMPLING8((_PCLK_), (_BAUD_)) * 100U)) * 8U)\
                                                         + 50U) / 100U)
/* UART BRR = mantissa + overflow + fraction
            = (UART DIVMANT << 4) + ((UART DIVFRAQ & 0xF8) << 1) + (UART DIVFRAQ & 0x07U) */
#define UART_BRR_SAMPLING8(_PCLK_, _BAUD_)             ((UART_DIVMANT_SAMPLING8((_PCLK_), (_BAUD_)) << 4U) + \
                                                        ((UART_DIVFRAQ_SAMPLING8((_PCLK_), (_BAUD_)) & 0xF8U) << 1U) + \
                                                        (UART_DIVFRAQ_SAMPLING8((_PCLK_), (_BAUD_)) & 0x07U))

/**
 * @brief configura el USARTx como:
 * 1-bit stop
 * sin paridad
 * 8-bit de datos
 */
void USART_Init(USART_TypeDef * USARTx,uint32_t baud);

/**
 * @brief enviar N data por el USART
 */
void USART_SendData(USART_TypeDef * USARTx,uint8_t *pTxBuffer, uint32_t Size);

/**
 * @brief enviar N data por el USART
 */
void USART_ReceiveData(USART_TypeDef * USARTx,uint8_t *pRxBuffer, uint32_t Size);

/*
 * IRQ Configuration and ISR handling
 */
void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void USART_IRQHandling(USART_Handle_t *pUSARTHandle);

/**
 * @brief enviar datos por USART mediante generacion de interrupciones
 */
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer, uint32_t Size);

/**
 * @brief recibir datos mediante interrupcion
 */
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer, uint32_t Size);

/*
 * Application Callbacks
 */
void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t ApEv);

#endif /* INC_USART_H_ */
