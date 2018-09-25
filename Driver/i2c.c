/********************************************************/
// CPU需要：STM32F103--RAM内存不小于64K	Flash内存不小于128K
// 本代码已在STM32F103RCT6 RDT6 VCT6 VET6测试通过
// 编辑日期：20150903
// editor by 传人记
/********************************************************/

#include "i2c.h"
#define IIC_SDA_DAT GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
#define IIC_SDA_L   GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define IIC_SDA_H   GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define IIC_SCL_L   GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define IIC_SCL_H   GPIO_SetBits(GPIOB, GPIO_Pin_6)

void delay_5us(u16 num)//满足IIC时序的延时
{
	u16 i,j;
	for(i=num;i>0;i--)
	{
		for(j=10;j>0;j--);
	}
}

void IIC_Init(void)//IIC初始化程序
{
	IIC_SDA_H;
	IIC_SCL_H;
	delay_5us(1);
}

void IIC_Start(void)//IIC开始信号
{
	IIC_SDA_H;
	IIC_SCL_H;
	delay_5us(1);
	IIC_SDA_L;
	delay_5us(1);
	IIC_SCL_L;
}

void IIC_Stop(void)//IIC停止信号
{
	IIC_SDA_L;
	IIC_SCL_H;
	delay_5us(1);
	IIC_SDA_H;
}

void IIC_Wait_ack(void)//IIC主器件等待应答  ?????
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


void IIC_Send_ack(void)//IIC主器件发送应答
{
	IIC_SDA_L;
	delay_5us(1);
	IIC_SCL_H;
	delay_5us(1);
	IIC_SCL_L;
}


void IIC_Send_noack(void)//IIC主器件发送非应答
{
	IIC_SDA_H;
	IIC_SCL_H;
	delay_5us(1);
	IIC_SCL_L;
}


void IIC_Writebyte(u8 Data)//IIC写一个字节
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


u8 IIC_Readbyte(void)//IIC读一个字节
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
		Data|=IIC_SDA_DAT; //读管脚的状态
	}
	IIC_SCL_L;
	delay_5us(1);
	return(Data);
}


void IIC_Write(u16 Address,u8 Data)//IIC 往指定地址Address里写Data
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


void IIC_Write_Array(u8 *Data,u16 Address,u16 Num)//IIC 往起始地址Address里写Data[]数组
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


u8 IIC_Read(u16 Address)//IIC 读指定地址Address里的Data
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


void IIC_Read_Array(u8 *Data,u16 Address,u16 Num)//IIC 读起始地址Address里的Data[]
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

