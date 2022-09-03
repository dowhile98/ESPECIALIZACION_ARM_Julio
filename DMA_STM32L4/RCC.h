#ifndef __RCC_H
#define __RCC_H
/**************************INCLUDES***************************/
#include "stm32l4xx.h"
/***********************MACRO DEFINES*************************/

/*MSI*/
#define MSI_RANGE0_100kHz			(0x0<<4)
#define MSI_RANGE1_200kHz			(0x1U<<4)
#define MSI_RANGE2_400kHz			(0x2U<<4)
#define MSI_RANGE3_800kHz			(0x3U<<4)
#define MSI_RANGE4_1MHz				(0x4U<<4)
#define MSI_RANGE5_2MHz				(0x5U<<4)
#define MSI_RANGE6_4MHz				(0x6U<<4)
#define MSI_RANGE7_8MHz				(0x7U<<4)
#define MSI_RANGE8_16MHz			(0x8U<<4)
#define MSI_RANGE9_24MHz			(0x9U<<4)
#define MSI_RANGE10_32MHz			(0xAU<<4)
#define MSI_RANGE11_48MHz			(0xBU<<4)
/**********************ENUM DEFINES***************************/
/*
 *
 */
typedef enum{
	RCC_ERROR = 0,
	RCC_OK = 1U,
}RCC_Status_t;

/*********************FUNCTION PROTOTYPES*********************/


/**
 * @brief Configura la frecuencia del MSI CLOCK
 * @param MSIRange: Se define el rango de del MSI
 * @return : RCC_Status
 */
RCC_Status_t MSI_ConfigRange(uint8_t MSIRange);

/**
 * @brief Configura la frecuencia del PLL clock
* @param :none
 * @return : RCC_Status
 */
RCC_Status_t PLL_Config(void );

#endif /*__RCC_H*/

