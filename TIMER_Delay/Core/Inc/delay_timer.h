/*
 * delay_timer.h
 *
 *  Created on: 22 ago. 2022
 *      Author: jeffr
 */

#ifndef INC_DELAY_TIMER_H_
#define INC_DELAY_TIMER_H_

/*includes ................................*/
#include <stm32f4xx.h>
/*Defines ---------------------------------*/

/*function prototype ----------------------*/
/**
 * @brief funcion para generar retardos en ms
 * @param [tick]: valor
 */
void delay_tim4_init(uint32_t ck_psc);
/**
 * @brief genera retardos en ms
 * @param [ms]: retardo en ms deseado
 */
void delayms(uint32_t ms);

#endif /* INC_DELAY_TIMER_H_ */
