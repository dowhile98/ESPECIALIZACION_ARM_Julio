/*
 * delay.h
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f4xx.h"


/**
 * 1: retardo en milisegundos y  microsegundos
 * 0: retardo en milisegundos
 */
#define USE_DELAY_US		0
/**
 * @brief inicializa la funcion que genera retardos en milisegundos/microsegundos
 */
void delay_init(void);

/**
 * @brief funcion que genera retardos en milisegudos
 * @param[delay]: cantidad de retardos en milisegundos
 * @return none
 */
void delay_ms(uint32_t delay);

#if USE_DELAY_US

/**
 * @brief funcion que genera retardos en microsegundos
 * @param[delay]: cantidad de retardos en milisegundos
 * @return none
 */
void delay_us(uint32_t delay);

#endif



#endif /* DELAY_H_ */
