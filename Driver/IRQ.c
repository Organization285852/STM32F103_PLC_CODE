/********************************************************/
// CPU需要：STM32F103--RAM内存不小于64K	Flash内存不小于128K
// 本代码已在STM32F103RCT6 RDT6 VCT6 VET6测试通过
// 编辑日期：20150903
// editor by 传人记
/********************************************************/

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include <io.h>
#include <stdio.h>
//#include <absacc.h> 

void NVIC_Configuration(void)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;	//定时器5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);			  
}


