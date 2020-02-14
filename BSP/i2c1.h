/*********************************************************************
File    : i2c.h
Purpose : 
**********************************************************************/
#ifndef __I2C1_H__
#define __I2C1_H__
/****************************** Includes *****************************/
/****************************** Defines *******************************/


void I2C1_Init(void);

int I2C1_Read(unsigned char Adress, unsigned char data);
int I2C1_ReadRegister(unsigned char Address, unsigned char RegisterAddr,
                                          unsigned short RegisterLen, unsigned char *RegisterValue);
int I2C1_WriteRegister(unsigned char Address, unsigned char RegisterAddr,
                                           unsigned short RegisterLen, const unsigned char *RegisterValue);
 
#endif // __I2C_H__


