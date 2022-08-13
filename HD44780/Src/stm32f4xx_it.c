/*
 * stm32f4xx_it.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */

#include "stm32f4xx.h"

extern uint32_t uwTick;

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
	uwTick++;
}

