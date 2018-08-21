/********************************************************/
// CPU需要：STM32F103--RAM内存不小于48K	Flash内存不小于256K
// 本代码已在STM32F103RCT6 RDT6 VCT6 VET6测试通过
// 编辑日期：20150903
// editor by 传人记
/********************************************************/
// 20150905 :ADD command: INC  INCP  DEC  DECP
// 20150914 :  MPP  LD<>  AND<>  OR=  OR>  OR<  OR>=  OR<=  OR<>
// 20150917 :  WAND WOR WXOR NEG ROL ROR RCL RCR SQR SWAP
// 20151009 :  ADD RUN/STOP KEY
// 20151214 :  ADD RST_T_D_M_C()
// 20160320 :  ADD ADC&DAC 	 AD(D8030 D8031)  DA(D7030 D7031)
/********************************************************/


#include "stm32f10x.h"
#include "OS_Config.h"
#include <stdio.h>
//#include <absacc.h> 
#include "math.h"          //数学函数库 

#define on 0xff
#define off 0x00
#define ROTATE_LEFT(x, s, n)        ((x) << (n)) | ((x) >> ((s) - (n)))      //循环左移  x为数据 s为数据位数 n为移动位数
#define ROTATE_RIGHT(x, s, n)       ((x) >> (n)) | ((x) << ((s) - (n)))      //循环右移  x为数据 s为数据位数 n为移动位数
#define swap_u16(x)                 ((x) >> (8)) | ((x) << (8))            //上下交换传送

extern u16 all_data[];
extern u8 p_all_data[];
extern const u16  x[0x6800];
extern u8  step_status[];
extern u8  Run_Flag; 	//ADD	   
extern void timer_enable(u16 timer_number);	 
extern void timer_disble(u16 timer_number);	 
extern void RST_T_D_C_M_data(void);
extern void backup_data(void);
static u16 *p_data,process_value;
static const u16 *p_prog;
static u8 T_number,C_number;
static u16 T_value,C_value;
static u16 mov_d_value;
static u16 mov_d_addr;
u16 C_count[2];
static const u16 *program_start_addr;
static const u16 *prog_p_addr[129];	
static const u16 *p_save[129];      
u16 process[64];                    
u16 sub_add1,sub_add2;
u8 edit_prog;



u8 find_step(u16 addr)    
{ 
	static u16 temp1,temp2;
	temp1=addr/0x08;
	temp2=addr%0x08;
	temp2=(1<<temp2);
	if((step_status[temp1]&temp2)==temp2)
	return on;
	else		
	return off;
}

void set_step(u16 addr)    
{ 
	static u16 temp1,temp2;
	temp1=addr/0x08;
	temp2=addr%0x08;
	temp2=(1<<temp2);
	step_status[temp1]|=temp2;
}

void reset_step(u16 addr)    
{ 
	static u16 temp1,temp2;
	static u8 temp4;
	temp1=addr/0x08;
	temp2=addr%0x08;
	temp2=(1<<temp2);
	temp4=~temp2;
	step_status[temp1]&=temp4;
}

static void LD(u16 start_addr,u8 process_addr)	
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if((*p_data&(1<<temp2))==(1<<temp2)) 
	{
		process_value<<=1,process_value|=1;
	}
	else
	{
		process_value<<=1,process_value&=~1;
	}
}

static u8 LDF(u16 start_addr,u8 process_addr)	  
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if((*p_data&(1<<temp2))==(1<<temp2)) 
	return on;
	else
	return off;
}

static void LDI(u16 start_addr,u8 process_addr)
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if((*p_data&(1<<temp2))==(1<<temp2)) 
	{
		process_value<<=1,process_value&=-1;
	}
	else
	{
		process_value<<=1,process_value|=1;
	}
}

void AND(u16 start_addr,u8 process_addr)
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if(((*p_data&(1<<temp2))==(1<<temp2))&&((process_value&0X01)==0X01)) 
	process_value|=0X01;
	else
	process_value&=~0X01; 
}


static void ANI(u16 start_addr,u8 process_addr)
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if((!((*p_data&(1<<temp2))==(1<<temp2)))&&((process_value&0X01)==0X01)) 
	process_value|=0X01;
	else
	process_value&=~0X01; 
}

static void OR(u16 start_addr,u8 process_addr)
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if(((*p_data&(1<<temp2))==(1<<temp2))||((process_value&0X01)==0X01)) 
	process_value|=0X01;
	else
	process_value&=~0X01; 
}




static void ORI(u16 start_addr,u8 process_addr)
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if((!((*p_data&(1<<temp2))==(1<<temp2)))||((process_value&0X01)==0X01)) 
	process_value|=0X01;
	else
	process_value&=~0X01; 
}

static void OUT(u16 start_addr,u8 process_addr)
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if((process_value&0X01)==0X01)
	{
		*p_data|=(1<<temp2);
	}
	else
	{
		*p_data&=~(1<<temp2); 
	}
}

void force_set(u16 start_addr,u8 process_addr)  
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	*p_data|=(1<<temp2);
}

static void BIT_SET(u16 start_addr,u8 process_addr)	
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if((process_value&0X01)==0X01)
	*p_data|=(1<<temp2);
}

void force_reset(u16 start_addr,u8 process_addr) 
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	*p_data&=~(1<<temp2);
}
			 
static void RST(u16 start_addr,u8 process_addr)	
{
	u16 temp2;
	p_data=all_data+start_addr+process_addr/0x10;   
	temp2=process_addr%0x10;
	if((process_value&0X01)==0X01)
	*p_data&=~(1<<temp2);
}

static void MPS(void)				      
{
	process_value<<=1;
	if((process_value&0x02)==0x02)
	process_value|=0x01;
	else
	process_value&=~0x01;
}

static void MRD(void)					
{
	if((process_value&0x02)==0x02)
	process_value|=0x01;
	else
	process_value&=~0x01;
}

static void MPP(void)	//ADD				
{
	if((process_value&0x02)==0x02)
	process_value|=0x01;
	else
	process_value&=~0x01;
	process_value>>=1   ;
}

static void ORB(void)				  
{
	u16 temp;
	temp=process_value;
	process_value>>=1;
	if(((process_value&0x01)==0x01)||((temp&0X01)==0X01))
	process_value|=0x01;
	else
	process_value&=~0x01;
}

static void ANB(void)				   
{
	u16 temp;
	temp=process_value;
	process_value>>=1;
	if(((process_value&0x01)==0x01)&&((temp&0X01)==0X01))
	process_value|=0x01;
	else
	process_value&=~0x01;
}

static void INV(void)				   
{
    process_value = (process_value & 0xfe) | (~process_value & 0x01); 
}

static void other_function(u8 process_addr)
{
	switch(process_addr)
	{ 
		case 0xF8:   ANB()               ;	break;    //块串联 ANB
		case 0xF9:   ORB()               ;	break;    //块并联 ORB
		case 0xFA:   MPS()               ;  break;    //进栈   MPS
		case 0xFB:   MRD()               ;  break;	   //读栈   MRD
		case 0xFC:   MPP()               ;  break;    //出栈   MPP  //ADD
		case 0xFD:   INV()               ;  break;    //取反   INV
		case 0xFF: 					    break;	  //ADD
		default:  ;                       break;	
	}
}
static void extend_LD_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: LD(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: LD(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: LD(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: LD(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: LD(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: LD(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_LDI_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: LDI(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: LDI(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: LDI(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: LDI(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: LDI(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: LDI(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_OR_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: OR(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: OR(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: OR(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: OR(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: OR(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: OR(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_ORI_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: ORI(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: ORI(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: ORI(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: ORI(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: ORI(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: ORI(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_AND_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: AND(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: AND(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: AND(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: AND(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: AND(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: AND(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_ANI_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: ANI(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: ANI(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: ANI(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: ANI(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: ANI(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: ANI(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_SET_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: BIT_SET(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: BIT_SET(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: BIT_SET(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: BIT_SET(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: BIT_SET(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: BIT_SET(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_RST_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: RST(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: RST(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: RST(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: RST(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: RST(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: RST(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_OUT_M(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0xA8: OUT(0X0060,*p_prog),p_prog++;             break;  
		case 0xA9: OUT(0X0070,*p_prog),p_prog++;             break;  
		case 0xAA: OUT(0X0080,*p_prog),p_prog++;             break;  
		case 0xAB: OUT(0X0090,*p_prog),p_prog++;             break;  
		case 0xAC: OUT(0X00A0,*p_prog),p_prog++;             break;  
		case 0xAD: OUT(0X00B0,*p_prog),p_prog++;             break;  
	}
}

static void extend_SET_S(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0x80: BIT_SET(0X0140,*p_prog),p_prog++;             break;  
		case 0x81: BIT_SET(0X0150,*p_prog),p_prog++;             break;  
		case 0x82: BIT_SET(0X0160,*p_prog),p_prog++;             break;  
		case 0x83: BIT_SET(0X0170,*p_prog),p_prog++;             break;  
	}
}

static void extend_RST_S(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0x80: RST(0X0140,*p_prog),p_prog++;             break;  
		case 0x81: RST(0X0150,*p_prog),p_prog++;             break;  
		case 0x82: RST(0X0160,*p_prog),p_prog++;             break;  
		case 0x83: RST(0X0170,*p_prog),p_prog++;             break;  
	}
}

static void extend_OUT_S(void)   
{ 
	switch(*p_prog/0x100)
	{
		case 0x80: OUT(0X0140,*p_prog),p_prog++;             break;  
		case 0x81: OUT(0X0150,*p_prog),p_prog++;             break;  
		case 0x82: OUT(0X0160,*p_prog),p_prog++;             break;  
		case 0x83: OUT(0X0170,*p_prog),p_prog++;             break;  
	}
}

static void RESET_T(u8 process_addr) 		
{ 
	if((process_value&0x01)==0x01) 			
	{
		timer_disble(process_addr);
	}
	OUT(0X0380,*p_prog);
}

static void RESET_C(u8 process_addr) 	   
{ 
	if((process_value&0x01)==0x01) 		   
	{ 
		p_data=all_data+0x0500+process_addr;  
		*p_data=0;							   
		p_data=all_data+0x00F0+(process_addr/0x10); 
		*p_data&=~(1<<process_addr%0x10);	   
	}
	OUT(0X0370,*p_prog);
}

static void extend_RST_T(void)             
{  
	switch(*p_prog/0x100)
	{
		case 0x86: RESET_T(*p_prog),p_prog++;            break;  
		case 0x8E: RESET_C(*p_prog),p_prog++;            break;  
	}
}

static void MOV_TO_K_H(void)	   
 {  
  static u8 LL_BIT;      
  static u16 JOB_ADDR;
  static u32 MOV_DATA_16BIT,MOV_DATA_16BIT_BACKUP,MOV_DATA_BACKUP1;  

   LL_BIT=mov_d_addr%0x10;						 
   JOB_ADDR=mov_d_addr/0x10;					 

   switch(*p_prog/0x100)							 
	
    {
	 case 0x82: MOV_DATA_16BIT_BACKUP=mov_d_value&0X000F,MOV_DATA_16BIT_BACKUP<<=LL_BIT,MOV_DATA_BACKUP1=~(0X000F<<LL_BIT); break;
	 case 0x84: MOV_DATA_16BIT_BACKUP=mov_d_value&0X00FF,MOV_DATA_16BIT_BACKUP<<=LL_BIT,MOV_DATA_BACKUP1=~(0X00FF<<LL_BIT); break;
	 case 0x86: MOV_DATA_16BIT_BACKUP=mov_d_value&0X0FFF,MOV_DATA_16BIT_BACKUP<<=LL_BIT,MOV_DATA_BACKUP1=~(0X0FFF<<LL_BIT); break;
	 case 0x88: MOV_DATA_16BIT_BACKUP=mov_d_value       ,MOV_DATA_16BIT_BACKUP<<=LL_BIT,MOV_DATA_BACKUP1=~(0XFFFF<<LL_BIT); break;
	 default:     p_prog+=3;                           break;  
	    }

   switch(*p_prog%0x100)
              
    {
	 case 0x00: MOV_DATA_16BIT=all_data[0x0140+JOB_ADDR]+(all_data[0x0141+JOB_ADDR])*0X10000,
	           
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
			   
				all_data[0x0140+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0141+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x01: MOV_DATA_16BIT=all_data[0x0150+JOB_ADDR]+(all_data[0x0151+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0150+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0151+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x02: MOV_DATA_16BIT=all_data[0x0160+JOB_ADDR]+(all_data[0x0161+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0160+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0161+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x03: MOV_DATA_16BIT=all_data[0x0170+JOB_ADDR]+(all_data[0x0171+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0170+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0171+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;

	 case 0x04: MOV_DATA_16BIT=all_data[0x0120+JOB_ADDR]+(all_data[0x0121+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0120+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0121+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x05: MOV_DATA_16BIT=all_data[0x00C0+JOB_ADDR]+(all_data[0x00C1+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x00C0+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x00C1+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
				
	 case 0x08: MOV_DATA_16BIT=all_data[0x0000+JOB_ADDR]+(all_data[0x0001+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0000+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0001+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x09: MOV_DATA_16BIT=all_data[0x0010+JOB_ADDR]+(all_data[0x0011+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0010+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0011+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x0A: MOV_DATA_16BIT=all_data[0x0020+JOB_ADDR]+(all_data[0x0021+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0020+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0021+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x0B: MOV_DATA_16BIT=all_data[0x0030+JOB_ADDR]+(all_data[0x0031+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0030+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0031+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x0C: MOV_DATA_16BIT=all_data[0x0040+JOB_ADDR]+(all_data[0x0041+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0040+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0041+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x0D: MOV_DATA_16BIT=all_data[0x0050+JOB_ADDR]+(all_data[0x0051+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0050+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0051+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;

	 case 0x28: MOV_DATA_16BIT=all_data[0x0060+JOB_ADDR]+(all_data[0x0061+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0060+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0061+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x29: MOV_DATA_16BIT=all_data[0x0070+JOB_ADDR]+(all_data[0x0071+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0070+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0071+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x2A: MOV_DATA_16BIT=all_data[0x0080+JOB_ADDR]+(all_data[0x0081+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0080+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0081+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x2B: MOV_DATA_16BIT=all_data[0x0090+JOB_ADDR]+(all_data[0x0091+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x0090+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x0091+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x2C: MOV_DATA_16BIT=all_data[0x00A0+JOB_ADDR]+(all_data[0x00A1+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x00A0+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x00A1+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
	 case 0x2D: MOV_DATA_16BIT=all_data[0x00B0+JOB_ADDR]+(all_data[0x00B1+JOB_ADDR])*0X10000,
	            MOV_DATA_16BIT&=MOV_DATA_BACKUP1,MOV_DATA_16BIT|=MOV_DATA_16BIT_BACKUP,
				all_data[0x00B0+JOB_ADDR]=MOV_DATA_16BIT,all_data[0x00B1+JOB_ADDR]=MOV_DATA_16BIT/0X10000; break;
				
				 
	  default:     p_prog++;                           break;  
        }
		p_prog++;	
     }
static void MOV_K(void)	  
 {static u8 LL_BIT;      
  static u32 MOV_DATA_16BIT;   

   LL_BIT=mov_d_value%0x10;						 

   switch(*p_prog%0x100)
    {
	 case 0x00: MOV_DATA_16BIT=all_data[0x0140+mov_d_value/0x10]+(all_data[0x0141+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x01: MOV_DATA_16BIT=all_data[0x0150+mov_d_value/0x10]+(all_data[0x0151+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x02: MOV_DATA_16BIT=all_data[0x0160+mov_d_value/0x10]+(all_data[0x0161+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x03: MOV_DATA_16BIT=all_data[0x0170+mov_d_value/0x10]+(all_data[0x0171+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;

	 case 0x04: MOV_DATA_16BIT=all_data[0x0120+mov_d_value/0x10]+(all_data[0x0121+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x05: MOV_DATA_16BIT=all_data[0x00C0+mov_d_value/0x10]+(all_data[0x00C1+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
				
	 case 0x08: MOV_DATA_16BIT=all_data[0x0000+mov_d_value/0x10]+(all_data[0x0001+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x09: MOV_DATA_16BIT=all_data[0x0010+mov_d_value/0x10]+(all_data[0x0011+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x0A: MOV_DATA_16BIT=all_data[0x0020+mov_d_value/0x10]+(all_data[0x0021+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x0B: MOV_DATA_16BIT=all_data[0x0030+mov_d_value/0x10]+(all_data[0x0031+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x0C: MOV_DATA_16BIT=all_data[0x0040+mov_d_value/0x10]+(all_data[0x0041+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x0D: MOV_DATA_16BIT=all_data[0x0050+mov_d_value/0x10]+(all_data[0x0051+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;

	 case 0x28: MOV_DATA_16BIT=all_data[0x0060+mov_d_value/0x10]+(all_data[0x0061+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x29: MOV_DATA_16BIT=all_data[0x0070+mov_d_value/0x10]+(all_data[0x0071+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x2A: MOV_DATA_16BIT=all_data[0x0080+mov_d_value/0x10]+(all_data[0x0081+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x2B: MOV_DATA_16BIT=all_data[0x0090+mov_d_value/0x10]+(all_data[0x0091+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x2C: MOV_DATA_16BIT=all_data[0x00A0+mov_d_value/0x10]+(all_data[0x00A1+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
	 case 0x2D: MOV_DATA_16BIT=all_data[0x00B0+mov_d_value/0x10]+(all_data[0x00B1+mov_d_value/0x10])*0X10000,
	            MOV_DATA_16BIT>>=LL_BIT,mov_d_value=MOV_DATA_16BIT; break;
						 
	  default:     p_prog+=2;                           break;  
	    }
     }

static void K_M_MOV_D_H(void)	   
{							 
	switch(*p_prog/0x100)
	{
		case 0x82: MOV_K(),mov_d_value&=0X000F; break;
		case 0x84: MOV_K(),mov_d_value&=0X00FF; break;
		case 0x86: MOV_K(),mov_d_value&=0X0FFF; break;
		case 0x88: MOV_K()                   ; break;
		default:     p_prog+=3;                           break;  
	}
}

static u16 h_cos_value(u16 l_value)
 {
     static u16 temp; 
     switch(*p_prog/0x100)
    {
	 case 0x80: temp=l_value+((*p_prog%0x100)*0x100),p_prog++; break;                             
	 case 0x82: temp=l_value+((*p_prog%0x100)*0x100),temp=all_data[0x0800+temp/2],p_prog++; break;
	 case 0x84: temp=l_value+((*p_prog%0x100)*0x100),temp=all_data[0x0500+temp/2],p_prog++; break;
	 case 0x86: temp=l_value+((*p_prog%0x100)*0x100),temp=all_data[0x2000+temp/2],p_prog++; break;
	 case 0x88: temp=l_value+((*p_prog%0x100)*0x100),temp=all_data[0x2000+temp/2+1000],p_prog++; break;
	}
	return temp;
}
static u16  cos_value(void)	   
{
	static u16 temp; 
	switch(*p_prog/0x100)
	{
		case 0x80: temp=*p_prog%0x100,p_prog++,temp=h_cos_value(temp); break;
		case 0x82: temp=*p_prog%0x100,p_prog++,temp=h_cos_value(temp); break;
		case 0x84: mov_d_value=*p_prog%0x100,p_prog++,K_M_MOV_D_H(),temp=mov_d_value,p_prog++;  break;	
		case 0x86: temp=*p_prog%0x100,p_prog++,temp=h_cos_value(temp); break;
		case 0x88: temp=*p_prog%0x100,p_prog++,temp=h_cos_value(temp); break;
	}
	return temp;
}

static void add_assgin_value(u16 assgin_value) 
{
	static u16 temp; 
	switch(*p_prog/0x100)
	{
		case 0x82: temp=assgin_value+((*p_prog%0x100)*0x100),all_data[0x0800+temp/2]=sub_add1,p_prog++; break;
		case 0x84: temp=assgin_value+((*p_prog%0x100)*0x100),all_data[0x0500+temp/2]=sub_add1,p_prog++; break;
		case 0x86: temp=assgin_value+((*p_prog%0x100)*0x100),all_data[0x2000+temp/2]=sub_add1,p_prog++; break;
		case 0x88: temp=assgin_value+((*p_prog%0x100)*0x100),all_data[0x2000+temp/2+1000]=sub_add1,p_prog++; break;
	}
} 

static void add_target(void)	
{ 
	u16 temp;
	switch(*p_prog/0x100)
	{
		case 0x82: temp=*p_prog%0x100,p_prog++,add_assgin_value(temp); break;	 
		case 0x84: mov_d_addr=*p_prog%0x100,p_prog++,mov_d_value=sub_add1,MOV_TO_K_H(); break;	         
		case 0x86: temp=*p_prog%0x100,p_prog++,add_assgin_value(temp); break;
		case 0x88: temp=*p_prog%0x100,p_prog++,add_assgin_value(temp); break;
	}
}

static void div_assign_value(u16 l_value) 
{
	static u16 temp; 
	switch(*p_prog/0x100)
	{
		case 0x82: temp=l_value+((*p_prog%0x100)*0x100),all_data[0x0800+temp/2]=sub_add1,all_data[0x0801+temp/2]=sub_add2,p_prog++; break;
		case 0x84: temp=l_value+((*p_prog%0x100)*0x100),all_data[0x0500+temp/2]=sub_add1,all_data[0x0501+temp/2]=sub_add2,p_prog++; break;
		case 0x86: temp=l_value+((*p_prog%0x100)*0x100),all_data[0x2000+temp/2]=sub_add1,all_data[0x2001+temp/2]=sub_add2,p_prog++; break;
		case 0x88: temp=l_value+((*p_prog%0x100)*0x100),all_data[0x2000+temp/2+1000]=sub_add1,all_data[0x2000+temp/2+1001]=sub_add2,p_prog++; break;
	}
}

static void mul_target(void)									
{
	u16 temp;
	switch(*p_prog/0x100)
	{
		case 0x82: temp=*p_prog%0x100,p_prog++,div_assign_value(temp); break;
		case 0x84: mov_d_addr=*p_prog%0x100,p_prog++,mov_d_value=sub_add1,MOV_TO_K_H(); break;	         
		case 0x86: temp=*p_prog%0x100,p_prog++,div_assign_value(temp); break;
		case 0x88: temp=*p_prog%0x100,p_prog++,div_assign_value(temp); break;
	}
}

static void extend_MOV(void)	   
{
	if((process_value&0X01)==0X01)  
	sub_add1=cos_value(),add_target();  
	else
	p_prog+=4;		          
}

static void extend_MOVP(void)	  
{
	u8  logic_1;   
	logic_1=find_step(p_prog-program_start_addr);  
	if(logic_1==off)							   
	{ 
		if((process_value&0x01)==0x01)					   
		{ 
			set_step(p_prog-program_start_addr);
			sub_add1=cos_value(),add_target(); 
		}
		else
		p_prog+=4;		  
	}
	else
	{
		if(!((process_value&0x01)==0x01))						 
		reset_step(p_prog-program_start_addr);		 
		p_prog+=4;		  
	} 
}

static void WAND(void)	                          //逻辑运算“与”
{ 
	signed short int temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value();
		temp2=cos_value();
		sub_add1=temp1&temp2;
		add_target();
	}
	else p_prog+=6;              //跳过6步程序
}

static void WOR(void)	                          //逻辑运算“与”
{ 
	signed short int temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value();
		temp2=cos_value();
		sub_add1=temp1|temp2;
		add_target();
	}
	else p_prog+=6;              //跳过6步程序
}

static void WXOR(void)	                          //逻辑运算“异或”
{ 
	if((process_value&0X01)==0X01)
	{
		sub_add1=cos_value()^cos_value();
		add_target();
	}
	else p_prog+=6;              //跳过6步程序
}

static void NEG(void)                             //逻辑运算
{
	if((process_value&0X01)==0X01)
	{
		sub_add1=0-cos_value();
		p_prog-=2;
		add_target();
	}
	else p_prog+=2;              //跳过2步程序
}

static void ROR(void)	                 
{  	
	u16 temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value(); 
		temp2=cos_value(); 
		sub_add1=ROTATE_RIGHT(temp1,16,temp2);
		p_prog-=4;
		mul_target();	
		p_prog+=2;
	}
	else p_prog+=4;                      //没有动作跳过4步程序
}

static void ROL(void)	                 
{  	
	u16 temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value(); 
		temp2=cos_value(); 
		sub_add1=ROTATE_LEFT(temp1,16,temp2);
		p_prog-=4;
		mul_target();	
		p_prog+=2;
	}
	else p_prog+=4;                      //没有动作跳过4步程序
}

//======================================================================================================
// 函数名称:  static void RCR(void)
// 功能描述： 16位右移位 RCR指令
//=======================================================================================================
static void RCR(void)	                 
{  	
	u16 temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value(); 
		temp2=cos_value(); 
		sub_add1=temp1>>temp2;
		p_prog-=4;
	    mul_target();
		p_prog+=2;
	}
	else p_prog+=4;                       //没有动作跳过4步程序
}

//======================================================================================================
// 函数名称:  static void ROL(void)
// 功能描述： 16位左移位 RCL指令 
//=======================================================================================================
static void RCL(void)	                 
{  	
	u16 temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value(); 
		temp2=cos_value(); 
		sub_add1=temp1<<temp2;
		p_prog-=4;
		mul_target();	
		p_prog+=2;
	}
	else p_prog+=4;                      //没有动作跳过4步程序
}

//=======================================================================================================
// 函数名称:  static void SWAP(void)	
// 功能描述： 16位上下交换 SWAP指令
//=======================================================================================================
static void SWAP(void)	                 
{  	
	signed short int temp;
	if((process_value&0X01)==0X01)
	{
		temp=cos_value();	  
		sub_add1=swap_u16(temp);
		p_prog-=2;
		add_target();		 
	}
	else p_prog+=4;                      //没有动作跳过4步程序
}

//======================================================================================================
// 函数名称:  static void SQR(void)
// 功能描述： SQR 开方
//=======================================================================================================
static void SQR(void)
{
   if((process_value&0X01)==0X01)
   {
  		sub_add1=(u16)sqrt((double)cos_value());
	    add_target();	
   } 
}


static void add(void)	   
{ 
	signed short int temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value();
		temp2=cos_value();
		sub_add1=temp1+temp2;
		add_target();
	}
}

static void INC(void)	   	 //ADD 传人记
{
	if((process_value&0X01)==0X01)
	{
		sub_add1=cos_value();
		p_prog-=2;
		sub_add1++;
		add_target();
	}
	else
	p_prog+=2;
}


static void extend_INCP(void)	  
{ 
	u8  logic_1;     
	logic_1=find_step(p_prog-program_start_addr);  
	if(logic_1==off)							   
	{ 
		if((process_value&0x01)==0x01)					   
		{ 
			set_step(p_prog-program_start_addr);
			INC();  
		}
		else
		p_prog+=2;		  
	}
	else
	{
		if(!((process_value&0x01)==0x01))						 
		reset_step(p_prog-program_start_addr);		 
		p_prog+=2;		  
	} 
}

static void DEC(void)	   	 //ADD 传人记	//逻辑运算 减1指令
{
	if((process_value&0X01)==0X01)
	{
		sub_add1=cos_value();
		p_prog-=2;
		sub_add1--;
		add_target();
	}
	else
	p_prog+=2;	    //跳过2步程序
}

static void extend_DECP(void)	  
{ 
	u8  logic_1; 	
	logic_1=find_step(p_prog-program_start_addr);  
	if(logic_1==off)							   
	{ 
		if((process_value&0x01)==0x01)					   
		{ 
			set_step(p_prog-program_start_addr);
			DEC();  
		}
		else
		p_prog+=2;		  
	}
	else
	{
		if(!((process_value&0x01)==0x01))						 
		reset_step(p_prog-program_start_addr);		 
		p_prog+=2;		  
	} 
}


static void extend_ADDP(void)	  
{ 
	u8  logic_1; 
	logic_1=find_step(p_prog-program_start_addr);  
	if(logic_1==off)							   
	{ 
		if((process_value&0x01)==0x01)					   
		{ 
			set_step(p_prog-program_start_addr);
			add();  
		}
		else
		p_prog+=4;		  
	}
	else
	{
		if(!((process_value&0x01)==0x01))						 
		reset_step(p_prog-program_start_addr);		 
		p_prog+=4;		  
	} 
}

static void sub(void)	   
{ 
	signed short int temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value();
		temp2=cos_value();
		sub_add1=temp1-temp2;
		add_target();
	}
}

static void extend_SUBP(void)	  
{
	u8  logic_1;     
	logic_1=find_step(p_prog-program_start_addr);  
	if(logic_1==off)							   
	{ 
		if((process_value&0x01)==0x01)					   
		{
			set_step(p_prog-program_start_addr);
			sub();  
		}
		else
		p_prog+=4;		  
	}
	else
	{
		if(!((process_value&0x01)==0x01))						 
		reset_step(p_prog-program_start_addr);		 
		p_prog+=4;		  
	} 
}

static void mul(void)	 
{ 
	signed int temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value();
		temp2=cos_value();
		sub_add1=(temp1*temp2)%0x10000;
		sub_add2=(temp1*temp2)/0x10000;
		mul_target();
	}
}

static void extend_MULP(void)	  
{ 
	u8  logic_1;    
	logic_1=find_step(p_prog-program_start_addr);  
	if(logic_1==off)							   
	{ 
		if((process_value&0x01)==0x01)					   
		{
			set_step(p_prog-program_start_addr);
			mul();  
		}
		else
		p_prog+=4;		  
	}
	else
	{
		if(!((process_value&0x01)==0x01))						 
		reset_step(p_prog-program_start_addr);		 
		p_prog+=4;		  
	} 
}

static void div(void)	 
{ 
	signed short int temp1,temp2;
	if((process_value&0X01)==0X01)
	{
		temp1=cos_value();
		temp2=cos_value();
		sub_add1=temp1/temp2;
		sub_add2=temp1%temp2;
		mul_target();
	}
}

static void extend_DIVP(void)	  
{ 
	u8  logic_1;    
	logic_1=find_step(p_prog-program_start_addr);  
	if(logic_1==off)							   
	{ 
		if((process_value&0x01)==0x01)					   
		{ 
			set_step(p_prog-program_start_addr);
			div();  
		}
		else
		p_prog+=4;		  
	}
	else
	{
		if(!((process_value&0x01)==0x01))						 
		reset_step(p_prog-program_start_addr);		 
		p_prog+=4;		  
	} 
}

static void amount(void)	 
{
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if(temp1==temp2) 
	process_value<<=1,process_value|=1;
	else
	process_value<<=1,process_value&=~1;
}

static void big(void)		
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if(temp1>temp2) 
	process_value<<=1,process_value|=1;
	else
	process_value<<=1,process_value&=~1;
}


static void less(void)	   
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if(temp1<temp2) 
	process_value<<=1,process_value|=1;
	else
	process_value<<=1,process_value&=~1;
}



static void less_amount(void)	   
{
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if(temp1>temp2) 
	process_value<<=1,process_value|=1;
	else
	process_value<<=1,process_value&=~1;
}
	


static void big_amount(void)	   
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if(temp1<temp2) 
	process_value<<=1,process_value|=1;
	else
	process_value<<=1,process_value&=~1;
}

static void no_amount(void)	   //ADD  LD<>
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if(temp1!=temp2) 
	process_value<<=1,process_value|=1;
	else
	process_value<<=1,process_value&=~1;
}

static void amount_and(void)	 
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1==temp2)&&((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void big_and(void)		
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1>temp2)&&((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void less_and(void)	   
{
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1<temp2)&&((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void less_amount_and(void)	   
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1>temp2)&&((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void big_amount_and(void)	   
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1<temp2)&&((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void no_amount_and(void)	 //ADD  AND<>  
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1<temp2)&&((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}


static void amount_OR(void)	 	  //ADD  OR=
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1==temp2)||((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void big_OR(void)		  //ADD  OR>
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1>temp2)||((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void less_OR(void)	   	   //ADD  OR<
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1<temp2)||((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void less_amount_OR(void)   //ADD  OR<= 	   
{
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1>temp2)||((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void big_amount_OR(void)	   //ADD  OR>= 
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1<temp2)||((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}

static void no_amount_OR(void)	 //ADD  OR<>  
{ 
	signed short int temp1,temp2;
	temp1=cos_value();
	temp2=cos_value();
	if((temp1<temp2)||((process_value&0X01)==0X01)) 
	process_value|=1;
	else
	process_value&=~1;
}


static u8 bit_value(void)	 
 {  u8  temp;
    switch(*p_prog/0X100)
     { case 0x80: temp=LDF(0X0140,*p_prog); break;  
	   case 0x81: temp=LDF(0X0150,*p_prog); break;  
	   case 0x82: temp=LDF(0X0160,*p_prog); break;  
	   case 0x83: temp=LDF(0X0170,*p_prog); break;  
	   case 0x84: temp=LDF(0X0120,*p_prog); break;  
	   case 0x85: temp=LDF(0X00C0,*p_prog); break;  
	   case 0x86: temp=LDF(0X0100,*p_prog); break;  
	   
	   case 0x88: temp=LDF(0X0000,*p_prog); break;  
	   case 0x89: temp=LDF(0X0010,*p_prog); break;  
	   case 0x8A: temp=LDF(0X0020,*p_prog); break;  
	   case 0x8B: temp=LDF(0X0030,*p_prog); break;  
	   case 0x8C: temp=LDF(0X0040,*p_prog); break;  
	   case 0x8D: temp=LDF(0X0050,*p_prog); break;  
	   case 0x8E: temp=LDF(0X00F0,*p_prog); break;  
	   case 0x8F: temp=LDF(0X00E0,*p_prog); break;  

	   case 0xA8: temp=LDF(0X0060,*p_prog); break;  
	   case 0xA9: temp=LDF(0X0070,*p_prog); break;  
	   case 0xAA: temp=LDF(0X0080,*p_prog); break;  
	   case 0xAB: temp=LDF(0X0090,*p_prog); break;  
	   case 0xAC: temp=LDF(0X00A0,*p_prog); break;  
	   case 0xAD: temp=LDF(0X00B0,*p_prog); break;  

	   default:  temp=2   ;                       break;  
         }
		 return temp;
     }
static void extend_LDP(void)	  
{ 
	u8  logic_1,logic_2;
	logic_1=find_step(p_prog-program_start_addr);	 
	logic_2=bit_value();							 
	if(logic_1==off)							   
	{ 
		if(logic_2==on)							   
		process_value<<=1,process_value|=1,set_step(p_prog-program_start_addr);
		else
		process_value<<=1,process_value&=~1;;		 
	}
	else
	{
		if(logic_2==on)							   
		process_value<<=1,process_value&=~1;
		else
		process_value<<=1,process_value&=~1,reset_step(p_prog-program_start_addr);		 
	}  
	p_prog++;	    
}

static void extend_LDF(void)	 
{
	u8  logic_3,logic_4;
	logic_3=find_step(p_prog-program_start_addr);	 
	logic_4=bit_value();				  		 
	if(logic_3==on)							   
	{ 
		if(logic_4==off)						   
		process_value<<=1,process_value|=1,reset_step(p_prog-program_start_addr);
		else
		process_value<<=1,process_value&=~1;		 
	}
	else
	{
		if(logic_4==off)							   
		process_value<<=1,process_value&=~1;
		else
		process_value<<=1,process_value&=~1,set_step(p_prog-program_start_addr);		 
	}  
	p_prog++;
}

static void extend_ANDP(void)	 
{ 
	u8  logic_1,logic_2;
	logic_1=find_step(p_prog-program_start_addr);	 
	logic_2=bit_value();
	if(logic_1==off)							   
	{ 
		if(logic_2==on)							   
		logic_2=1,set_step(p_prog-program_start_addr);
		else
		logic_2=0;		 
	}
	else
	{
		if(logic_2==on)							   
		logic_2=0;
		else
		logic_2=0,reset_step(p_prog-program_start_addr); 
	}  
	if(((process_value&0x01)==0x01)&&(logic_2==1))
	process_value|=1;
	else
	process_value&=~1;
	p_prog++;	    
}

static void extend_ANDF(void)	 
{ 
	u8  logic_3,logic_4;
	logic_3=find_step(p_prog-program_start_addr);	 
	logic_4=bit_value();
	if(logic_3==on)							   
	{ 
		if(logic_4==off)						   
		logic_4=1,reset_step(p_prog-program_start_addr);
		else
		logic_4=0;		 
	}
	else
	{
		if(logic_4==off)							   
		logic_4=0;
		else
		logic_4=0,set_step(p_prog-program_start_addr);		 
	}  
	if(((process_value&0x01)==0x01)&&(logic_4==1))
	process_value|=1;
	else
	process_value&=~1;
	p_prog++;
}

static void extend_ORP(void)	 
{
	u8  logic_1,logic_2;
	logic_1=find_step(p_prog-program_start_addr);	 
	logic_2=bit_value();
	if(logic_1==off)							   
	{ 
		if(logic_2==on)							   
		logic_2=1,set_step(p_prog-program_start_addr);
		else
		logic_2=0;		 
	}
	else
	{
		if(logic_2==on)							   
		logic_2=0;
		else
		logic_2=0,reset_step(p_prog-program_start_addr);		 
	}  
	if(((process_value&0x01)==0x01)||(logic_2==1))
	process_value|=1;
	else
	process_value&=~1;
	p_prog++;	    
}

static void extend_ORF(void)	 
{ 
	u8  logic_3,logic_4;
	logic_3=find_step(p_prog-program_start_addr);	 
	logic_4=bit_value();
	if(logic_3==on)							   
	{ 
		if(logic_4==off)						   
		logic_4=1,reset_step(p_prog-program_start_addr);
		else
		logic_4=0;		 
	}
	else
	{
		if(logic_4==off)							   
		logic_4=0;
		else
		logic_4=0,set_step(p_prog-program_start_addr);	
	}  
	if(((process_value&0x01)==0x01)||(logic_4==1))
	process_value|=1;
	else
	process_value&=~1;
	p_prog++;
}
static void CJ_EX(u16 value)  
{ 
	p_prog++;
	switch(*p_prog/0X100)
	{ 
		case 0x80: p_prog=prog_p_addr[value/2],p_prog++;   break;  
		default:               break;
	}
}

static void CJ(void)
{ 
	if((process_value&0X01)==0X01)
	{
		switch(*p_prog/0X100)
		{ 
			case 0x88: CJ_EX(*p_prog%0X100);   break;  
		}
	}
}

static void RET(void)
{ 
	u8 temp;
	process_value=process[0];	
	p_prog=p_save[0];	  	    
	for(temp=62;temp>0;temp--)
	{
		process[temp]=process[temp+1];    
		p_save[temp]=p_save[temp+1]; 
	}
}
static void P_MOV(void)
{ 
	u8 temp;
	for(temp=62;temp>0;temp--)
	{
		process[temp+1]=process[temp];    
		p_save[temp+1]=p_save[temp]; 
	}
	process[0]=process_value;	
	p_save[0]=p_prog;				
}

static void CALL_EX(u16 value)
{ 
	p_prog++;
	switch(*p_prog/0X100)
	{ 
		case 0x80: P_MOV(),p_prog=prog_p_addr[value/2];   break;  
	}
}

static void CALL(void)
{ 
	if((process_value&0X01)==0X01)
	{ 
		switch(*p_prog/0X100)
		{ 
			case 0x88: CALL_EX(*p_prog%0X100),p_prog++;   break;  
		}
	}
}

static void extend_function(void) 
 { 
    switch(*p_prog) 
    {
	 case 0x0002: p_prog++,extend_OUT_M();            break;  
	 case 0x0003: p_prog++,extend_SET_M();            break;  
	 case 0x0004: p_prog++,extend_RST_M();            break;  

	 case 0x0005: p_prog++,extend_OUT_S();            break;  
	 case 0x0006: p_prog++,extend_SET_S();            break;  
	 case 0x0007: p_prog++,extend_RST_S();            break;  

	 case 0x000C: p_prog++,extend_RST_T();            break;  
	 case 0X0038: p_prog++,add();					  break; 

	case 0x0040: p_prog++,INC();                     break;  //ADD BY 传人记 
	case 0x0042: p_prog++,DEC();                     break;  //ADD BY 传人记
	case 0x0044: p_prog++,WAND();	                 break;  //逻辑运算与逻辑	 ADD BY 传人记
	case 0x0046: p_prog++,WOR();                     break;  //逻辑运算或逻辑
	case 0x0048: p_prog++,WXOR();                    break;  //逻辑运算异或逻辑
	case 0x004A: p_prog++,NEG();                     break;  //逻辑运算取负数
	case 0x004C: p_prog++,ROR();                     break;  //ROR
	case 0x004E: p_prog++,ROL();                     break;  //ROL
	case 0x0050: p_prog++,RCR();                     break;  //RCR
	case 0x0052: p_prog++,RCL();                     break;  //RCL
	case 0x0070: p_prog++,SQR();	                 break;  //SQR16位整数开方
	case 0x0136: p_prog++,SWAP();                    break;  //SWAP

	 case 0X003A: p_prog++,sub();					  break;  
	 case 0x003C: p_prog++,mul();                     break;  
	 case 0x003E: p_prog++,div();                     break;  
	 
	 case 0x0010: p_prog++,CJ();                      break;  
	 case 0x0012: p_prog++,CALL();                    break;  

//	 case 0x0072: p_prog++,FLT();	                  break;  //16位整数转浮点	  //ADD BY 传人记
//   case 0x0094: p_prog++,ALT();	                   break;  //ALT

	 case 0x01C2: p_prog++,extend_LD_M();             break;  
	 case 0x01C3: p_prog++,extend_LDI_M();            break;  
	 case 0x01C4: p_prog++,extend_AND_M();            break;  
	 case 0x01C5: p_prog++,extend_ANI_M();            break;  
	 case 0x01C6: p_prog++,extend_OR_M();             break;  
	 case 0x01C7: p_prog++,extend_ORI_M();            break;  

	 case 0x01CA: p_prog++,extend_LDP();			  break;  
	 case 0x01CB: p_prog++,extend_LDF();			  break;  
	 case 0x01CC: p_prog++,extend_ANDP();			  break;  
	 case 0x01CD: p_prog++,extend_ANDF();			  break;  
	 case 0x01CE: p_prog++,extend_ORP();			  break;  
	 case 0x01CF: p_prog++,extend_ORF();			  break;  

	 case 0x0028: p_prog++,extend_MOV();		      break;  

	 case 0x000f: p_prog=p_prog;                      break;  

	 case 0X01D0: p_prog++,amount();                  break;  
	 case 0X01D2: p_prog++,big();                     break;  
	 case 0X01D4: p_prog++,less();                    break; 
	 case 0X01D8: p_prog++,no_amount();	              break;  //LD 16位不等于比较指令 
	 case 0X01DA: p_prog++,less_amount();             break;  
	 case 0X01DC: p_prog++,big_amount();              break;  
	 case 0X1028: p_prog++,extend_MOVP();             break;  
	 case 0X1038: p_prog++,extend_ADDP();             break;  
	 case 0X103A: p_prog++,extend_SUBP();             break;  
	 case 0X103C: p_prog++,extend_MULP();             break;  
	 case 0X103E: p_prog++,extend_DIVP();             break;  
	 
	 case 0x1040: p_prog++,extend_INCP();              break;  //16位上升沿逻辑运算加1指令  传人记
	 case 0x1042: p_prog++,extend_DECP();              break;  //16位上升沿逻辑运算减1指令	传人记

	 case 0X01E0: p_prog++,amount_and();                  break;  
	 case 0X01E2: p_prog++,big_and();                     break;  
	 case 0X01E4: p_prog++,less_and();                    break; 
	 case 0X01E8: p_prog++,no_amount_and(); 	          break;  //LD AND  16位不等于比较指令  传人记
	 case 0X01EA: p_prog++,less_amount_and();             break;  
	 case 0X01EC: p_prog++,big_amount_and();              break;  

	 case 0X01F0: p_prog++,amount_OR();               break;  //LD OR 16位等于比较	   传人记
	 case 0X01F2: p_prog++,big_OR();                  break;  //LD OR 16位大于比较
	 case 0X01F4: p_prog++,less_OR();                 break;  //LD OR 16位小于比较
	 case 0X01F8: p_prog++,no_amount_OR();            break;  //LD OR 16位不等于比较
	 case 0X01FA: p_prog++,less_amount_OR();          break;  //LD OR 16位小于等于比较
	 case 0X01FC: p_prog++,big_amount_OR();           break;  //LD OR 16位大于等于比较

	 case 0XF7FF: p_prog++,RET();                     break;  
	 	 
	 default:     p_prog++;                           break;  
	    }
}

static void enable_T_K(void)
{
	T_value=*p_prog%0x100;   
	p_prog++;
	T_value+=(*p_prog%0x100)*0x100;  
	p_data=all_data+0x1000+T_number; 
	*p_data=T_value;       
	timer_enable(T_number);
	OUT(0X280,T_number);
}

static void enable_T_D(void)
{
	p_data=all_data+0x1000+T_number;
	*p_data=all_data[0x2000+T_value];
	timer_enable(T_number);
	OUT(0X280,T_number);
}

static void disable_T(void)
{
	timer_disble(T_number);
	OUT(0X0280,T_number);	 
	OUT(0x0100,T_number);	 
}

static void T_given_value_K(void)	      
{
	if((process_value&0X01)==0X01)  
	enable_T_K();
	else
	disable_T(); 
}

static void T_given_value_D(void)	      
{ 
	T_value=(*p_prog%0x100)/2;
	p_prog++;
	switch(*p_prog/0x100) 
	{ 
		case 0x86: T_value+=(*p_prog%0x100)*0x80;   break;
		case 0x88: T_value+=(*p_prog%0x100)*0x80+1000;   break; 
	}
	if((process_value&0X01)==0X01)  
	enable_T_D();
	else
	disable_T();
}

static void operation_T(void)
{
	T_number=*p_prog;       
	p_prog++;				  
	switch(*p_prog/0x100) 
	{
		case 0x80: T_given_value_K(),p_prog++;              break;  
		case 0x86: T_given_value_D(),p_prog++;              break;  
	}	
}

static void enable_C_K(void)	   
{
	u16 temp_bit,*p_C_enable_coil;
	C_value=*p_prog%0x100;           
	p_prog++;
	C_value+=(*p_prog%0x100)*0x100;  
	p_data=all_data+0x0500+C_number;
	temp_bit=1<<(C_number%0x10);
	if(*p_data<C_value)              
	{
		p_C_enable_coil=all_data+0x0270+(C_number/0X10);   
		if(!((*p_C_enable_coil&temp_bit)==temp_bit))
		*p_data+=1;
	}
	if(*p_data<C_value)  
	{ 
		p_data=all_data+0x00F0+(C_number/0x10);  
		*p_data&=~(1<<(C_number%0x10));
	}
	else
	{  
		p_data=all_data+0x00F0+(C_number/0x10);  
		*p_data|=(1<<(C_number%0x10));
	}
	OUT(0X270,C_number);
}

static void enable_C_D(void)	  
{
	u16 temp_bit,*p_C_enable_coil;
	C_value=all_data[0x2000+C_value];
	p_data=all_data+0x0500+C_number;
	temp_bit=1<<(C_number%0x10);
	if(*p_data<C_value)    
	{ 
		p_C_enable_coil=all_data+0x0270+(C_number/0X10);   
		if(!((*p_C_enable_coil&temp_bit)==temp_bit))
		*p_data+=1;
	}
	p_C_enable_coil=all_data+0x00F0+(C_number/0x10);  
	if(*p_data<C_value)  
	*p_C_enable_coil&=~temp_bit;
	else
	*p_C_enable_coil|=temp_bit;
	OUT(0X270,C_number);
}

static void disable_C_K(void)
{
	C_value=*p_prog%0x100;           
	p_prog++;
	C_value+=(*p_prog%0x100)*0x100;  
	p_data=all_data+0x0500+C_number;
	if(*p_data<C_value)  
	{ 
		p_data=all_data+0x00F0+(C_number/0x10);  
		*p_data&=~(1<<(C_number%0x10));
	}
	else
	{  
		p_data=all_data+0x00F0+(C_number/0x10);  
		*p_data|=(1<<(C_number%0x10));
	}
	OUT(0X270,C_number);
}

static void disable_C_D(void)	   
{
	u16 temp_bit,*p_C_enable_coil;
	C_value=all_data[0x2000+C_value];
	p_data=all_data+0x0500+C_number;
	temp_bit=1<<(C_number%0x10);
	p_C_enable_coil=all_data+0x00F0+(C_number/0x10);  
	if(*p_data<C_value)  
	*p_C_enable_coil&=~temp_bit;
	else
	*p_C_enable_coil|=temp_bit;
	OUT(0X270,C_number);
}

static void C_given_value_K(void)	      
{
	if((process_value&0X01)==0X01)          
	enable_C_K();				   
	else
	disable_C_K(); 
}

static void C_given_value_D(void)	      
{ 
	C_value=(*p_prog%0x100)/2;
	p_prog++;
	switch(*p_prog/0x100) 
	{ 
		case 0x86: C_value+=(*p_prog%0x100)*0x80;        break;
		case 0x88: C_value+=(*p_prog%0x100)*0x80+1000;   break; 
	}
	if((process_value&0X01)==0X01)        
	enable_C_D();
	else
	disable_C_D();
}

static void operation_C(void)
{
	C_number=*p_prog;       
	p_prog++;				  
	switch(*p_prog/0x100) 
	{ 
		case 0x80: C_given_value_K(),p_prog++;              break;  
		case 0x86: C_given_value_D(),p_prog++;              break;  
	}	
}

void find_p(void)   
{ 
	u16 temp;
	p_prog=x+0x5c/2-1;
	for(temp=0;temp<7999;temp++)	   
	{ 
		if((*p_prog/0x100)==0xB0)
		prog_p_addr[*p_prog%0x100]=p_prog;
		p_prog++;
	}
}

void RST_Y(void)
{
	static u8 all_out_rst;
	if(all_out_rst>10)
	all_out_rst=0;
	if(p_all_data[0X01C4]==0x09)
	all_out_rst=0;
	else
	{	
		if(all_out_rst==0)
		all_out_rst++,all_data[0x180/2]=0;
	}
}
u8 find_toend(void)   
{ 
	u16 temp;
	p_prog=x+0x5c/2-2;
	temp=0;	
	do
	{ 
		p_prog++;
		temp++;
	}while((!(*p_prog==0x000f))&&(temp<7998)); 
	if(temp>7997)
	return 1;
	else		  
	return 0;
}

void PLC_ProComParse(void)		 //20151009
 { 
	u8  temp5,run_keep;
	static u8  puls;
	program_start_addr=x+0x5c/2-1;     
	run_keep=p_all_data[0X01C4];  
	Err = 1; 
	RST_Y(); 
	
	
	if(RUN_STOP==1)	  //MY PCB== 0	 AIR=1
	{
		if(Run_Flag==1)	 //RUN_STOP
		{
			Run_Flag = 0;
			run_keep=0x09;
		}
		
		
		if(run_keep==0x09)			  
		{
			Run=0;
			force_set(0X00E0,0);	 
			force_reset(0X00E0,1);	
			if(edit_prog==0x00)		 
			{ 
				find_p(),edit_prog=1;
				if(find_toend())
				{  
					p_all_data[0X01C4]=9;
					goto all_end;  
				}
			}
			if(puls==0x00)		 
			force_set(0X00E0,2),force_reset(0X00E0,3);
			p_prog=x+0x5c/2-1;
			all_data[0x070C]=20;        
			do
			{
				temp5=*p_prog/0x100;
				switch(temp5) 
					{ 
					case 0x06: operation_T();                           break;  
					case 0x0E: operation_C();                           break;  
					
          /* 操作S位元件所有的函数 */
					case 0x20: LD(0X0140,*p_prog),p_prog++;             break;  
					case 0x30: LDI(0X0140,*p_prog),p_prog++;            break;  
					case 0x40: AND(0X0140,*p_prog),p_prog++;            break;  
					case 0x50: ANI(0X0140,*p_prog),p_prog++;            break;  
					case 0x60: OR(0X0140,*p_prog),p_prog++;             break;  
					case 0x70: ORI(0X0140,*p_prog),p_prog++;            break;  
					
          /* 操作S位元件所有的函数 */
					case 0x21: LD(0X0150,*p_prog),p_prog++;             break;  
					case 0x31: LDI(0X0150,*p_prog),p_prog++;            break;  
					case 0x41: AND(0X0150,*p_prog),p_prog++;            break;  
					case 0x51: ANI(0X0150,*p_prog),p_prog++;            break;  
					case 0x61: OR(0X0150,*p_prog),p_prog++;             break;  
					case 0x71: ORI(0X0150,*p_prog),p_prog++;            break;  
					
          /* 操作S位元件所有的函数 */
					case 0x22: LD(0X0160,*p_prog),p_prog++;             break;  
					case 0x32: LDI(0X0160,*p_prog),p_prog++;            break;  
					case 0x42: AND(0X0160,*p_prog),p_prog++;            break;  
					case 0x52: ANI(0X0160,*p_prog),p_prog++;            break;  
					case 0x62: OR(0X0160,*p_prog),p_prog++;             break;  
					case 0x72: ORI(0X0160,*p_prog),p_prog++;            break;  
					
          /* 操作S位元件所有的函数 */
					case 0x23: LD(0X0170,*p_prog),p_prog++;             break;  
					case 0x33: LDI(0X0170,*p_prog),p_prog++;            break;  
					case 0x43: AND(0X0170,*p_prog),p_prog++;            break;  
					case 0x53: ANI(0X0170,*p_prog),p_prog++;            break;  
					case 0x63: OR(0X0170,*p_prog),p_prog++;             break;  
					case 0x73: ORI(0X0170,*p_prog),p_prog++;            break;  
					
					/* 操作X位元件所有的函数 */
					case 0x24: LD(0X0120,*p_prog),p_prog++;             break;  
					case 0x34: LDI(0X0120,*p_prog),p_prog++;            break;  
					case 0x44: AND(0X0120,*p_prog),p_prog++;            break;  
					case 0x54: ANI(0X0120,*p_prog),p_prog++;            break;  
					case 0x64: OR(0X0120,*p_prog),p_prog++;             break;  
					case 0x74: ORI(0X0120,*p_prog),p_prog++;            break;  
					
          /* 操作Y位元件所有的函数 */
					case 0x25: LD(0X00C0,*p_prog),p_prog++;             break;  
					case 0x35: LDI(0X00C0,*p_prog),p_prog++;            break;  
					case 0x45: AND(0X00C0,*p_prog),p_prog++;            break;
					case 0x55: ANI(0X00C0,*p_prog),p_prog++;            break;
					case 0x65: OR(0X00C0,*p_prog),p_prog++;             break;  
					case 0x75: ORI(0X00C0,*p_prog),p_prog++;            break;  
					case 0XC5: OUT(0X00C0,*p_prog),p_prog++;			      break;  
					case 0XD5: BIT_SET(0X00C0,*p_prog),p_prog++;	      break;  
					case 0XE5: RST(0X00C0,*p_prog),p_prog++;			      break;  
					
          /* 操作T位元件所有的函数 */
					case 0x26: LD(0X0100,*p_prog),p_prog++;             break;  
					case 0x36: LDI(0X0100,*p_prog),p_prog++;            break;  
					case 0x46: AND(0X0100,*p_prog),p_prog++;            break;
					case 0x56: ANI(0X0100,*p_prog),p_prog++;            break;
					case 0x66: OR(0X0100,*p_prog),p_prog++;             break;  
					case 0x76: ORI(0X0100,*p_prog),p_prog++;            break;  
					case 0XC6: OUT(0X0100,*p_prog),p_prog++;			      break;  
					
          /* 操作M0_255位元件所有的函数 */  
					case 0x28: LD(0X0000,*p_prog),p_prog++;             break;  
					case 0x38: LDI(0X0000,*p_prog),p_prog++;            break;  
					case 0x48: AND(0X0000,*p_prog),p_prog++;            break;
					case 0x58: ANI(0X0000,*p_prog),p_prog++;            break;
					case 0x68: OR(0X0000,*p_prog),p_prog++;             break;  
					case 0x78: ORI(0X0000,*p_prog),p_prog++;            break;  
					case 0XC8: OUT(0X0000,*p_prog),p_prog++;			      break;  
					case 0XD8: BIT_SET(0X0000,*p_prog),p_prog++;	      break;  
					case 0XE8: RST(0X0000,*p_prog),p_prog++;			      break;  
					
          /* 操作M256_511位元件所有的函数 */
					case 0x29: LD(0X0010,*p_prog),p_prog++;             break;  
					case 0x39: LDI(0X0010,*p_prog),p_prog++;            break;  
					case 0x49: AND(0X0010,*p_prog),p_prog++;            break;
					case 0x59: ANI(0X0010,*p_prog),p_prog++;            break;
					case 0x69: OR(0X0010,*p_prog),p_prog++;             break;  
					case 0x79: ORI(0X0010,*p_prog),p_prog++;            break;  
					case 0XC9: OUT(0X0010,*p_prog),p_prog++;			      break;  
					case 0XD9: BIT_SET(0X0010,*p_prog),p_prog++;	      break;  
					case 0XE9: RST(0X0010,*p_prog),p_prog++;			      break;  
					
          /* 操作M512_767位元件所有的函数 */
					case 0x2A: LD(0X0020,*p_prog),p_prog++;             break;  
					case 0x3A: LDI(0X0020,*p_prog),p_prog++;            break;  
					case 0x4A: AND(0X0020,*p_prog),p_prog++;            break;
					case 0x5A: ANI(0X0020,*p_prog),p_prog++;            break;
					case 0x6A: OR(0X0020,*p_prog),p_prog++;             break;  
					case 0x7A: ORI(0X0020,*p_prog),p_prog++;            break;  
					case 0XCA: OUT(0X0020,*p_prog),p_prog++;			      break;  
					case 0XDA: BIT_SET(0X0020,*p_prog),p_prog++;	      break;  
					case 0XEA: RST(0X0020,*p_prog),p_prog++;			      break;  
					
          /* 操作M768_1023位元件所有的函数 */
					case 0x2B: LD(0X0030,*p_prog),p_prog++;             break;  
					case 0x3B: LDI(0X0030,*p_prog),p_prog++;            break;  
					case 0x4B: AND(0X0030,*p_prog),p_prog++;            break;
					case 0x5B: ANI(0X0030,*p_prog),p_prog++;            break;
					case 0x6B: OR(0X0030,*p_prog),p_prog++;             break;  
					case 0x7B: ORI(0X0030,*p_prog),p_prog++;            break;  
					case 0XCB: OUT(0X0030,*p_prog),p_prog++;			      break;  
					case 0XDB: BIT_SET(0X0030,*p_prog),p_prog++;	      break;  
					case 0XEB: RST(0X0030,*p_prog),p_prog++;			      break;  
					
          /* 操作M1024_1279位元件所有的函数 */
					case 0x2C: LD(0X0040,*p_prog),p_prog++;             break;  
					case 0x3C: LDI(0X0040,*p_prog),p_prog++;            break;  
					case 0x4C: AND(0X0040,*p_prog),p_prog++;            break;
					case 0x5C: ANI(0X0040,*p_prog),p_prog++;            break;
					case 0x6C: OR(0X0040,*p_prog),p_prog++;             break;  
					case 0x7C: ORI(0X0040,*p_prog),p_prog++;            break;  
					case 0XCC: OUT(0X0040,*p_prog),p_prog++;			      break;  
					case 0XDC: BIT_SET(0X0040,*p_prog),p_prog++;	      break;  
					case 0XEC: RST(0X0040,*p_prog),p_prog++;			      break;  
					
          /* 操作M1280_1535位元件所有的函数 */
					case 0x2D: LD(0X0050,*p_prog),p_prog++;             break;  
					case 0x3D: LDI(0X0050,*p_prog),p_prog++;            break;  
					case 0x4D: AND(0X0050,*p_prog),p_prog++;            break;
					case 0x5D: ANI(0X0050,*p_prog),p_prog++;            break;
					case 0x6D: OR(0X0050,*p_prog),p_prog++;             break;  
					case 0x7D: ORI(0X0050,*p_prog),p_prog++;            break;  
					case 0XCD: OUT(0X0050,*p_prog),p_prog++;			      break;  
					case 0XDD: BIT_SET(0X0050,*p_prog),p_prog++;	      break;  
					case 0XED: RST(0X0050,*p_prog),p_prog++;			      break;  
					
          /* 操作C0-C255位元件所有的函数 */
					case 0x2E: LD(0X00F0,*p_prog),p_prog++;             break;  
					case 0x3E: LDI(0X00F0,*p_prog),p_prog++;            break;  
					case 0x4E: AND(0X00F0,*p_prog),p_prog++;            break;
					case 0x5E: ANI(0X00F0,*p_prog),p_prog++;            break;
					case 0x6E: OR(0X00F0,*p_prog),p_prog++;             break;  
					case 0x7E: ORI(0X00F0,*p_prog),p_prog++;            break;  
					
          /*m8000-m8255*/
					case 0x2F: LD(0X00E0,*p_prog),p_prog++;             break;  
					case 0x3F: LDI(0X00E0,*p_prog),p_prog++;            break;  
					case 0x4F: AND(0X00E0,*p_prog),p_prog++;            break;
					case 0x5F: ANI(0X00E0,*p_prog),p_prog++;            break;
					case 0x6F: OR(0X00E0,*p_prog),p_prog++;             break;  
					case 0x7F: ORI(0X00E0,*p_prog),p_prog++;            break;  
					case 0XCF: OUT(0X00E0,*p_prog),p_prog++;			      break;  
					case 0XDF: BIT_SET(0X00E0,*p_prog),p_prog++;	      break;  
					case 0XEF: RST(0X00E0,*p_prog),p_prog++;			      break;  
					
          //MPP,MPS
					case 0XFF: other_function(*p_prog),p_prog++;	      break;  
					
          //指针P标识
          case 0xB0: p_prog++;                                break;  
					
          //遇到0X001C为FEND,0X000F为END指令
          case 0x00: 
					{
						 if(((*p_prog%0x100)==0x1C)||((*p_prog%0x100)==0x0F)) 
							 goto all_end;
					}	
        
          //遇到不支持的命令 ,此处需要执行命令为16bit的指令						                    
					default:   extend_function();                       break;	
				}
			}while(1);
			all_end: p_prog=p_prog;
			puls=0x01;		 
			force_reset(0X00E0,2),force_set(0X00E0,3);// 初始化脉冲用到8002 8003			
		}
		else
		{ 
			RST_Y();               // 如果从运行壮态切换到停止壮态，需要清除Y输出
			force_reset(0X00E0,0); // 没有运行强制M80000为OFF 
			force_set(0X00E0,1);	 // 没有运行强制M80001为ON  
			all_data[0x070C]=0; 
			edit_prog=0;	         // 编程时要用到
			puls=0;                // 初始化脉冲用到8002 8003
			Run=1;
			RST_T_D_C_M_data();	  
		}
	}
	else
	{	
		RST_Y(); 
		force_reset(0X00E0,0);	 
		force_set(0X00E0,1);	    
		all_data[0x070C]=0; 
		edit_prog=0;	  
		puls=0;
		if(Run_Flag == 0)	//ADD
		{
			Run_Flag = 1;
		    RST_T_D_C_M_data();
		}		
		Run=1;
	}	
	all_data[0X701]=0X01;		
}

