#ifndef __USART_H /* if not defined*/
#define __USART_H

/*includes*/
#include "stm32l4xx.h"                  // Device header

/*DEFINICION DE MACROS*/
#define USART_OVER8_16			0x0<<15
#define USART_OVER8_8				0x1U<<15

/*PROTOTIPO DE FUNCIONES*/

/**
 * @brief CONFIGURA EL USART1
 * @param fck: La freq. para generacion de baudios
 * @param OVER8: valor de oversampling deseado
 * @param txRxBaud: La taza de baudios deseada
 * @return nada
 */
void USART1_Config(uint32_t fck, uint32_t OVER8,uint32_t txRxBaud);

/**
 * @brief Envia un byte por el usart1
 * @param ch: byte a enviar
 */
void USART1_putc(char ch);
/**
 * @brief Lee un byte por el usart1
 * @return retorna el byte recibido
 */
char USART1_getc(void);
	
	
/**
 * @brief CONFIGURA EL USART2
 * @param fck: La freq. para generacion de baudios
 * @param OVER8: valor de oversampling deseado
 * @param txRxBaud: La taza de baudios deseada
 * @return nada
 */
void USART2_Config(uint32_t fck, uint32_t OVER8,uint32_t txRxBaud);

/**
 * @brief Envia un byte por el usart1
 * @param ch: byte a enviar
 */
void USART2_putc(char ch);
/**
 * @brief Lee un byte por el usart1
 * @return retorna el byte recibido
 */
char USART2_getc(void);
	



#endif /*__USART_H*/

