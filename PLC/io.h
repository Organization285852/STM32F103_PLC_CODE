#include "stm32f10x.h"
#ifndef DISP_GLOBAL
#define DISP_EXT
#else
#define DISP_EXT extern
#endif

void BSP_config(void);
void usart_init(u16);
void usart(u16);
void process_x(void);
void y_refresh(void);
void x_refresh(void);
void init_xy(void);

void PLC_ProComParse(void);

void TIM5_Init(void);
void NVIC_Configuration(void);
