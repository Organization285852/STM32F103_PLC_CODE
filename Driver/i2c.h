/******************************************************************************
文  件  名：wx_i2c.h
文件说明：i2c总线eeprom操作头文件
调用库版本：V3.0
日        期：11-5-30
  ******************************************************************************/

#ifndef _I2C_H
#define _I2C_H

#include "stm32f10x.h"

#define IIC_Write_Address 0xa0 //Write Address
#define IIC_Read_Address 0xa1 //Read Address
void delay_5us(u16 num);
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Wait_ack(void);
void IIC_Send_noack(void);
void IIC_Writebyte(u8 Data);
u8 IIC_Readbyte(void);
void IIC_Write(u16 Address,u8 Data);
void IIC_Write_Array(u8 *Data,u16 Address,u16 Num);
u8 IIC_Read(u16 Address);
void IIC_Read_Array(u8 *Data,u16 Address,u16 Num);
void IIC_Clear(u16 Address,u16 Num);



#endif

/*******************************************************************************************************
					endfile!!!
********************************************************************************************************/

