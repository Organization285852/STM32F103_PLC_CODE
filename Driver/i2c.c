/********************************************************/
// CPU��Ҫ��STM32F103--RAM�ڴ治С��64K	Flash�ڴ治С��128K
// ����������STM32F103RCT6 RDT6 VCT6 VET6����ͨ��
// �༭���ڣ�20150903
// editor by ���˼�
/********************************************************/

#include "i2c.h"
#define IIC_SDA_DAT GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
#define IIC_SDA_L   GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define IIC_SDA_H   GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define IIC_SCL_L   GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define IIC_SCL_H   GPIO_SetBits(GPIOB, GPIO_Pin_6)

void delay_5us(u16 num)//����IICʱ�����ʱ
{
	u16 i,j;
	for(i=num;i>0;i--)
	{
		for(j=10;j>0;j--);
	}
}

void IIC_Init(void)//IIC��ʼ������
{
	IIC_SDA_H;
	IIC_SCL_H;
	delay_5us(1);
}

void IIC_Start(void)//IIC��ʼ�ź�
{
	IIC_SDA_H;
	IIC_SCL_H;
	delay_5us(1);
	IIC_SDA_L;
	delay_5us(1);
	IIC_SCL_L;
}

void IIC_Stop(void)//IICֹͣ�ź�
{
	IIC_SDA_L;
	IIC_SCL_H;
	delay_5us(1);
	IIC_SDA_H;
}

void IIC_Wait_ack(void)//IIC�������ȴ�Ӧ��  ?????
{
	u8 ErrTime=255;
	IIC_SCL_H;
	//while(IIC_SDA&&(ErrTime>0))
	while(IIC_SDA_DAT)
	{
		ErrTime--;
	}
	delay_5us(1);
	IIC_SCL_L;
}


void IIC_Send_ack(void)//IIC����������Ӧ��
{
	IIC_SDA_L;
	delay_5us(1);
	IIC_SCL_H;
	delay_5us(1);
	IIC_SCL_L;
}


void IIC_Send_noack(void)//IIC���������ͷ�Ӧ��
{
	IIC_SDA_H;
	IIC_SCL_H;
	delay_5us(1);
	IIC_SCL_L;
}


void IIC_Writebyte(u8 Data)//IICдһ���ֽ�
{
	u8 i;
	for(i=8;i>0;i--)
	{
		IIC_SCL_L;
		if((Data&0x80)==0x80)
		{
			IIC_SDA_H;
		}
		else
		{
			IIC_SDA_L;
		}
		Data<<=1;
		delay_5us(1);
		IIC_SCL_H;
		delay_5us(1);
	}
	IIC_SCL_L;
	IIC_SDA_H;
	delay_5us(10);
}


u8 IIC_Readbyte(void)//IIC��һ���ֽ�
{
	u8 i,Data=0;
	IIC_SDA_H;
	for(i=8;i>0;i--)
	{
		Data<<=1;
		IIC_SCL_L;
		delay_5us(1);
		IIC_SCL_H;
		delay_5us(1);
		//   Data|=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin7) 
		Data|=IIC_SDA_DAT; //���ܽŵ�״̬
	}
	IIC_SCL_L;
	delay_5us(1);
	return(Data);
}


void IIC_Write(u16 Address,u8 Data)//IIC ��ָ����ַAddress��дData
{
	u8 page,addr;
	addr = Address&0xff;
	page = (Address>>8)&0xf;
	IIC_Start();
	IIC_Writebyte(IIC_Write_Address);
	IIC_Wait_ack();
	IIC_Writebyte(page);
	IIC_Wait_ack();
	IIC_Writebyte(addr);
	IIC_Wait_ack();
	IIC_Writebyte(Data);
	IIC_Wait_ack();
	IIC_Stop();
	delay_5us(10000);
}


void IIC_Write_Array(u8 *Data,u16 Address,u16 Num)//IIC ����ʼ��ַAddress��дData[]����
{
	u8 i;
	u8 *p;
	u8 page1,page2,yushu;
	u8 page,addr,addr1;
	addr = Address&0xff;
	page1 = Address>>5;
	page2 =(Address+Num)>>5;
	addr1 = (Address+Num)&0xff;
	p=Data;
	for(page=page1;page<=page2;page++)
	{
		if((page==page1)||(page==page2))
		{
		   if(page==page1)
		   	{
		   	 if(page1==page2)
		   	 	{
		                yushu= Num;
		   	           }
		   	
		    else
		    	{
				yushu=(32-addr&0x1f)%32;
		    	}
		   	}
		   else 
		   	yushu=addr1%32;
		}
		else
		{
			yushu=0;
		}
		IIC_Start();
		IIC_Writebyte(IIC_Write_Address);
		IIC_Wait_ack();
		IIC_Writebyte(page>>3);
		IIC_Wait_ack();
		IIC_Writebyte(addr);
		IIC_Wait_ack();
		for(i=0;i<32;i++)
		{
			addr++;
			IIC_Writebyte(*p);
			IIC_Wait_ack();
			p++;
			if(yushu!=0)
			{
				if(i==yushu-1)
				break;
			}
		}
		IIC_Stop();
		delay_5us(10000);
	}  
}


u8 IIC_Read(u16 Address)//IIC ��ָ����ַAddress���Data
{
	u8 Data;
	u8 page,addr;
	addr = Address&0xff;
	page = (Address>>8)&0xf;
	IIC_Start();
	IIC_Writebyte(IIC_Write_Address);
	IIC_Wait_ack();
	IIC_Writebyte(page);
	IIC_Wait_ack();
	IIC_Writebyte(addr);
	IIC_Wait_ack();
	IIC_Start();
	IIC_Writebyte(IIC_Read_Address);
	IIC_Wait_ack();
	Data=IIC_Readbyte();
	IIC_Send_noack();
	IIC_Stop();
	return(Data);  
}


void IIC_Read_Array(u8 *Data,u16 Address,u16 Num)//IIC ����ʼ��ַAddress���Data[]
{
	u16 i;
	u8 *p;
	u8 page,addr;
	addr = Address&0xff;
	page = Address>>8;
	p=Data;   
	IIC_Start();
	IIC_Writebyte(IIC_Write_Address);
	IIC_Wait_ack();
	IIC_Writebyte(page);
	IIC_Wait_ack();
	IIC_Writebyte(addr);
	IIC_Wait_ack();
	IIC_Start();
	IIC_Writebyte(IIC_Read_Address);
	IIC_Wait_ack();
	for(i=0;i<Num;i++)
	{
		*(p+i)=IIC_Readbyte();
		if(i==Num-1)
		IIC_Send_noack();
		else
		IIC_Send_ack();
	}
	IIC_Stop();
}


void IIC_Clear(u16 Address,u16 Num)
{
	u8 i,j;
	u8 page1,page2,yushu;
	u8 page,addr,n,addr1;
	addr = Address&0xff;
	page1 = (Address>>8)&0xf;
	page2 =((Address+Num)>>8)&0xf;
	addr1 = (Address+Num)&0xff;
	for(page=page1;page<=page2;page++)
	{
		if((page==page1)||(page==page2))
		{
			if(page==page1)
			{
				if(page1==page2)
				{
					n=Num/32;
					yushu=Num%32;
				}
				else
				{
					n=(256-addr)/32;
					yushu=(256-addr)%32;
				}
				if(yushu)
				n+=1;
			}
			else
			{
				n=addr1/32;
				yushu=addr1%32;
				if(yushu)
				n+=1;
			}
		}
		else
		{
			n=8;
			yushu=0;
		}
		for(j=0;j<n;j++)
		{
			IIC_Start();
			IIC_Writebyte(IIC_Write_Address);
			IIC_Wait_ack();
			IIC_Writebyte(page);
			IIC_Wait_ack();
			IIC_Writebyte(addr+32*j);
			IIC_Wait_ack();
			for(i=0;i<32;i++)
			{
				IIC_Writebyte(0xff);
				IIC_Wait_ack();
			}
			IIC_Stop();
			delay_5us(10000);
		}
	}
}


/*******************************************************************************************************
                                 end  file!!!
********************************************************************************************************/

