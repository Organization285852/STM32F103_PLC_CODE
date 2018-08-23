/********************************************************/
// CPU��Ҫ��STM32F103--RAM�ڴ治С��64K	Flash�ڴ治С��128K
// ����������STM32F103RCT6 RDT6 VCT6 VET6����ͨ��
// �༭���ڣ�20150903
// editor by ���˼�
/********************************************************/

#include "stm32f10x.h"
#include "stm32f10x_flash.h"
//#include "abs_addr.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include <OS_Config.h>
#include <stdio.h>
//#include <absacc.h>
#include <io.h>

#define flash_start_address 0x8006000			 

const signed short int  x[0x770C] __attribute__((at( flash_start_address+2)))= // PLC��������� flash_start_address+0x5c��ʼ��ע���С������
{
0XBAD8,0X0000,0X0000,0X2020,0X2020,0X2020,0X2020,0X2020,0X2020,0X2020,     //0-0x5C byte ΪPLC������Ϣ
0X2020,0X2020,0X2020,0X2020,0X2020,0X2020,0X2020,0X2020,0X2020,0X2020,
0X2020,0X2020,0X2020,0X09F4,0X0BFF,0X01F4,0X03E7,0X0E64,0X0EC7,0X0EDC,
0X0EFF,0X0190,0X03FE,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,
0x2400,0xC500,0X2401,0XC501,0X2402,0XC502,0X2403,0XC503,0X000F,//��ʼ���Գ���
0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF};
 
const u8   p_x[2] __attribute__((at(flash_start_address)))={0x08,0x00};	
const u16 special_d[256]={
0X00C8,0X5EF6,0X0002,0X0010,0X0000,0X0025,0X001E,0X0000,0XFFFF,0X0000,
0X0000,0X0000,0X0000,0X00A5,0X00A5,0X00A5,0X00A5,0X00A5,0X0811,0X000F,
0X000A,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X003D,0X001C,0X0000,0X0000,0X0014,0X00FF,0X03D7,0X0000,0X0000,0X0000,
0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0X0000,0XFFFF,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X183B,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X01F4,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0064,0X5EF6,0X0008,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0DDC,0X3DB6,0X0000,0X0000,
0X0000,0X0000,0X0000,0X0000,0X0002,0X0003,0X0000,0X0000
    };
static const char bcd_ascll[20]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,
                                 0x37,0x38,0x39,0X41,0X42,0X43,0X44,0X45,0X46};

const char ascll_bcd[25]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0X0B,0X0C,0X0D,0X0E,0X0F};

const char plc_type[2]={0XF6,0X5E};	  	  //PLC�ͺ�  FX2N

u16 prog_write_buffer[2050];

u16 prog_count,rx_temp,tx_temp,rx_count,tx_count;

u16  all_data[16600] __attribute__((at(0x20005004)));	
u8   p_all_data[4] __attribute__((at(0x20005000)));  
u8  step_status[1000]  __attribute__((at(0x2000D200)));

char *str;
u16 *px;
char tx_data[610],rx_data[610];
u16 prog_address,data_address; 
u8  data_size,block_contol[2],rx_end;
extern u8  edit_prog;
extern u8 Run_Flag;
void recover_data(void); 

void data_init(void) 
{ 
	u16 temp;
	u16 temp_address;
	for(temp=0;temp<126;temp++)
	{
		temp_address=0x0700; 		 
		all_data[temp_address+temp]=special_d[temp];  
	}
	temp_address=0x01C0+4;		    
	p_all_data[temp_address]=0x09;    
	block_contol[0]=200;			    
	block_contol[1]=200;
	recover_data();
}

//void usart_init(u16 baud)               
//{  
//	float temp;
//	u16 mantissa;
//	u16 fraction;	
//	
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	temp=(float)(72*1000000)/(baud*16);
//	mantissa=temp;				 
//	fraction=(temp-mantissa)*16; 
//	mantissa<<=4;
//	mantissa+=fraction; 
//	RCC->APB2ENR|=1<<2;   
//	RCC->APB2ENR|=1<<14;  
//	GPIOA->CRH&=0XFFFFF00F; 
//	GPIOA->CRH|=0X000008B0;
//	  
//	RCC->APB2RSTR|=1<<14;   
//	RCC->APB2RSTR&=~(1<<14);
//	
//	USART1->BRR=mantissa; 
//	USART1->CR1|=0X240C;  
//	#ifdef EN_USART1_RX		  
//	
//	USART1->CR1|=1<<8;    
//	USART1->CR1|=1<<5;    
//	MY_NVIC_Init(3,3,USART1_IRQChannel,2);
//	#endif	 
//	data_init();		   
//}

//---------------------���ڹ�������---------------------
void  usart_init(u16 baud) 
{
//	DMA_InitTypeDef  DMA_InitStructure;                           //DMA��ʼ���ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;                          //�����������üĴ���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);       //�򿪴��ڶ�Ӧ������ʱ��  
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);            //����DMAʱ��
//	DMA_DeInit(DMA1_Channel4);                                    //DMA1ͨ��4����
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);//�����ַ
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)tx_data+1;        //�ڴ��ַ
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;            //dma���䷽����
//	DMA_InitStructure.DMA_BufferSize = 143;                       //����DMA�ڴ���ʱ�������ĳ���
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����DMA���������ģʽ��һ������
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;       //����DMA���ڴ����ģʽ
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//���������ֳ�
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;//�ڴ������ֳ�
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                 //����DMA�Ĵ���ģʽ
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;           //����DMA�����ȼ���
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                  //����DMA��2��memory�еı����������
//	DMA_Init(DMA1_Channel4,&DMA_InitStructure);
//	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
	usart(baud);                                //��ʼ������ 
	//TXE�����ж�,TC��������ж�,RXNE�����ж�,PE��ż�����ж�,�����Ƕ��   
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  
//	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//����DMA��ʽ����
	USART_Cmd(USART1, ENABLE);                  //��������    
	
	//*********************����1�Ĺܽų�ʼ�� ****************************************   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                       // �ܽ�9  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;               // ѡ��GPIO��Ӧ�ٶ�  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 // �����������  
	GPIO_Init(GPIOA, &GPIO_InitStructure);                          // TX��ʼ��  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                      // �ܽ�10  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;           // ��������  
	GPIO_Init(GPIOA, &GPIO_InitStructure);                          // RX��ʼ��    
	
	data_init();                                                  
}

void usart(u16 DEFAULT_BAUD)
{
	USART_InitTypeDef USART_InitStructure;                          // �������üĴ���	
	USART_InitStructure.USART_BaudRate = DEFAULT_BAUD;              // ���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     // 8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          // һλֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;           // żУ��λ 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1,&USART_InitStructure);                        // ��ʼ������   
}  


void write_block(u16 number) //ÿһ��2K
{
	u16 temp,wait_write,appoint_address;
	if(number<12)			  
	{
		FLASH_Unlock();		  
		FLASH_ErasePage(flash_start_address+number*0x800);         
		for(temp=0;temp<1024;temp++)	  
		{
			appoint_address=flash_start_address+number*0x800+temp*2;     
			wait_write=prog_write_buffer[temp*2]+prog_write_buffer[temp*2+1]*0X100; 
			FLASH_ProgramHalfWord(appoint_address,wait_write);  
		}
		FLASH_Lock();	   
	}
}
//����֮ǰ�������
void backup_block(u16 number)		 
{
	u16 temp,appoint_address;
	if(number<12)
	{
		for(temp=0;temp<2048;temp++)
		{
			appoint_address=number*0x800+temp;
			prog_write_buffer[temp]=p_x[appoint_address];	
		}
	}
}

void rx_data_sum(void)	     
{
	u16 temp; 
	u8 all_sum;
	rx_data[599]=0;     
	all_sum=0;		  
	str = rx_data;	  
	str +=3;			  
	for(temp=3;temp<(rx_count-1);temp++)   
	{
		all_sum+=*str;						 
		str++;								 
	}
	if((rx_data[rx_count-1] == bcd_ascll[all_sum/0x10])&&(rx_data[rx_count] == bcd_ascll[all_sum%0x10])) 
	rx_data[599]=1;				
	else
	rx_data[599]=5;				
}

//ת��ASCII��ΪHEX��,ת���õ������ٴ�tx_data����С������2��ʼ��ֵ
void switch_read_data(void)  
{ 
	u16 i;
	for(i=4;i<(rx_count-2);i++)// �ӵ�ַ(4)��ʼת����ÿ����ASCII��ת��һ��HEX��     
	{ 
		tx_data[i/2]=ascll_bcd[rx_data[i]-0x30]*0x10; 
		tx_data[i/2]+=ascll_bcd[rx_data[i+1]-0x30];      
		i++;
	}
}

void setup_HL(void)	    
{
	u8 temp;
	temp=tx_data[3];				  
	prog_address=temp*0x100+tx_data[2];
}	
	    
void setup_LH(void)	    
{ 
	u8 temp;
	temp=tx_data[3];				  
	data_address=temp*0x100+tx_data[4];
}

void read_plc_tyte(void)  
{
	u16 temp;
	u8 temp_sum; 
	tx_data[1]=0x02;          
	temp_sum=0;
	for(temp=0;temp<data_size;temp++)
	{ 
		tx_data[temp*2+2]=bcd_ascll[plc_type[temp]/0x10]; 
		tx_data[temp*2+3]=bcd_ascll[plc_type[temp]%0x10]; 
		temp_sum+=tx_data[temp*2+2]+tx_data[temp*2+3];
	}
	tx_data[temp*2+2]=0x03;    
	temp_sum+=0x03;
	tx_data[temp*2+3]=bcd_ascll[temp_sum/0x10];
	tx_data[temp*2+4]=bcd_ascll[temp_sum%0x10]; 
	tx_count=temp*2+4;
}

void read_other_data(void)	 
{
	u16 temp;
	u8 temp_sum;
	tx_data[1]=0x02;          
	temp_sum=0;
	for(temp=0;temp<data_size;temp++)
	{ 
		tx_data[temp*2+2]=bcd_ascll[p_all_data[temp+prog_address+4]/0x10]; 
		tx_data[temp*2+3]=bcd_ascll[p_all_data[temp+prog_address+4]%0x10]; 
		temp_sum+=tx_data[temp*2+2]+tx_data[temp*2+3];
	}
	tx_data[temp*2+2]=0x03;    
	temp_sum+=0x03;
	tx_data[temp*2+3]=bcd_ascll[temp_sum/0x10];
	tx_data[temp*2+4]=bcd_ascll[temp_sum%0x10]; 
	tx_count=temp*2+4;
}

void PC_READ_byte(void)	       
{
	prog_address=tx_data[2]*0x100+tx_data[3];
	switch(prog_address)
	{ 
		case 0x0ECA: read_plc_tyte();  break;  
		case 0x0E02: read_plc_tyte();  break;  
		default: read_other_data();  break;
	}
}

void PC_WRITE_byte(void)       
{ 
	u16 temp;
	prog_address=tx_data[2]*0x100+tx_data[3]+4;
	for(temp=0;temp<data_size;temp++)
	{
		p_all_data[temp+prog_address]=tx_data[5+temp];
	}
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}

void PC_FORCE_ON(void)	      
{ 
	u8 *p_bit;
	p_bit=p_all_data;
	prog_address=(tx_data[2]*0x100+tx_data[3]);
	p_bit+=prog_address/0x08+4;
	*p_bit|=1<<(prog_address%0x08);
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}

void PC_FORCE_OFF(void)	      
{ 
	u8 *p_bit;
	p_bit=p_all_data;
	prog_address=(tx_data[2]*0x100+tx_data[3]);
	p_bit+=prog_address/0x08+4;
	*p_bit&=!(1<<(prog_address%0x08));
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}

void EPC_FORCE_ON(void)	      
{ 
	u8 *p_bit;
	p_bit=p_all_data;
	p_bit+=prog_address/0x08+4;
	switch(prog_address)
	{
		case 0x0E23:                                                  ;  break;  
		case 0x0E24: prog_address=0X01C4,p_all_data[prog_address]=0x09;  break;  
		case 0x0E25: prog_address=0X01C4,p_all_data[prog_address]=0x0A;  break;  
		default:     *p_bit|=1<<(prog_address%0x08);                     break;  
	}
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}


void EPC_FORCE_OFF(void)	  
{ 
	u8 *p_bit;
	p_bit=p_all_data;
	p_bit+=prog_address/0x08+4;
	*p_bit&=~(1<<(prog_address%0x08));
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}


void PC_READ_Parameter(void)  
{ 
	u16 temp,temp_bit,temp_addr,mov_bit;
	u8 temp_sum;
	u8 send,monitor,monitor1,monitor2; 
	tx_data[1]=0x02;              
	temp_sum=0;
	prog_address=(tx_data[3]*0x100+tx_data[4]);
	if((prog_address==0x1790)||(prog_address==0x17D0))
	{
		if(prog_address==0x1790)
		{
			monitor1=all_data[0x1400/2]%0X100;	
			for(monitor=0;monitor<monitor1;monitor++)
			{
				temp_bit=all_data[0x1400/2+2+monitor]/2;
				all_data[0x1790/2+monitor]=all_data[temp_bit]; 
			}
			monitor2=all_data[0x1400/2+1]%0X100;	
			for(monitor1=0;monitor1<monitor2;monitor1++)
			{
				temp_addr=all_data[0x1400/2+2+monitor+monitor1];
				temp_bit=all_data[temp_addr/0x10];
				mov_bit = temp_addr%0x10;
				if((temp_bit&(1<<mov_bit))==(1<<mov_bit))
				all_data[0x1790/2+monitor+monitor1/0x10]|=1<<(monitor1%0x10);
				else
				all_data[0x1790/2+monitor+monitor1/0x10]&=~(1<<(monitor1%0x10)); 
			}
		}
	}
	if(prog_address>0x7fff)	//����������FLASH
	{						
		prog_address-=0x8000;	
		for(temp=0;temp<data_size;temp++)
		{ 
			tx_data[temp*2+2]=bcd_ascll[p_x[prog_address+temp]/0x10]; 
			tx_data[temp*2+3]=bcd_ascll[p_x[prog_address+temp]%0x10]; 
			temp_sum+=tx_data[temp*2+2]+tx_data[temp*2+3];
		}
	}
	else//������SRAM
	{
		prog_address+=0x04;	
		for(temp=0;temp<data_size;temp++)	  
		{ 
			send=p_all_data[prog_address+temp];
			tx_data[temp*2+2]=bcd_ascll[send/0x10]; 
			tx_data[temp*2+3]=bcd_ascll[send%0x10]; 
			temp_sum+=tx_data[temp*2+2]+tx_data[temp*2+3];
		}
	}
	tx_data[temp*2+2]=0x03;    
	temp_sum+=0x03;
	tx_data[temp*2+3]=bcd_ascll[temp_sum/0x10];
	tx_data[temp*2+4]=bcd_ascll[temp_sum%0x10]; 
	tx_count=temp*2+4;
}


void PC_WRITE_Parameter(void) 
{
	u16 temp;
	prog_address=(tx_data[3]*0x100+tx_data[4]);
	if(prog_address>0x7fff)
	{ 
		edit_prog=0;                       
		prog_address-=0x8000;
		for(temp=0;temp<data_size;temp++)
		{
			block_contol[0]=(prog_address+temp)/0x800;	
			if(block_contol[0]==block_contol[1])			 
			{
				prog_write_buffer[(prog_address+temp)-block_contol[0]*0x800]=tx_data[6+temp];  
			}
			else							  
			{
				write_block(block_contol[1]);   
				backup_block(block_contol[0]);  
				block_contol[1]=block_contol[0];
				prog_write_buffer[(prog_address+temp)-block_contol[0]*0x800]=tx_data[6+temp];
			}
		}
	}
	else
	{
		prog_address+=0x04;
		for(temp=0;temp<data_size;temp++)	  
		{ 
			p_all_data[prog_address+temp]=tx_data[6+temp];;
		}
	}  
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}


void PC_READ_PORG(void)	         
{
	u16 temp;
	u8 temp_sum; 
	tx_data[1]=0x02;               
	temp_sum=0;
	if(data_address>0x7fff)		 
	{							 
		data_address-=0x8000;	     
		for(temp=0;temp<data_size;temp++)
		{ 
			tx_data[temp*2+2]=bcd_ascll[p_x[data_address+temp]/0x10]; 
			tx_data[temp*2+3]=bcd_ascll[p_x[data_address+temp]%0x10]; 
			temp_sum+=tx_data[temp*2+2]+tx_data[temp*2+3];
		}
	}
	else
	{
		data_address+=0x4;	
		for(temp=0;temp<data_size;temp++)	  
		{
			tx_data[temp*2+2]=bcd_ascll[p_all_data[data_address+temp]/0x10]; 
			tx_data[temp*2+3]=bcd_ascll[p_all_data[data_address+temp]%0x10]; 
			temp_sum+=tx_data[temp*2+2]+tx_data[temp*2+3];
		}
	}
	tx_data[temp*2+2]=0x03;    
	temp_sum+=0x03;
	tx_data[temp*2+3]=bcd_ascll[temp_sum/0x10];
	tx_data[temp*2+4]=bcd_ascll[temp_sum%0x10]; 
	tx_count=temp*2+4;
}

void PC_WRITE_PORG(void)	 
{ 
	u16 temp;
	prog_address=(tx_data[3]*0x100+tx_data[4]);
	if(prog_address>0x7fff)
	{ 
		edit_prog=0;                       
		prog_address-=0x8000;
		for(temp=0;temp<data_size;temp++)
		{
			block_contol[0]=(prog_address+temp)/0x800;	
			if(block_contol[0]==block_contol[1])			 
			{
				prog_write_buffer[(prog_address+temp)-block_contol[0]*0x800]=tx_data[6+temp];  
			}
			else							  //д�Ĳ���ͬһ�飬
			{
				write_block(block_contol[1]);   
				backup_block(block_contol[0]);  //
				block_contol[1]=block_contol[0];
				prog_write_buffer[(prog_address+temp)-block_contol[0]*0x800]=tx_data[6+temp];
			}
		}
	}
	else
	{
		prog_address+=0x04;
		for(temp=0;temp<data_size;temp++)	  
		{ 
			p_all_data[prog_address+temp]=tx_data[6+temp];;
		}
	}  
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}

// ���˼�,20160305�Ż�
//���Ҿ������ݵĶ�Ӧ��ַ
u16 find_data(u16 addr,u16 findData) 
{
	u8 data_H,data_L;
	u8 find_ok = 5;         

	data_H =findData/0x100;
	data_L =findData%0x100;

	if(addr>0x7fff)               // ���ܴ���31K����Ҫ����ʵ��������е���
	{ 
		addr -= 0x8000;
		do
    {
			if( (p_x[addr] == data_L) && (p_x[addr+1] == data_H) ) 
      {
			  find_ok=0;// �ҵ���Ҫ��ָ��
      }
			else
      {
			  addr +=2;
      }
			if( addr > (0xdedb-0x8000))// 23K PLC������Ҫ����ʵ��������е���
      {
			  find_ok = 1;// ����Ч�ķ�Χ��û���ҵ�ENDָ��                                
      }     
		}while(find_ok > 3);
	}
	addr+=0X8000;
	return addr;
}


// ���˼�,20160305�Ż�
// ������λ����Ҫ��ָ���ַ
void find_data_address(void)  
{
	u8 temp_sum,data_H,data_L;  
	if(data_address > 0x7fff)		
	{
		data_L=tx_data[5];	        
		data_H=tx_data[6]; 
         
		data_address=find_data(data_address,data_H*0X100+data_L);	
		tx_data[1]=0x02;              
		temp_sum=0;
		tx_data[2]=0x31;
		temp_sum+=tx_data[2];
		data_H=data_address/0x100;
		data_L=data_address%0x100;

		tx_data[3]=bcd_ascll[data_H/0X10];
		tx_data[4]=bcd_ascll[data_H%0X10];
		tx_data[5]=bcd_ascll[data_L/0X10];
		tx_data[6]=bcd_ascll[data_L%0X10];
		tx_data[7]=0X03;

		temp_sum+=tx_data[3];
		temp_sum+=tx_data[4];
		temp_sum+=tx_data[5];
		temp_sum+=tx_data[6];
		temp_sum+=tx_data[7];
		tx_data[8]=bcd_ascll[temp_sum/0x10];
		tx_data[9]=bcd_ascll[temp_sum%0x10]; 
		tx_count=9;
	}
	else
	{
	}

}

void backup_mov_block(u16 number)
{
	u16 temp,appoint_address;
	if(number<10)
	{
		for(temp=0;temp<2048;temp++)
		{
			appoint_address=number*0x800+temp;
			prog_write_buffer[temp]=p_x[appoint_address];
		}
	}
}


// flash�����ݴ�ָ���Ŀ�ʼ��ַstartAddr�ƶ�num���ֽڵ�����
void mov_flash(u16 startAddr,u8 num) 
{
	u16 newStartAddr,end_addr,backup_addr,temp,temp1,temp2,mov_byte,addr_mov;
 
	static u8 offset;

	offset=num;

  // �ҳ�END(0x000f)ָ������λ�� + �ƶ�����num = ���������END����λ��
	end_addr=find_data(startAddr + 0x8000,0x000f) + num - 0x8000; 

	newStartAddr = end_addr;   
	addr_mov   = startAddr;    // ָ����λ�ã��Ѽ���0x8000

	if(startAddr>0x5B) // ����91
	{ 
		addr_mov -=0X5C; // ��ȥPLC�������ԡ�0-0x5C��
		end_addr -=0x5C;

		addr_mov/=2;		 // һ��ռ�������ֽ�(����X0:00 24),��2Ϊָ����ʼ�ֽ�
		end_addr/=2;

		addr_mov/=8;		 // ָ����ÿһ����״̬��ַ �� 
		end_addr/=8;

		offset/=2;
		mov_byte=offset/8;
		offset%=8;

		while(!(end_addr==addr_mov))//END addr == curennt addr �� ����������ѭ��	   
		{
			temp   = step_status[end_addr] * 0x100 + step_status[end_addr - 1];// ����ǰ16λ��״ֵ̬��Ϊ������Ҫ�ƶ�
			temp <<= offset;                                                   // ����ƶ�ƫ�Ƶĵ�ַ����
			step_status[end_addr+mov_byte] = temp/0x100;		                   // ��ֵ��Ŀ���ַ
			end_addr --;							                                         // ��ַ��ǰ��8��
		}
		temp   = step_status[end_addr]  *0x100 + step_status[end_addr - 1];	 
		temp <<= offset;   
		step_status[end_addr + mov_byte] = temp/0x100;		  
	}
	end_addr = newStartAddr;   
	temp = newStartAddr;

 do
 {
		if((end_addr/0x800)==(startAddr/0x800))  
    {
		  newStartAddr=startAddr%0x800;		
    }	   
		else
    {
		  newStartAddr=0;	
    }		
		   
		if((temp/0x800)==(end_addr/0x800))
    {  
		  temp1=end_addr%0x800+1; 
    }
		else
    {
		  temp1=2048;		
    }		
    
    // ���� 
		backup_block(end_addr/0x800);                           // PLC����ͼ���򱸷�,Ŀ����д����֮ǰ��ǰ����򱸷�

		for(temp2 = newStartAddr;temp2 < temp1 + 1;temp2 ++)
		{
			backup_addr = (end_addr/0x800)*0x800+temp2-num;   

      // ����PLC����ͼ���򵽻���
			prog_write_buffer[temp2] = p_x[backup_addr];          // ����PLC����ͼ����				
		}
    
    // дFlash
		write_block(end_addr/0x800);                            // ���ƶ���������д��FLASH
		end_addr -= (temp1 - newStartAddr);   

	}while(end_addr > startAddr + num);   
}

// ���˼�,20160305�Ż������ӣ�д��ģʽ�������߱��
void online_write_data(void) 
{ 
	u16 temp;
	u8 temp1,temp2;
	temp1=tx_data[5];                     // ����ͼ�б�Ų�����ֽ���
	temp2=tx_data[6];                     // ����ͼ�б��Ķ����ֽ���
    
	temp2-=temp1;                         // ��������ͼ��ʵ���������޸ĵ��ֽ���
 
	if(temp2>0)                           // ������ͼ��ʵ���������޸ĵ��ֽ���                  
	{ 
		mov_flash(data_address-0x8000,temp2);// ��ָ����ʼ��ַ��Ų������ͼ��ʵ���������޸ĵ��ֽ���        
	}

  // �ѳ���༭���,��Ϊд����ʱ���ܴ���P��ַ�����仯��PLCӦ����P��ַ 
	edit_prog=0;                       
	block_contol[0]=100;
	block_contol[1]=100;
	prog_address=(tx_data[3]*0x100+tx_data[4])-0x8000;
	data_size=tx_data[6];

	for(temp=0;temp<data_size;temp++)
	{
		block_contol[0]=(prog_address+temp)/0x800;	
		if(block_contol[0]==block_contol[1])			 
		{
			prog_write_buffer[(prog_address+temp)-block_contol[0]*0x800]=tx_data[7+temp];  
		}
		else							  
		{
			write_block(block_contol[1]);   
			backup_block(block_contol[0]);  
			block_contol[1]=block_contol[0];
			prog_write_buffer[(prog_address+temp)-block_contol[0]*0x800]=tx_data[7+temp];
		}
	} 
	write_block(block_contol[0]);   
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}

void all_flash_unlock(void)  
{
	block_contol[1]=200;
	block_contol[0]=200;
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}

void all_flash_lock(void) 	 
{
	write_block(block_contol[1]);   
	block_contol[1]=200;
	block_contol[0]=200;
	FLASH_Lock();
	tx_data[1]=0x06,tx_count=1,rx_end=5;
}


// ���˼�,20160305�Ż�
// ��չ����"E"����
void PC_OPTION_PROG(void)    
{ 	
	u16 temp;
	if((rx_count==11)&&((rx_data[4]==0x37)||(rx_data[4]==0x38)))
	{
		prog_address=ascll_bcd[rx_data[5]-0x30]*0x10+ascll_bcd[rx_data[6]-0x30]+ascll_bcd[rx_data[7]-0x30]*0x1000+ascll_bcd[rx_data[8]-0x30]*0x100;
		if(rx_data[4]==0x37) 
		  EPC_FORCE_ON();  
		else
		  EPC_FORCE_OFF();
	}
	else
	{
		setup_LH();			 
		temp=tx_data[2];
		switch(temp) 
		{ 
			case 0x00: PC_READ_Parameter();               break;  // ������ E00
			case 0x10: PC_WRITE_Parameter();              break;  // д���� E10
			case 0x01: PC_READ_PORG();                    break;  // ������ E01
			case 0x11: PC_WRITE_PORG(),edit_prog=0; ;     break;  // д���� E11 
			case 0x77: all_flash_unlock();                break;  
			case 0x87: all_flash_lock();                  break;  
			case 0x41: find_data_address();               break;  // ����ENDָ���ַ	
			case 0xD1: 																						// ���ӣ�д��ģʽ��
      {
        online_write_data();                        break;
      }
			default: tx_data[1]=0x15,tx_count=1,rx_end=5; break;	// ��ʶ���ָ��
		}
	}
}

void find_end(void)		   
{
	if(rx_count==13)
		tx_data[1]=0x06,tx_count=1,rx_end=5;
	else
		tx_data[1]=0x06,tx_count=1,rx_end=5;
}

void Process_switch(void)
{ 
	u8 temp;

	switch_read_data();                                      // �ѵ���λ��ʼ��ASCII��ת����HEX	����ַΪ���ݷ�����                       
               
	temp=rx_data[3];
	switch(temp) 
	{ 
		case 0x30: data_size=tx_data[4],PC_READ_byte();  break;// ������
		case 0x31: data_size=tx_data[4],PC_WRITE_byte(); break;// д���� 
		case 0x34: find_end();                           break;// ����ָ�����ҵ��������򷵻�6 
		case 0x37: setup_HL(),PC_FORCE_ON();             break;// PLC	���� Զ�� ��0x37��
		case 0x38: setup_HL(),PC_FORCE_OFF();            break;// PLC ֹͣ Զ�� ��0x38��
		case 0x42: all_flash_lock();                     break;// д������������
		case 0x45:                                             // ͨѶE����ָ��
    {
      data_size=tx_data[5];                                // ����ͼ�б��Ķ����ֽڸ���
      PC_OPTION_PROG();
      break;
    }
		default:	                                       break;
	} 
	if((tx_count==0)&&(rx_count==0))	 
		tx_data[1]=0x15,tx_count=1,rx_end=5;			 		 
}								 
void send_test(void);
extern void USB_USART_SendData(u8 data);
void  TX_Process(void)	
{
	u16 temp=0;
	//ʹ��USART
//	if((tx_count>0)&&(rx_end==0x00))  
//	{
//		USART_SendData(USART1,(0x80|(tx_data[1]))); 
//		USART_ITConfig(USART1,USART_IT_TC,ENABLE); 
//		for(temp=0;temp<tx_count;temp++)
//		{ 
//			tx_data[temp]=tx_data[temp+1];
//		}
//		tx_count--;								 
//	}
//	else
//		USART_ITConfig(USART1,USART_IT_TC,DISABLE); 
		
	//ʹ��USB
	if(rx_end==0x00)
	{
		while(tx_count)
		{
			USB_USART_SendData(tx_data[temp+1]);
			tx_count--;		
			temp++;
		
		}	
	}
}

			  		
void RX_Process(char res)
{ 
  static u8 f=1;	  //ADD ���˼�
	rx_data[0]=0x7f&res;

	if(rx_data[0]==0X05)	
  {
		rx_count=0,tx_data[1]=0x06,tx_count=1,rx_end=5;  
	}
	else if(rx_data[0]==0X02)		
	{
		rx_count=0x01;
	}
//  else if(rx_count==0)		 //ADD
//	{
//		if(f==1)
//		{
//			usart(19200*0.75);     //19200          
//			rx_count=0,tx_data[1]=0x06,tx_count=1,rx_end=1;//������λ�����ҷ���0X06Ӧ��	 
//			f=0;
////			Err = 0;
//		}
//		else if(f==0)
//		{
//			usart((9600*0.75));	//9600
//			rx_count=0,tx_data[1]=0x06,tx_count=1,rx_end=1;//������λ�����ҷ���0X06Ӧ��	 	 
//			f=1;
////			Err = 1;
//		}		
//	}

	if(rx_count>0)		  
	{
		rx_count++; 
		rx_data[rx_count]=rx_data[0];
		if(rx_count>500)	
    {
		  tx_count=0,rx_count=0;
    }
		if((rx_count>3)&&(rx_data[rx_count-2]==0x03))	 
		{ 
			rx_data_sum();		
			if(rx_data[599]==1)		
			  rx_end=1;
			else 
			 tx_data[1]=0x15,tx_count=1,rx_end=5;	  
		}
	}	   
}

void USART1_IRQHandler(void)
{ 
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)  
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		RX_Process(USART_ReceiveData(USART1));
	}	
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)
	{
		USART_ClearFlag(USART1,USART_FLAG_ORE); 
		USART_ReceiveData(USART1);    
	}
	if(USART_GetITStatus(USART1, USART_IT_TXE)==SET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_TXE);
		USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
	}
	if(USART_GetITStatus(USART1, USART_IT_TC)==SET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_TC);
		TX_Process();
	}
}

void write_data(u16 number)
{
	u16 temp,appoint_address;
	if(number<12)			  
	{
		FLASH_Unlock();		  
		FLASH_ErasePage(flash_start_address+number*0x800);         
		for(temp=0;temp<1024;temp++)	  
		{
			appoint_address=flash_start_address+number*0x800+temp*2;     
			FLASH_ProgramHalfWord(appoint_address,prog_write_buffer[temp]);  
		}
		FLASH_Lock();	   
	}
}

void read_data(u16 number)		 
{
	u16 temp,appoint_address;
	if(number<12)
	{
		for(temp=0;temp<1024;temp++)
		{
			appoint_address=number*0x800+temp*2;
			prog_write_buffer[temp]=p_x[appoint_address]+(p_x[appoint_address+1]*0x100);	
		}
	}
}


// ���ݱ���
void backup_data(void)
{
	u16 temp=0,backup_addr;  
	for(backup_addr=(0x2000+500);backup_addr<(0x2000+950);backup_addr++)	 // D500-950
		prog_write_buffer[temp]=all_data[backup_addr],temp++;

	for(backup_addr=(0x0500+100);backup_addr<(0x0500+150);backup_addr++)	 // C100-C150
		prog_write_buffer[temp]=all_data[backup_addr],temp++;

	for(backup_addr=(0x0800+100);backup_addr<(0x0800+150);backup_addr++)	 // T100-T150
		prog_write_buffer[temp]=all_data[backup_addr],temp++;

	for(backup_addr=(0x0020);backup_addr<(0x0020+32);backup_addr++)	 		   // M512-M1024
		prog_write_buffer[temp]=all_data[backup_addr],temp++;

	write_data(10);	  
}


void recover_data(void)
{ 
	u16 temp=0,backup_addr;
	read_data(10);
	for(backup_addr=(0x2000+500);backup_addr<(0x2000+950);backup_addr++)		 //DM
		all_data[backup_addr]=prog_write_buffer[temp], temp++;

	for(backup_addr=(0x0500+100);backup_addr<(0x0500+150);backup_addr++)		 //C
		all_data[backup_addr]=prog_write_buffer[temp], temp++;

	for(backup_addr=(0x0800+100);backup_addr<(0x0800+150);backup_addr++)		 //T
		all_data[backup_addr]=prog_write_buffer[temp], temp++;

	for(backup_addr=(0x0020);backup_addr<(0x0020+32);backup_addr++)	         //M512��ʼ-M1024����
		all_data[backup_addr]=prog_write_buffer[temp], temp++;
}
//all_data[0]-all_data[0x40]--------------------M0-M1024
//all_data[0x180/2]-all_data[0x180/2+15]--------Y00-Y3FF
//CH-X00:all_data[0x240/2]----all_data[0x240/2+15]	   16��
//CH-Y00:all_data[0x180/2]----all_data[0x180/2+15]	   16��
//	  all_data[0x2000+500]----------------------//D500-D950
//	  all_data[0x0500+100]----------------------//C100-C150


void RST_T_D_C_M_data(void)		//ADD  ���˼�  20160320
{ 
	u16 backup_addr;
	for(backup_addr=(0x2000);backup_addr<(0x2000+500);backup_addr++)		 // D0000-D499  0X2000--0X3F40
	 all_data[backup_addr]=0;

	for(backup_addr=(0x2000+951);backup_addr<(0x2000+8000);backup_addr++)// D951-D7999
	 all_data[backup_addr]=0;

	for(backup_addr=(0x0500);backup_addr<(0x0500+100);backup_addr++)		 // C0-C99
	 all_data[backup_addr]=0;

	for(backup_addr=(0x0500+151);backup_addr<(0x0500+256);backup_addr++) // C151-C255
	 all_data[backup_addr]=0;

	for(backup_addr=(0x0800);backup_addr<(0x0800+100);backup_addr++)		 // T0-T99
	 all_data[backup_addr]=0;

	for(backup_addr=(0x0800+151);backup_addr<(0x0800+256);backup_addr++) // T151-T255
	 all_data[backup_addr]=0;

	for(backup_addr=(0x00);backup_addr<(0x0020);backup_addr++)	         //M0-M511
	 all_data[backup_addr]=0;

	for(backup_addr=(0x0020+32);backup_addr<(0x0020+96);backup_addr++)	 //M512-M3072
	 all_data[backup_addr]=0;

}
