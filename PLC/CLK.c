/********************************************************/
// CPU��Ҫ��STM32F103--RAM�ڴ治С��64K	Flash�ڴ治С��128K
// ����������STM32F103RCT6 RDT6 VCT6 VET6����ͨ��
// �༭���ڣ�20150903
// editor by ���˼�
/********************************************************/

#include "stm32f10x_it.h"
#include "stm32f10x_tim.h"
#include <stdio.h> 
extern u16 all_data[16600];	 //
extern u8  p_all_data[];		 //
extern void x_filter(void);  //	   ����X�ź��˲�1MSһ��
extern void force_reset(u16 start_addr,u8 process_addr);  //����ǿ�����Ԫʹ��
extern void force_set(u16 start_addr,u8 process_addr);	  //
extern void filter(void);	//���˼�  20151122	adc
extern void DAC_data(void);	//���˼�  20151211	dac
u16 temp[5];
u16 *p_data_given,*p_reset_coil,*p_over_coil,*p_enable_coil,*p_value;

void timer_enable(u16 timer_number)
{     
	p_data_given=all_data+0x1000+timer_number;		  //������ֵ��ַ
	p_value=all_data+0x0800+timer_number;			  //
	p_over_coil=all_data+0x0100+(timer_number/0x10);	  //�����Ȧ
	if(*p_value<*p_data_given)
	*p_over_coil&=~(1<<timer_number%0x10); //ֵС���趨ֵʱOFF
	else
	*p_over_coil|=(1<<timer_number%0x10);   //ֵ�����趨ֵʱON
}

void timer_disble(u16 timer_number)
{     
	p_data_given=all_data+0x1000+timer_number;		  //������ֵ��ַ
	p_value=all_data+0x0800+timer_number;			  //
	p_reset_coil=all_data+0x0380+(timer_number/0x10);  //��λ��Ȧ
	p_over_coil=all_data+0x0100+(timer_number/0x10);	  //�����Ȧ
	p_enable_coil=all_data+0x0280+(timer_number/0x10); //ʹ����Ȧ
	*p_reset_coil&=~(1<<timer_number%0x10);  //��λ��Ȧ
	*p_over_coil&=~(1<<timer_number%0x10);	  //�����Ȧ
	*p_enable_coil&=~(1<<timer_number%0x10); //ʹ����Ȧ
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
		if((*p_enable_coil&(1<<timer_count%0x10))==(1<<timer_count%0x10))  //��Ȧ״̬
		{
			if(*p_value<*p_data_given)						 //ֵ״̬
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
		if((*p_enable_coil&(1<<timer_count%0x10))==(1<<timer_count%0x10))  //��Ȧ״̬
		{
			if(*p_value<*p_data_given)						 //ֵ״̬
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
		if((*p_enable_coil&(1<<timer_count%0x10))==(1<<timer_count%0x10))  //��Ȧ״̬
		{
			if(*p_value<*p_data_given)						 //ֵ״̬
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
		if((*p_enable_coil&(1<<timer_count%0x10))==(1<<timer_count%0x10))  //��Ȧ״̬
		{
			if(*p_value<*p_data_given)						 //ֵ״̬
			*p_value+=1;
		}
	}
}

void TIM5_IRQHandler(void)//1ms����һ���ж��ź�
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

		if((all_clock%10)==7)  //10ms��ʱ�����ÿ�����ˢ��һ��
		{
			T_10MS();
			force_reset(0X00E0,11);
		}
		if((all_clock%10)==2)
		{
		   force_set(0X00E0,11);   
		}

		if(all_clock==50)	//����100MS��ʱ���ֿ�ˢ��
		T_100MS(),force_reset(0X00E0,12);
			
		if(all_clock==90)	 //ÿ100ms���ӷ��Ӷ�ʱ��
		T_H100MS(),minute++;
		
	    if((all_clock%0x10)==0x02)	    //����һ��DAC����
		{
		    DAC_data();	   //ADC  10msˢ��һ�� //����һ��DAC����
			filter(); 
		}
		
		if(minute%10==5)	  // ˢ������8013
		force_reset(0X00E0,13);
		if(minute%10==0)
		force_set(0X00E0,13);

		if(minute==300)			//ˢ�·���8014
		force_reset(0X00E0,14);
		if(minute==0)
		force_set(0X00E0,14);

		if(minute>599)
		minute=0;		  
		x_filter();       //���X����״ֵ̬	
		test++;
	}
}

void TIM5_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	//��������Զ�װ�صļ���ֵ�����ڼ����Ǵ�0��ʼ�ģ�����10000�κ�Ϊ9999
	TIM_TimeBaseStructure.TIM_Period = (100 - 1);
	// �������Ԥ��Ƶϵ����������Ϊ0ʱ��ʾ����Ƶ����Ҫ��1
	TIM_TimeBaseStructure.TIM_Prescaler = (720 - 1);
	// �߼�Ӧ�ñ��β��漰�������ڶ�ʱ��ʱ��(CK_INT)Ƶ���������˲���(ETR,TIx)
	// ʹ�õĲ���Ƶ��֮��ķ�Ƶ����
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//��ʼ����ʱ��5
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	/* Clear TIM5 update pending flag[���TIM5����жϱ�־] */
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	/* TIM IT enable */ //������ж�
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM5, ENABLE);  //������ʹ�ܣ���ʼ����
}

