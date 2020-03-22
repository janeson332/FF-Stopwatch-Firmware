/**
  ******************************************************************************
  * @file    LCD_i2c.c
  * @author  Stefan Jahn <stefan.jahn332@gmail.com>
  * @version V1.0
  * @date    14.02.2020
  * @brief   lcd display over i2c implementation (ported from arduino lib)
  ******************************************************************************
*/

#include <assert.h>
#include <string.h>

#include "LCD_i2c.h"

// private functions
static void expanderWrite(tLCD_InitStruct *lcd,uint8_t value);
static void pulseEnable(tLCD_InitStruct *lcd,uint8_t value);
static void write4bits(tLCD_InitStruct *lcd,uint8_t value);
static void command(tLCD_InitStruct *lcd,uint8_t cmd);
static uint32_t write(tLCD_InitStruct *lcd,uint8_t value);
static void send(tLCD_InitStruct *lcd,uint8_t value, uint8_t mode);

static void (*I2C_Write1Byte)(uint8_t addr, uint8_t value) = 0;
static void (*mdelay)(uint32_t) = 0;


void LCD_Init(tLCD_InitStruct * lcd, void (*I2C_Write1ByteFunc)(uint8_t, uint8_t),void (*delayFunc)(uint32_t)){

	assert(I2C_Write1ByteFunc != 0);
	assert(delayFunc != 0);

	I2C_Write1Byte = I2C_Write1ByteFunc;
	mdelay = delayFunc;

	// i2c has to be initialized in main ..

	lcd->backlightval = LCD_BACKLIGHT;

	lcd->displayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (lcd->rows > 1) {
		lcd->displayFunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((lcd->charsize != 0) && (lcd->rows == 1)) {
		lcd->displayFunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	mdelay(50);

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(lcd, lcd->backlightval);	// reset expander and turn backlight off (Bit 8 =1)
	mdelay(50);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(lcd, 0x03 << 4);
	mdelay(5); // wait min 4.1ms

	// second try
	write4bits(lcd, 0x03 << 4);
	mdelay(5); // wait min 4.1ms

	// third go!
	write4bits(lcd, 0x03 << 4);
	mdelay(1);

	// finally, set to 4-bit interface
	write4bits(lcd, 0x02 << 4);

	// set # lines, font size, etc.
	command(lcd, LCD_FUNCTIONSET | lcd->displayFunction);

	// turn the display on with no cursor or blinking default
	lcd->displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	LCD_Display(lcd,1);

	// clear it off
	LCD_Clear(lcd);

	// Initialize to default text direction (for roman languages)
	lcd->displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(lcd, LCD_ENTRYMODESET | lcd->displayMode);

	LCD_Home(lcd);
}

void LCD_WriteChar(tLCD_InitStruct * lcd, uint8_t ch){
	write(lcd,ch);
}

void LCD_WriteString(tLCD_InitStruct * lcd, uint8_t const * str){

	uint8_t len = strnlen(str,lcd->cols);
	for(uint8_t i=0;i<len;++i){
		LCD_WriteChar(lcd,str[i]);
	}
}

void LCD_Clear(tLCD_InitStruct * lcd){
	command(lcd, LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	mdelay(2); // this command takes a long time!
}

void LCD_Home(tLCD_InitStruct * lcd){
	command(lcd, LCD_RETURNHOME);  // set cursor position to zero
	mdelay(2); // this command takes a long time!
}

void LCD_SetCursor(tLCD_InitStruct * lcd, uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
		if (row > lcd->rows) {
			row = (lcd->rows)-1;    // we count rows starting w/0
		}
	command(lcd, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_Display(tLCD_InitStruct * lcd, uint8_t state){
	if(state == 1){
		lcd->displayControl |= LCD_DISPLAYON;
	}
	else{
		lcd->displayControl &= ~LCD_DISPLAYON;
	}
	command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void LCD_Blink(tLCD_InitStruct * lcd, uint8_t state){
	if(state == 1){
		lcd->displayControl |= LCD_BLINKON;
	}
	else{
		lcd->displayControl &= ~LCD_BLINKON;
	}
	command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void LCD_Cursor(tLCD_InitStruct * lcd, uint8_t state){
	if(state == 1){
		lcd->displayControl |= LCD_CURSORON;
	}
	else{
		lcd->displayControl &= ~LCD_CURSORON;
	}
	command(lcd, LCD_DISPLAYCONTROL | lcd->displayControl);
}

void LCD_Backlight(tLCD_InitStruct * lcd, uint8_t state){
	if(state==1){
		lcd->backlightval = LCD_BACKLIGHT;
	}
	else{
		lcd->backlightval = LCD_NOBACKLIGHT;
	}
	expanderWrite(lcd,lcd->backlightval);
}

uint8_t LCD_GetBacklightState(tLCD_InitStruct * lcd){
	if(lcd->backlightval == LCD_BACKLIGHT){
		return 1;
	}
	else{
		return 0;
	}
}

//------ private functions ---------------------------------------------------------
static void command(tLCD_InitStruct *lcd, uint8_t cmd){
	send(lcd,cmd, 0);
}

static uint32_t write(tLCD_InitStruct *lcd, uint8_t value){
	send(lcd, value,Rs);
	return 1;
}

static void send(tLCD_InitStruct *lcd, uint8_t value, uint8_t mode){
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
	write4bits(lcd,(highnib)|mode);
	write4bits(lcd,(lownib)|mode);
}

static void write4bits(tLCD_InitStruct *lcd, uint8_t value){
	expanderWrite(lcd,value);
	pulseEnable(lcd,value);
}

static void expanderWrite(tLCD_InitStruct *lcd, uint8_t value){
	I2C_Write1Byte(lcd->addr,value|lcd->backlightval);
}

static void pulseEnable(tLCD_InitStruct *lcd, uint8_t value){
	expanderWrite(lcd,value | En);	// En high
//	mdelay(1);		// enable pulse must be >450ns    /* commented out cause, due i2c transport delay already happens */

	expanderWrite(lcd,value & ~En);	// En low
//	mdelay(1); // commands need > 37us to settle
}
