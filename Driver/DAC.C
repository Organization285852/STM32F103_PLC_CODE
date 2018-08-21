#include "stm32f10x.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include "PLC_CONF.H"


void DAC_out_init(void)
{ 
	DAC_InitTypeDef  DAC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIOA Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* DAC Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;      
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);
	DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
	
	DAC_SetChannel1Data(DAC_Align_12b_R,0);
	DAC_SetChannel2Data(DAC_Align_12b_R,0);
	
	DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);

}

void DAC_data(void)
{ 
	DAC_SetChannel1Data(DAC_Align_12b_R,D7030);
	DAC_SetChannel2Data(DAC_Align_12b_R,D7031);

	DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
}


