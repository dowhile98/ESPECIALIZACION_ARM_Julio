/**
 *
 */

#ifndef __DELAY_H
#define __DELAY_H


/*Includes -------------------------------------*/
#include <stm32f4xx.h>


/*Function prototype ---------------------------*/
void delay_init_it(void);

/**
 * @brief funcion encargada de generar retardos en ms
 * @param [delay]: tiempo de retado en ms deseado
 */
void delay_ms(uint32_t delay);

/**
 * @brief funcion similar a la funcion de arduino
 */
uint32_t millis(void);

#endif /*__DELAY_H*/
