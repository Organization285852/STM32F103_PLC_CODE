#define BITBAND(addr, bitnum)    ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)           *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C  
#define GPIOE_ODR_Addr    (GPIOE_BASE+12)   
//IO模式定义
#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 


#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入 

//*********************

//输入X定义
#define X0  PEin(2)	
#define X1  PEin(3)
#define X2  PEin(4)
#define X3  PAin(6)
#define X4  PAin(7)		 
#define X5  PCin(4)
#define X6  PCin(5)
#define X7  PBin(0)

#define X10 PBin(1)		
#define X11 PBin(12)
#define X12 PBin(13)
#define X13 PBin(14)
#define X14 PBin(15)
//#define X15 PBin(7)
//#define X16 PBin(8)
//#define X17 PBin(9)

//输出Y定义
#define Y00 PBout(5)    
#define Y01 PEout(5)
#define Y02 PAout(11)
#define Y03 PAout(12)
#define Y04 PAout(11)   
#define Y05 PCout(10) 
#define Y06 PCout(11) 
#define Y07 PCout(12)

#define Y10 PDout(2 )
#define Y11 PBout(7)
#define Y12 PBout(7 )
#define Y13 PBout(5 )
#define Y14 PBout(6 )
#define Y15 PBout(7 )
//#define Y16 PAout(1 )
//#define Y17 PAout(0 )

#define Run PBout(8)  
#define Err PBout(9)
#define RUN_STOP PCin(13)
#define PVD PAin(0)           // GPIOA0

