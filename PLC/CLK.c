/********************************************************/
// CPU需要：STM32F103--RAM内存不小于64K	Flash内存不小于128K
// 本代码已在STM32F103RCT6 RDT6 VCT6 VET6测试通过
// 编辑日期：20150903
// editor by 传人记
/********************************************************/

#include "stm32f10x_it.h"
#include "stm32f10x_tim.h"
#include <stdio.h> 
extern u16 all_data[16600];	 //
extern u8  p_all_data[];		 //
extern void x_filter(void);  //	   输入X信号滤波1MS一次
extern void force_reset(u16 start_addr,u8 process_addr);  //用于强制软件元使用
extern void force_set(u16 start_addr,u8 process_addr);	  //
extern void filter(void);	//传人记  20151122	adc
extern void DAC_data(void);	//传人记  20151211	dac
u16 temp[5];
u16 *p_data_given,*p_reset_coil,*p_over_coil,*p_enable_coil,*p_value;

void timer_enable(u16 timer_number)
{     
	p_data_given=all_data+0x1000+timer_number;		  //计数器值地址
	p_value=all_data+0x0800+timer_number;			  //
	p_over_coil=all_data+0x0100+(timer_number/0x10);	  //溢出线圈
	if(*p_value<*p_data_given)
	*p_over_coil&=~(1<<timer_number%0x10); //值小于设定值时OFF
	else
	*p_over_coil|=(1<<timer_number%0x10);   //值到达设定值时ON
}

void timer_disble(u16 timer_number)
{     
	p_data_given=all_data+0x1000+timer_number;		  //计数器值地址
	p_value=all_data+0x0800+timer_number;			  //
	p_reset_coil=all_data+0x0380+(timer_number/0x10);  //复位线圈
	p_over_coil=all_data+0x0100+(timer_number/0x10);	  //溢出线圈
	p_enable_coil=all_data+0x0280+(timer_number/0x10); //使能线圈
	*p_reset_coil&=~(1<<timer_number%0x10);  //复位线圈
	*p_over_coil&=~(1<<timer_number%0x10);	  //溢出线圈
	*p_enable_coil&=~(1<<timer_number%0x10); //使能线圈
	*p_data_given=0;
	*p_value=0;
}

void T_100MS(void)
{
	u16 timer_count;
	for(timer_count=0;timer_count<200;timer_count++)
	{
		p_data_given=all_data+0x1000+timer_count;
		p_value=all_data+0x0800+timer_count;
		p_enable_coil=all_data+0x0280+(timer_count/0x10);
		if((*p_enable_coil&(1<<timer_count%0x10))==(1<<timer_count%0x10))  //线圈状态
		{
			if(*p_value<*p_data_given)						 //值状态
			*p_value+=1;
		}
	}
}

void T_10MS(void)
{ 
	u16 timer_count;
	for(timer_count=200;timer_count<246;timer_count++)
	{ 
		p_data_given=all_data+0x1000+timer_count;
		p_value=all_data+0x0800+timer_count;
		p_enable_coil=all_data+0x0280+(timer_count/0x10);
		if((*p_enable_coil&(1<<timer_count%0x10))==(1<<timer_count%0x10))  //线圈状态
		{
			if(*p_value<*p_data_given)						 //值状态
			*p_value+=1;
		}
	}
}
void T_1MS(void)
{
	u16 timer_count;
	for(timer_count=246;timer_count<250;timer_count++)

	{ 
		p_data_given=all_data+0x1000+timer_count;
		p_value=all_data+0x0800+timer_count;
		p_enable_coil=all_data+0x0280+(timer_count/0x10);
		if((*p_enable_coil&(1<<timer_count%0x10))==(1<<timer_count%0x10))  //线圈状态
		{
			if(*p_value<*p_data_given)						 //值状态
			*p_value+=1;
		}
	}
}
void T_H100MS(void)
{ 
	u16 timer_count; 	
	for(timer_count=250;timer_count<256;timer_count++)
	{ 
		p_data_given=all_data+0x1000+timer_count;
		p_value=all_data+0x0800+timer_count;
		p_enable_coil=all_data+0x0280+(timer_count/0x10);
		if((*p_enable_coil&(1<<timer_count%0x10))==(1<<timer_count%0x10))  //线圈状态
		{
			if(*p_value<*p_data_given)						 //值状态
			*p_value+=1;
		}
	}
}

void TIM5_IRQHandler(void)//1ms产生一次中断信号
{ 
	static u8 all_clock;
	static u16 minute,test;
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
	{	
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		all_clock++;
//		all_data[0x070C]++;
        T_1MS();
		if(all_clock>99) 	   //m8011 10MS m8012 100MS  m8013 1SEC m8014 1minute
		{
			all_clock=0,force_set(0X00E0,12);
//			T_100MS();
		}

		if((all_clock%10)==7)  //10ms定时器设计每计五次刷新一次
		{
			T_10MS();
			force_reset(0X00E0,11);
		}
		if((all_clock%10)==2)
		{
		   force_set(0X00E0,11);   
		}

		if(all_clock==50)	//两种100MS定时器分开刷新
		T_100MS(),force_reset(0X00E0,12);
			
		if(all_clock==90)	 //每100ms秒钟分钟定时器
		T_H100MS(),minute++;
		
	    if((all_clock%0x10)==0x02)	    //更新一次DAC数据
		{
		    DAC_data();	   //ADC  10ms刷新一次 //更新一次DAC数据
			filter(); 
		}
		
		if(minute%10==5)	  // 刷新秒钟8013
		force_reset(0X00E0,13);
		if(minute%10==0)
		force_set(0X00E0,13);

		if(minute==300)			//刷新分钟8014
		force_reset(0X00E0,14);
		if(minute==0)
		force_set(0X00E0,14);

		if(minute>599)
		minute=0;		  
		x_filter();       //检查X输入状态值	
		test++;
	}
}

void TIM5_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	//这个就是自动装载的计数值，由于计数是从0开始的，计数10000次后为9999
	TIM_TimeBaseStructure.TIM_Period = (100 - 1);
	// 这个就是预分频系数，当由于为0时表示不分频所以要减1
	TIM_TimeBaseStructure.TIM_Prescaler = (720 - 1);
	// 高级应用本次不涉及。定义在定时器时钟(CK_INT)频率与数字滤波器(ETR,TIx)
	// 使用的采样频率之间的分频比例
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//向上计数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//初始化定时器5
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	/* Clear TIM5 update pending flag[清除TIM5溢出中断标志] */
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	/* TIM IT enable */ //打开溢出中断
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM5, ENABLE);  //计数器使能，开始工作
}

