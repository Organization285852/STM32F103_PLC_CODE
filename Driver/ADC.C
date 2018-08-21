
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dma.h"
#include <stdio.h>       

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)  //ADC1 DR寄存器基地址
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define N  100  
#define M  2    
// 注：ADC为12位模数转换器，只有ADCConvertedValue的低12位有效
__IO uint16_t ADCConvertedValue[N][M];

//extern u16 PLC_16BIT[12100];		 
extern u16  all_data[16600];
extern void DAC_out_init(void);


void RCC_Configuration(void)  
{ 
	ADC_InitTypeDef  ADC_InitStructure;      //ADC初始化结构体声明
	DMA_InitTypeDef  DMA_InitStructure;      //DMA初始化结构体声明  
	
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);  
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Configure PC.00 (ADC Channel10) as analog input -------------------------*/
	//PC0 作为模拟通道10输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;                // DMA对应的外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;       // 内存存储基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	                       // DMA的转换模式为SRC模式，由外设搬移到内存
	DMA_InitStructure.DMA_BufferSize = N*M;		                                 // DMA缓存大小，单位为DMA_MemoryDataSize
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	         // 接收一次数据后，设备地址禁止后移
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	                   // 接收一次数据后，目标内存地址后移
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 定义外设数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;        // DMA搬数据尺寸，HalfWord就是为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                            // 转换模式，循环缓存模式。
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                       // DMA优先级高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		                           // M2M模式禁用
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);         
	
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                       // 独立的转换模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		                         // 开启扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                       // 开启连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	     // ADC外部开关，关闭状态
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                   // 对齐方式,ADC为12位中，右对齐方式
	ADC_InitStructure.ADC_NbrOfChannel = M;	                                 // 开启通道数，4个
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channel10 configuration ADC通道组， 第10个通道 采样顺序1，转换时间 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);

	
	/* ADC命令，使能 */
	ADC_DMACmd(ADC1, ENABLE);	  
	/* 开启ADC1 */
	ADC_Cmd(ADC1, ENABLE);  
	/* 重新校准 */   
	ADC_ResetCalibration(ADC1);	  
	/* 等待重新校准完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));
	/* 开始校准 */
	ADC_StartCalibration(ADC1);		
	/* 等待校准完成 */
	while(ADC_GetCalibrationStatus(ADC1));	  
	/* 连续转换开始，ADC通过DMA方式不断的更新RAM */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
}


	 
void ADC_init(void)
{	
	RCC_Configuration();
	DAC_out_init();
}

void filter(void)
{ 
	int sum=0;
	u8  count,i;
	for(i=0;i<M;i++)		 //
	{ 
		for(count=0;count<N;count++)
		{
			sum+=ADCConvertedValue[count][i];
		}
		all_data[0x378E+i] =sum/N;	
		sum=0;
	}
}
