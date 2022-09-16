/*
 * LCD.h
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */

#ifndef LCD_H_
#define LCD_H_

#include "delay.h"
#include "stm32f4xx.h"
#include "Config.h"
#include <stdarg.h>
#include <stdio.h>

/*
 * 0: Interfaz paralela de 4bits
 * 1: Interfaz I2C
 */
#define USE_I2C_INTERFACE 		1
/*
 * definir su funcion que genera retardo en ms
 */
#define LCD_Delay(x)        	delay_ms(x)


#if  USE_I2C_INTERFACE
#define LCD_I2C					I2C1
#define SLAVE_ADDRESS_LCD 		0x3F
extern void I2C_MasterSendData(I2C_TypeDef *I2Cx, uint8_t SlaveAddr,uint8_t *pTxbuffer, uint32_t Len);
#else
/* 4 bit mode */
/* Control pins, can be overwritten */
/* RS - Register select pin */
#define LCD_RS					A , 8
/* E - Enable pin */
#define LCD_E					A , 9
/* Data pins */
/* D4 - Data 4 pin */
#define LCD_D4					C , 7
/* D5 - Data 5 pin */
#define LCD_D5					B , 6
/* D6 - Data 6 pin */
#define LCD_D6					A , 7
/* D7 - Data 7 pin */
#define LCD_D7					A , 6

#endif /*USE I2C INTERFACE*/
/**
 * @brief  Initializes HD44780 LCD
 * @brief  cols: width of lcd
 * @param  rows: height of lcd
 * @retval None
 */
void LCD_Init(uint8_t cols, uint8_t rows);

/**
 * @brief  Turn display on
 * @param  None
 * @retval None
 */
void LCD_DisplayOn(void);

/**
 * @brief  Turn display off
 * @param  None
 * @retval None
 */
void LCD_DisplayOff(void);

/**
 * @brief  Clears entire LCD
 * @param  None
 * @retval None
 */
void LCD_Clear(void);

/**
 * @brief  Puts string on lcd
 * @param  x location
 * @param  y location
 * @param  *str: pointer to string to display
 * @retval None
 */
void LCD_Puts(uint8_t x, uint8_t y, char* str);

/**
 * @brief  Enables cursor blink
 * @param  None
 * @retval None
 */
void LCD_BlinkOn(void);

/**
 * @brief  Disables cursor blink
 * @param  None
 * @retval None
 */
void LCD_BlinkOff(void);

/**
 * @brief  Shows cursor
 * @param  None
 * @retval None
 */
void LCD_CursorOn(void);

/**
 * @brief  Hides cursor
 * @param  None
 * @retval None
 */
void LCD_CursorOff(void);

/**
 * @brief  Scrolls display to the left
 * @param  None
 * @retval None
 */
void LCD_ScrollLeft(void);

/**
 * @brief  Scrolls display to the right
 * @param  None
 * @retval None
 */
void LCD_ScrollRight(void);

/**
 * @brief  Creates custom character
 * @param  location: Location where to save character on LCD. LCD supports up to 8 custom characters, so locations are 0 - 7
 * @param *data: Pointer to 8-bytes of data for one character
 * @retval None
 */
void LCD_CreateChar(uint8_t location, uint8_t* data);

/**
 * @brief  Puts custom created character on LCD
 * @param  location: Location on LCD where character is stored, 0 - 7
 * @retval None
 */
void LCD_PutCustom(uint8_t x, uint8_t y, uint8_t location);

/**
 * @brief  Puts string on lcd
 * @param  x location
 * @param  y location
 * @param  *str: pointer to string to display
 * @retval None
 */
void LCD_Printf(uint8_t x, uint8_t y,char *format,...);

#endif /* LCD_H_ */
