#ifndef __SYS_H
#define __SYS_H	 

#include <stm32f10x.h>   

//#include <stm32f10x_lib.h>
//JTAGģʽ���ö���
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00	
//void BKP_Write(u8 reg,u16 dat);	//д��󱸼Ĵ���
void Stm32_Clock_Init(u8 PLL);      //ʱ�ӳ�ʼ��  
void Sys_Soft_Reset(void);          //ϵͳ��λ
void Sys_Standby(void);             //����ģʽ 	
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);//����ƫ�Ƶ�ַ
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);//����NVIC����
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//�����ж�
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);//�ⲿ�ж����ú���(ֻ��GPIOA~G)
void JTAG_Set(u8 mode);

#endif
