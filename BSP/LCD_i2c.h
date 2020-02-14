#ifndef LCD_I2C_H_INCLUDED
#define LCD_I2C_H_INCLUDED

#include <stdint.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01 // Register select bit

typedef struct{
	uint8_t addr;
	uint8_t cols;
	uint8_t rows;
	uint8_t charsize;

	uint8_t displayFunction;
	uint8_t displayControl;
	uint8_t displayMode;
	uint8_t backlightval;
}tLCD_InitStruct;


void LCD_Init(tLCD_InitStruct * lcd, void (*I2C_Write1ByteFunc)(uint8_t, uint8_t),void (*delayFunc)(uint32_t));

void LCD_WriteChar(tLCD_InitStruct * lcd, uint8_t ch);

void LCD_WriteString(tLCD_InitStruct * lcd, uint8_t const * str);

void LCD_Clear(tLCD_InitStruct * lcd);

void LCD_Home(tLCD_InitStruct * lcd);

void LCD_SetCursor(tLCD_InitStruct * lcd, uint8_t col, uint8_t row);

void LCD_Display(tLCD_InitStruct * lcd, uint8_t state);

void LCD_Blink(tLCD_InitStruct * lcd, uint8_t state);

void LCD_Cursor(tLCD_InitStruct * lcd, uint8_t state);

void LCD_Backlight(tLCD_InitStruct * lcd, uint8_t state);

uint8_t LCD_GetBacklightState(tLCD_InitStruct * lcd);



#endif
