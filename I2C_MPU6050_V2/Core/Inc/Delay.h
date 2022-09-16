/*
 * Delay.h
 *
 *  Created on: 31 ene. 2022
 *      Author: Lenovo
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32f4xx.h"

/**
 * USE_DELAY_US
 * 0 : solo retardos en milisegundos
 * 1 : retardos en microsegundos/milisegundos
 */
#define USE_DELAY_US  0


#if USE_DELAY_US == 1

/**
 * @brief funcion que va generar retardos en microsegundos
 */
void delay_us(uint32_t delay);
#endif
/**
 * delay init
 * @param [ticks] : obtenido de la formula ticks = SystemCoreClock * T(s)
 */
void delay_Init(uint32_t ticks);
/**
 * @brief funcion que va generar retardos en milisegundos
 */
void delay_ms(uint32_t delay);

#endif /* INC_DELAY_H_ */
