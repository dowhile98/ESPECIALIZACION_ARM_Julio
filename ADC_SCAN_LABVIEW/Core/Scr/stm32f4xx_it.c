/*
 * stm32f4xx_it.c
 *
 *  Created on: 27 mar. 2022
 *      Author: Lenovo
 */

#include "stm32f4xx.h"
#include "USART.h"


extern  USART_Handle_t usart2_handle;
void USART2_IRQHandler(void)
{
	USART_IRQHandling(&usart2_handle);
}
