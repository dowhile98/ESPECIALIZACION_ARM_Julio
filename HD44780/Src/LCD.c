/*
 * LCD.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */


#include "LCD.h"

/* Private HD44780 structure */
typedef struct {
	uint8_t DisplayControl;
	uint8_t DisplayFunction;
	uint8_t DisplayMode;
	uint8_t Rows;
	uint8_t Cols;
	uint8_t currentX;
	uint8_t currentY;
} LCD_Options_t;

/* Private variable */
static LCD_Options_t LCD_Opts;


/* Private functions */
static void LCD_InitPins(void);
static void LCD_Cmd(uint8_t cmd);
static void LCD_nibble_write(char data, unsigned char control);
static void LCD_Data(uint8_t data);
static void LCD_CursorSet(uint8_t col, uint8_t row);

/* Pin definitions */
#define LCD_RS_LOW              GPIOX_ODR(LCD_RS) = 0
#define LCD_RS_HIGH             GPIOX_ODR(LCD_RS) = 1
#define LCD_E_LOW               GPIOX_ODR(LCD_E) = 0
#define LCD_E_HIGH              GPIOX_ODR(LCD_E) = 1

#define LCD_E_BLINK             LCD_E_HIGH; LCD_Delay(4); LCD_E_LOW; LCD_Delay(4)
#define LCD_Delay(x)        	delay_ms(x)


/* Commands*/
#define HD44780_CLEARDISPLAY        0x01
#define HD44780_RETURNHOME          0x02
#define HD44780_ENTRYMODESET        0x04
#define HD44780_DISPLAYCONTROL      0x08
#define HD44780_CURSORSHIFT         0x10
#define HD44780_FUNCTIONSET         0x20
#define HD44780_SETCGRAMADDR        0x40
#define HD44780_SETDDRAMADDR        0x80

/* Flags for display entry mode */
#define HD44780_ENTRYRIGHT          0x00
#define HD44780_ENTRYLEFT           0x02
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYSHIFTDECREMENT 0x00

/* Flags for display on/off control */
#define HD44780_DISPLAYON           0x04
#define HD44780_CURSORON            0x02
#define HD44780_BLINKON             0x01

/* Flags for display/cursor shift */
#define HD44780_DISPLAYMOVE         0x08
#define HD44780_CURSORMOVE          0x00
#define HD44780_MOVERIGHT           0x04
#define HD44780_MOVELEFT            0x00

/* Flags for function set */
#define HD44780_8BITMODE            0x10
#define HD44780_4BITMODE            0x00
#define HD44780_2LINE               0x08
#define HD44780_1LINE               0x00
#define HD44780_5x10DOTS            0x04
#define HD44780_5x8DOTS             0x00

/***************************************************************/
void LCD_Init(uint8_t cols, uint8_t rows) {
	/* Initialize delay */
	delay_init();
	/* Init pinout */
	LCD_InitPins();
	/* At least 40ms */
	LCD_Delay(45);

	/* Set LCD width and height */
	LCD_Opts.Rows = rows;
	LCD_Opts.Cols = cols;

	/* Set cursor pointer to beginning for LCD */
	LCD_Opts.currentX = 0;
	LCD_Opts.currentY = 0;


	LCD_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
	if (rows > 1) {
		LCD_Opts.DisplayFunction |= HD44780_2LINE;
	}

	/* Try to set 4bit mode */
	LCD_nibble_write(0x03,0);
	LCD_Delay(45);

	/* Second try */
	LCD_nibble_write(0x03,0);
	LCD_Delay(45);

	/* Third goo! */
	LCD_nibble_write(0x03,0);
	LCD_Delay(45);

	/* Set 4-bit interface */
	LCD_nibble_write(0x02,0);
	LCD_Delay(1);

	/* Set # lines, font size, etc. */
	LCD_Cmd(HD44780_FUNCTIONSET | LCD_Opts.DisplayFunction);

	/* Turn the display on with no cursor or blinking default */
	LCD_Opts.DisplayControl = HD44780_DISPLAYON;
	LCD_DisplayOn();

	/* Clear lcd */
	LCD_Clear();

	/* Default font directions */
	LCD_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
	LCD_Cmd(HD44780_ENTRYMODESET | LCD_Opts.DisplayMode);

	/* Delay */
	LCD_Delay(45);
	return;
}


void LCD_Clear(void) {
	LCD_Cmd(HD44780_CLEARDISPLAY);
	LCD_Delay(3);
}
void LCD_Puts(uint8_t x, uint8_t y, char* str) {
	LCD_CursorSet(x, y);
	while (*str) {
		if (LCD_Opts.currentX >= LCD_Opts.Cols) {
			LCD_Opts.currentX = 0;
			LCD_Opts.currentY++;
			LCD_CursorSet(LCD_Opts.currentX, LCD_Opts.currentY);
		}
		if (*str == '\n') {
			LCD_Opts.currentY++;
			LCD_CursorSet(LCD_Opts.currentX, LCD_Opts.currentY);
		} else if (*str == '\r') {
			LCD_CursorSet(0, LCD_Opts.currentY);
		} else {
			LCD_Data(*str);
			LCD_Opts.currentX++;
		}
		str++;
	}
}


void LCD_DisplayOn(void) {
	LCD_Opts.DisplayControl |= HD44780_DISPLAYON;
	LCD_Cmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_DisplayOff(void) {
	LCD_Opts.DisplayControl &= ~HD44780_DISPLAYON;
	LCD_Cmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_BlinkOn(void) {
	LCD_Opts.DisplayControl |= HD44780_BLINKON;
	LCD_Cmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_BlinkOff(void) {
	LCD_Opts.DisplayControl &= ~HD44780_BLINKON;
	LCD_Cmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_CursorOn(void) {
	LCD_Opts.DisplayControl |= HD44780_CURSORON;
	LCD_Cmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_CursorOff(void) {
	LCD_Opts.DisplayControl &= ~HD44780_CURSORON;
	LCD_Cmd(HD44780_DISPLAYCONTROL | LCD_Opts.DisplayControl);
}

void LCD_ScrollLeft(void) {
	LCD_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void LCD_ScrollRight(void) {
	LCD_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}
void LCD_CreateChar(uint8_t location, uint8_t *data) {
	uint8_t i;
	/* We have 8 locations available for custom characters */
	location &= 0x07;
	LCD_Cmd(HD44780_SETCGRAMADDR | (location << 3));

	for (i = 0; i < 8; i++) {
		LCD_Data(data[i]);
	}
}

void LCD_PutCustom(uint8_t x, uint8_t y, uint8_t location) {
	LCD_CursorSet(x, y);
	LCD_Data(location);
}


void LCD_Printf(uint8_t x, uint8_t y,char *format,...){
	char str[80];

	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format,args);
	LCD_Puts(x, y,str);
	va_end(args);
}

/***************************************************************/

/* Private functions */
static void LCD_InitPins(void){
	/*enable clock*/
	RCC->AHB1ENR |= GPIOX_CLOCK(LCD_RS) | GPIOX_CLOCK(LCD_E) | GPIOX_CLOCK(LCD_D4) | \
					GPIOX_CLOCK(LCD_D5) | GPIOX_CLOCK(LCD_D6) | GPIOX_CLOCK(LCD_D7);
	/*Pinout configuration*/
	GPIOX_MODER(MODE_OUT,LCD_RS);
	GPIOX_OTYPER(MODE_OT_PP,LCD_RS);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,LCD_RS);
	GPIOX_PUPDR(MODE_PU_NONE,LCD_RS);

	GPIOX_MODER(MODE_OUT,LCD_E);
	GPIOX_OTYPER(MODE_OT_PP,LCD_E);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,LCD_E);
	GPIOX_PUPDR(MODE_PU_NONE,LCD_E);

	GPIOX_MODER(MODE_OUT,LCD_D4);
	GPIOX_OTYPER(MODE_OT_PP,LCD_D4);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,LCD_D4);
	GPIOX_PUPDR(MODE_PU_NONE,LCD_D4);

	GPIOX_MODER(MODE_OUT,LCD_D5);
	GPIOX_OTYPER(MODE_OT_PP,LCD_D5);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,LCD_D5);
	GPIOX_PUPDR(MODE_PU_NONE,LCD_D5);

	GPIOX_MODER(MODE_OUT,LCD_D6);
	GPIOX_OTYPER(MODE_OT_PP,LCD_D6);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,LCD_D6);
	GPIOX_PUPDR(MODE_PU_NONE,LCD_D6);

	GPIOX_MODER(MODE_OUT,LCD_D7);
	GPIOX_OTYPER(MODE_OT_PP,LCD_D7);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,LCD_D7);
	GPIOX_PUPDR(MODE_PU_NONE,LCD_D7);

	return;
}


static void LCD_nibble_write(char data, unsigned char control){
	uint32_t delay = 1;
	GPIOX_ODR(LCD_D7) = (data>>3) & 0x1;
	GPIOX_ODR(LCD_D6) = (data>>2) & 0x1;
	GPIOX_ODR(LCD_D5) = (data>>1) & 0x1;
	GPIOX_ODR(LCD_D4) = (data) & 0x1;

	GPIOX_ODR(LCD_E) = 0;

	if(control){
		/* Data mode */
		LCD_RS_HIGH;
	}else{
		/* Command mode */
		LCD_RS_LOW;
	}
	/*se genera un pulso*/
	GPIOX_ODR(LCD_E) = 1;
	while(delay-->0);
	GPIOX_ODR(LCD_E) = 0;

}
static void LCD_Cmd(uint8_t cmd){
	LCD_nibble_write((cmd>>4) & 0x0F,0);
	LCD_nibble_write(cmd & 0x0F,0);
	if(cmd < 4)
		LCD_Delay(3);
	else
		LCD_Delay(2);
	return;
}

static void LCD_Data(uint8_t data){
	LCD_nibble_write((data>>4) & 0x0F,1);
	LCD_nibble_write(data & 0x0F,1);
	LCD_Delay(1);
	return;
}
static void LCD_CursorSet(uint8_t col, uint8_t row){
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	/* Go to beginning */
	if (row >= LCD_Opts.Rows) {
		row = 0;
	}
	/* Set current column and row */
	LCD_Opts.currentX = col;
	LCD_Opts.currentY = row;

	/* Set location address */
	LCD_Cmd(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}
