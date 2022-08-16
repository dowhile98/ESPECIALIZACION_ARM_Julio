/*
 * test.c
 *
 *  Created on: 15 ago. 2022
 *      Author: jeffr
 */
#include <stm32f4xx.h>
#include "defines.h"

void EXTI_ApplicationCallback(uint32_t pinnumber){
	GPIOX_ODR(LED) ^= 1;
}
