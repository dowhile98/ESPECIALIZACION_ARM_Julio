/**
 * Libreria para generar retardos
 * @uator : Quino B. Jeffry
 * @version 1.0
 */

#ifndef __DELAY_H
#define __DELAY_H

/*inclusiones*/
#include "stm32l4xx.h"

/*****DEFINES***********/


/*****PROTOTIPO DE FUNCIONES************/

/**
 * @brief Configura el SysTick
 * Esta funcion configura el SysTick para generar
 * interrupciones cada 1ms.
 * @param none
 * @return none
 */
void Delay_Init(void);


/**
 * @brief Genera retardos en ms
 * Esta funcion genera retardos en ms 
 * @param delay: el tiempo en ms del retardo deseado
 * @return none
 */
void delay_ms(uint32_t delay);

#endif /*__DELAY_H*/
