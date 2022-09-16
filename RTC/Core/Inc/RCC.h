/*
 * RCC.h
 *
 *  Created on: 23 ene. 2022
 *      Author: Lenovo
 */

#ifndef RCC_H_
#define RCC_H_

#include "stm32f4xx.h"

/**
 * Configuracion de la flash
 */
void flash_cofig(void);
/**
 * Selecciona el HSE como sysclk
 */
void HSE_Config(void);
/**
 * Configurar el PLL  a 84MZh
 */
void PLL_Config(void);

#endif /* RCC_H_ */
