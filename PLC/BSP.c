/********************************************************/
// CPU需要：STM32F103--RAM内存不小于64K	Flash内存不小于128K
// 本代码已在STM32F103RCT6 RDT6 VCT6 VET6测试通过
// 编辑日期：20150903
// editor by 传人记
/********************************************************/

#include "stm32f10x.h"
#include <OS_Config.h>
#include <stdio.h>
//#include <absacc.h> 
u16 in_x,out_y,M00;		
extern u16 all_data[16600] __attribute__((at(0x20005002)));
void y_refresh(void);
void BSP_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO ,  ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
//	y_refresh();
	
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable , ENABLE);  //失能（JTAG+SW-DP）释放I/O
	

	//define X00-X14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);		
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);			

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode =   GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
		
	//define Y00-Y15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
    //define EEPROM
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;	   //EEPROM
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

    //define OTHER IO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	       //SHUTDOWN
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	       //RUN/STOP_SWITCH
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;  //RUN_LED  ERR_LED
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7 |GPIO_Pin_8; //文本显示
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
  	
}

void x_filter(void)			//每　1ms  需调用一次，用于X滤波，暂定为20MS
  {	
     static signed char x_buffer[20];    //暂定刷新20个X 
 		           u8 temp=0;
	 
     if(!X0)
	 	   	{
			   if(x_buffer[temp]<10)
			      x_buffer[temp]++;  
			   else
			      in_x|=1;
			 }
	  else
			 {
			    if(x_buffer[temp]>-10)
			       x_buffer[temp]--;  
			    else
	 		       in_x&=~1;
			  }
	     temp++;
     if(!X1)
	    {if(x_buffer[temp]<10)
		   x_buffer[temp]++;  
		    else
	   	   in_x|=1<<temp;
		     }
		   else
		 {if(x_buffer[temp]>-10)
		   x_buffer[temp]--;  
		    else
		  in_x&=~(1<<temp);
		      }
		  temp++;
     if(!X2)
	   	 {if(x_buffer[temp]<10)
		   x_buffer[temp]++;  
		    else
	   	   in_x|=1<<temp;
		     }
		   else
		 {if(x_buffer[temp]>-10)
		   x_buffer[temp]--;  
		    else
		  in_x&=~(1<<temp);
		      }
		  temp++;		   
     if(!X3)
	   	 {if(x_buffer[temp]<10)
		   x_buffer[temp]++;  
		    else
	   	   in_x|=1<<temp;
		     }
		   else
		 {if(x_buffer[temp]>-10)
		   x_buffer[temp]--;  
		    else
		  in_x&=~(1<<temp);
		      }
		  temp++;

     if(!X4)
 	   	{if(x_buffer[temp]<10)
		   x_buffer[temp]++;  
		    else
		    in_x|=1<<temp;
		      }
		   else
		 {if(x_buffer[temp]>-10)
		   x_buffer[temp]--;  
		    else
 		    in_x&=~(1<<temp);
			  }
	     temp++;
     if(!X5)
	    {if(x_buffer[temp]<10)
		   x_buffer[temp]++;  
		    else
	   	   in_x|=1<<temp;
		     }
		   else
		 {if(x_buffer[temp]>-10)
		   x_buffer[temp]--;  
		    else
		  in_x&=~(1<<temp);
		      }
		  temp++;
     if(!X6)
	   	 {if(x_buffer[temp]<10)
		   x_buffer[temp]++;  
		    else
	   	   in_x|=1<<temp;
		     }
		   else
		 {if(x_buffer[temp]>-10)
		   x_buffer[temp]--;  
		    else
		  in_x&=~(1<<temp);
		      }
		  temp++;		   
     if(!X7)
	   	 {
		  if(x_buffer[temp]<10)
		     x_buffer[temp]++;  
		   else
	   	     in_x|=1<<temp;
		  }
    else
		 {
		   if(x_buffer[temp]>-10)
		      x_buffer[temp]--;  
		   else
		      in_x&=~(1<<temp);
		 }
		temp++;
		//*****************
     if(!X10)
	   	 {
		  if(x_buffer[temp]<10)
		     x_buffer[temp]++;  
		   else
	   	     in_x|=1<<temp;
		  }
    else
		 {
		   if(x_buffer[temp]>-10)
		      x_buffer[temp]--;  
		   else
		      in_x&=~(1<<temp);
		 }
		temp++;
     if(!X11)
	   	 {
		  if(x_buffer[temp]<10)
		     x_buffer[temp]++;  
		   else
	   	     in_x|=1<<temp;
		  }
    else
		 {
		   if(x_buffer[temp]>-10)
		      x_buffer[temp]--;  
		   else
		      in_x&=~(1<<temp);
		 }
		temp++;
     if(!X12)
	   	 {
		  if(x_buffer[temp]<10)
		     x_buffer[temp]++;  
		   else
	   	     in_x|=1<<temp;
		  }
    else
		 {
		   if(x_buffer[temp]>-10)
		      x_buffer[temp]--;  
		   else
		      in_x&=~(1<<temp);
		 }
		temp++;
    if(!X13)
	   	 {
		  if(x_buffer[temp]<10)
		     x_buffer[temp]++;  
		   else
	   	     in_x|=1<<temp;
		  }
    else
		 {
		   if(x_buffer[temp]>-10)
		      x_buffer[temp]--;  
		   else
		      in_x&=~(1<<temp);
		 }
		temp++;
//    if(!X14)
//	   	 {
//		  if(x_buffer[temp]<10)
//		     x_buffer[temp]++;  
//		   else
//	   	     in_x|=1<<temp;
//		  }
//    else
//		 {
//		   if(x_buffer[temp]>-10)
//		      x_buffer[temp]--;  
//		   else
//		      in_x&=~(1<<temp);
//		 }
//		temp++;
//    if(!X15)
//	   	 {
//		  if(x_buffer[temp]<10)
//		     x_buffer[temp]++;  
//		   else
//	   	     in_x|=1<<temp;
//		  }
//    else
//		 {
//		   if(x_buffer[temp]>-10)
//		      x_buffer[temp]--;  
//		   else
//		      in_x&=~(1<<temp);
//		 }
//		temp++;
//    if(!X16)
//	   	 {
//		  if(x_buffer[temp]<10)
//		     x_buffer[temp]++;  
//		   else
//	   	     in_x|=1<<temp;
//		  }
//    else
//		 {
//		   if(x_buffer[temp]>-10)
//		      x_buffer[temp]--;  
//		   else
//		      in_x&=~(1<<temp);
//		 }
//		temp++;
//    if(!X17)
//	   	 {
//		  if(x_buffer[temp]<10)
//		     x_buffer[temp]++;  
//		   else
//	   	     in_x|=1<<temp;
//		  }
//    else
//		 {
//		   if(x_buffer[temp]>-10)
//		      x_buffer[temp]--;  
//		   else
//		      in_x&=~(1<<temp);
//		 }
//		temp++;
 }

              
void x_refresh(void)    //刷新输入点共4点
 {      
	  all_data[0x240/2]&=0x0000;		 //清除有外部输入的X
	  all_data[0x240/2]|=in_x;			 //更新外部输入X数据
	  //CH-X00:all_data[0x240/2]----all_data[0x240/2+15]	   16组
	  //CH-Y00:all_data[0x180/2]----all_data[0x180/2+15]	   16组
	  all_data[0x2000+500]=M00++;		// D500
	  all_data[0x0500+100]=M00;			//C100
	  all_data[0x0800+100]=M00;			//T100
 }

//刷新输出，一个地址为16个点
void y_refresh(void)         
 {
 	out_y=all_data[0x180/2];

    if((out_y&0x01)==0x01)	  //
		{
			Y00=1;
		}
	  else
		{
			Y00=0;	
		}			
	if((out_y&0x02)==0x02)	  //
          Y01=1;
	    else
	      Y01=0;
	if((out_y&0x04)==0x04)	  //
          Y02=1;
	    else
	      Y02=0;
    if((out_y&0x08)==0x08)	  //
          Y03=1;
	    else
	      Y03=0;	 
	if((out_y&0x10)==0x10)	  //
          Y04=1;
	    else
	      Y04=0; 
	if((out_y&0x20)==0x20)	  //
          Y05=1;
	    else
	      Y05=0;
	if((out_y&0x40)==0x40)	  //
          Y06=1;
	    else
	      Y06=0;
    if((out_y&0x80)==0x80)	  //
          Y07=1;
	    else
	      Y07=0;

//	if((out_y&0x100)==0x100)	  //
//          Y10=0;
//	    else
//	      Y10=1;
//	if((out_y&0x200)==0x200)	  //
//          Y11=0;
//	    else
//	      Y11=1;
//	if((out_y&0x400)==0x400)	  //
//          Y12=0;
//	    else
//	      Y12=1;
//	if((out_y&0x800)==0x800)	  //
//          Y13=0;
//	    else
//	      Y13=1;
//	if((out_y&0x1000)==0x1000)	  //
//          Y14=0;
//	    else
//	      Y14=1;
//	if((out_y&0x2000)==0x2000)	  //
//          Y15=0;
//	    else
//	      Y15=1;
//	if((out_y&0x4000)==0x4000)	  //
//          Y16=0;
//	    else
//	      Y16=1;
//	if((out_y&0x8000)==0x8000)	  //
//          Y17=0;
//	    else
//	      Y17=1;		  	
     }
void init_xy(void)
{ 
  BSP_config();
  y_refresh();
}

