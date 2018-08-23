/********************************************************/
// CPU��Ҫ��STM32F103--RAM�ڴ治С��48K	Flash�ڴ治С��256K
// ����������STM32F103RCT6 RDT6 VCT6 VET6����ͨ��
// �༭���ڣ�20160320
// editor by ���˼�
/********************************************************/
// 20150905 :ADD command: INC  INCP  DEC  DECP
// 20150914 :  MPP  LD<>  AND<>  OR=  OR>  OR<  OR>=  OR<=  OR<>
// 20150917 :  WAND WOR WXOR NEG ROL ROR RCL RCR SQR SWAP
// 20151009 :  ADD RUN/STOP KEY
// 20151214 :  ADD RST_T_D_M_C()
// 20160320 :  ADD ADC&DAC 	 AD(D8030 D8031)  DA(D7030 D7031)
// 20151221 :  ���Ķϵ籣��Ĵ�����Χ��D500-D950  C100-C150 T100-T150 M512-M1024
// 20151228 :  T0-T199��100ms  
//             T200-T245 :10ms
//             T246-T249 :1ms 
//             T250-T255 :100ms 
// 20160219 :  UPDATE AD(D6030 D6031) 
/********************************************************/

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include <OS_Config.h>
//#include "abs_addr.h"
#include <i2c.h>
#include <stdio.h>
//#include <absacc.h> 


#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"	

u32 startup  __attribute__((at(0x2000D5F0)));
u8 power_down;
u8 Run_Flag=1;
extern void data_init(void);
extern void PLC_ProComParse(void);
extern void init_xy(void);
extern void  usart_init(u16 baud);
extern void TIM5_Init(void);
extern void y_refresh(void);
extern void x_refresh(void);
extern void NVIC_Configuration(void);
extern void Process_switch(void);
extern void TX_Process(void);
extern void Stm32_Clock_Init(u8 PLL);
extern void backup_data(void);
extern void recover_data(void);
extern void off_out(void);
extern void ADC_init(void);	  //ADD
extern u16 all_data[16600] __attribute__((at(0x20005002)));
extern u8 rx_end;

#define BmpHeadSize (54)
 /* __GNUC__ */
#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

static void Delay(__IO uint32_t nCount)
{ 
	while(nCount--);
}

void send_test(void)
{
	u8 CSRH = RCC->CSR>>8;
	u8 CSRL = RCC->CSR&0xff;
	USART_SendData(USART1,CSRH); 
  Delay(10000);
	USART_SendData(USART1,CSRL); 
  Delay(10000);
	USART_SendData(USART1,0x36); 
  Delay(10000);
	USART_SendData(USART1,0x37); 
  Delay(10000);
}

int main(void)
{  
  u16 LED=0;
  u16 Timer[2];
  Delay(10000);
  init_xy();		                // PLC ���������ʼ��
  Stm32_Clock_Init(9);
  startup=0X55AA55AA;
  init_xy();		                // PLC ���������ʼ��
  Delay(6);
  //USART_DeInit(USART1);	        // ���ڳ�ʼ��
  Delay(6);
//  usart_init(9600);            // ���ڳ�ʼ��
  ADC_init();
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
  TIM5_Init();	                // TIMER5 ��ʱ����ʼ��
  NVIC_Configuration();	        // �жϳ���ȫ���ȼ�����
  power_down=5;
	PBout(5) = 0;
	PBout(5) = 1;
	USB_Port_Set(0); 	//USB�ȶϿ�
	Delay(10000);
	USB_Port_Set(1);	//USB�ٴ�����
 	Set_USBClock();   
 	USB_Interrupts_Config();    
 	USB_Init();	
	data_init(); 
  while (1)
  {	
		y_refresh(); 
		x_refresh(); 

    // PLCָ�����
		PLC_ProComParse();

		switch(rx_end)	            // ����FX2N ����������͵�����
		{
			case 1  :
      {
				  rx_end=0;
				  Process_switch(); //��������
				  TX_Process();         // ���ʹ�������
				  LED=800; 
				  break;	              // ������������ĳ���
      }
			case 5  :
      {
				  rx_end=0;
				  TX_Process();
				  LED=800;  
				  break;	              // ����һ�η���ָ��
      }
			default :   
      {
			    if(LED>=1)
            LED--;                                 
			    break;	
      }   
		}

/********************************************************/
// CPU��Ҫ��STM32F103--RAM�ڴ治С��48K	Flash�ڴ治С��256K
// ����������STM32F103RCT6 RDT6 VCT6 VET6����ͨ��
// �༭���ڣ�20160320
// editor by ���˼�
/********************************************************/

		//*	�͵�ѹ���  �ϵ籣������
 		if(PVD)	   //MY PCB== !PVD
 		{	
 			if(Timer[0]==0)
				recover_data();
 			if(Timer[0]<=60000)
 			Timer[0]++;
 		}
 		else
 		{
 			all_data[0x180/2]=0;
 			if(Timer[0]>=100)
 			  backup_data();

 			Timer[0]=0;
 		}
		
		//*/
	}
}


PUTCHAR_PROTOTYPE
{
	USART_SendData(USART2, (uint8_t) ch);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	return ch;
}
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
 {  
    while (1); 
 }
#endif
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
