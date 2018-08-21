#ifndef __SYS_H
#define __SYS_H	 

#include <stm32f10x.h>   

//#include <stm32f10x_lib.h>
//JTAG模式设置定义
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00	
//void BKP_Write(u8 reg,u16 dat);	//写入后备寄存器
void Stm32_Clock_Init(u8 PLL);      //时钟初始化  
void Sys_Soft_Reset(void);          //系统软复位
void Sys_Standby(void);             //待机模式 	
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);//设置偏移地址
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);//设置NVIC分组
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//设置中断
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);//外部中断配置函数(只对GPIOA~G)
void JTAG_Set(u8 mode);

#endif
