/*
 * delay.h
 *
 *  Created on: 12 ago. 2022
 *      Author: jeffr
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include <stm32f4xx.h>

/**
 * 0: generar retardos en us y ms
 * 1: solo retardos en ms
 */
#define DELAY_MODE 	0
/**
 * @brief funcion que configura la systick
 */
void delay_init(void);

#if (DELAY_MODE == 0)
/**
 * @brief funcion encargada de generar retardos en us
 * @param [delay]: tiempo de retado en us deseado
 */
void delay_us(uint32_t delay);
#endif
/**
 * @brief funcion encargada de generar retardos en ms
 * @param [delay]: tiempo de retado en ms deseado
 */
void delay_ms(uint32_t delay);


#endif /* INC_DELAY_H_ */
