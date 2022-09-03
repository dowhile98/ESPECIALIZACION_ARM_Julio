/**
 * HEADER PARA CONFIGURAR ALGUNOS MODOS DE FUNCIONAMIENTO DE LOS TIMERS
 * @autor QUINO B. JEFFRY
 */
 
#ifndef __TIMER_H
#define __TIMER_H

/*includes*/
#include "stm32l4xx.h"

/*DEFINICION DE MACROS*/
#define TIM_IT_ENABLE 			1
#define TIM_IT_DISABLE			0

/*FILTER: IC1F*/
#define TIM_IC_IC1F_NO_FITER							0x0<<4
#define TIM_IC_IC1F_FCK_INT_N_2						0x1<<4
#define TIM_IC_IC1F_FCK_INT_N_4						0x2<<4
#define TIM_IC_IC1F_FCK_INT_N_8						0x3<<4
#define TIM_IC_IC1F_FDTS_2_N_6						0x4<<4
#define TIM_IC_IC1F_FDTS_2_N_8						0x5<<4
#define TIM_IC_IC1F_FDTS_4_N_6						0x6<<4
#define TIM_IC_IC1F_FDTS_4_N_8						0x7<<4
#define TIM_IC_IC1F_FDTS_8_N_6						0x8<<4
#define TIM_IC_IC1F_FDTS_8_N_8						0x9<<4
#define TIM_IC_IC1F_FDTS_16_N_5						0xA<<4
#define TIM_IC_IC1F_FDTS_16_N_6						0xB<<4
#define TIM_IC_IC1F_FDTS_16_N_8						0xC<<4
#define TIM_IC_IC1F_FDTS_32_N_5						0xD<<4
#define TIM_IC_IC1F_FDTS_32_N_6						0xE<<4
#define TIM_IC_IC1F_FDTS_32_N_8						0xF<<4

/*PRESCALER: IC1PSC*/
#define TIM_IC_IC1PSC_NO_PSC							0x0<<2
#define TIM_IC_IC1PSC_2										0x1<<2
#define TIM_IC_IC1PSC_4										0x2<<2
#define TIM_IC_IC1PCS_8										0x3<<2



/*definir la polaradid de captura*/
/* CC1NP | CC1P */
#define TIM_IC_RISING											(0<<3 | 0<<1)
#define TIM_IC_FALLING										(0<<3 | 1<<1)
#define TIM_IC_BOTH												(1<<3 | 1<<1)

/*PROTOTIPO DE FUNCIONES*/


/**
 * @brief Configura el TIMER6 para generar base de tiempo
 * @param psc: Valor del prescaler para el timer
 * @param arr: Periodo de conteo del timer 
 * @param IT: si se quiere generar interrupciones se le pasa un valor
 */
void TIMER6_BaseTime(uint32_t psc, uint32_t arr, uint32_t IT);

/**
 * @brief Configura el CH1 del TIMER2 como captura de entrada
 * @param psc: Valor del prescaler para el timer
 * @param arr: Periodo de conteo del timer 
 * @param IC1F: valor del filtro para la entrada
 * @param IC1PSC: valor del prescaler para la entrada
 * @param POLARITY: flanco activa para la capture de entrada
 * @param IT: si se quiere generar interrupciones se le pasa un valor
 */
void TIMER2_CH1_IC_Start(uint32_t psc, uint32_t arr,uint32_t IC1F,uint32_t IC1PSC, uint32_t POLARITY, uint32_t IT);

/**
 * @brief Configura el CH1 del TIMER2 como comparacion de salida
 * @param psc: Valor del prescaler para el timer
 * @param arr: Periodo de conteo del timer 
 * @param dutty: ciclo de trabajo del PWM
 */
void TIMER16_CH1_OC_PWM_Start(uint32_t psc, uint32_t arr,uint32_t duty);

#endif /*__TIMER_H*/

