#ifndef __LGGATE_H
#define __LGGATE_H

#include "stm32f10x.h"

//baffle_inter 先定义为错误位置，baffle_outer定义为确认位置,BAFFLE_OFF失电是错误位置

#define				A_LEFT_GATE_IO					 GPIO_Pin_3
#define				A_LEFT_GATE_PORT			   GPIOC
#define				A_LEFT_GATE_RCC					 RCC_APB2Periph_GPIOC
#define				B_LEFT_GATE_IO					 GPIO_Pin_0
#define				B_LEFT_GATE_PORT				 GPIOB
#define				B_LEFT_GATE_RCC			  	 RCC_APB2Periph_GPIOB

#define				A_RIGHT_GATE_IO					 GPIO_Pin_1
#define				A_RIGHT_GATE_PORT			   GPIOB
#define				A_RIGHT_GATE_RCC				 RCC_APB2Periph_GPIOB
#define				B_RIGHT_GATE_IO					 GPIO_Pin_12
#define				B_RIGHT_GATE_PORT				 GPIOB
#define				B_RIGHT_GATE_RCC			   RCC_APB2Periph_GPIOB


#define       A_LEFT_GATE_SET   			GPIO_SetBits(A_LEFT_GATE_PORT, A_LEFT_GATE_IO)
#define       A_LEFT_GATE_RESET 	 	 	GPIO_ResetBits(A_LEFT_GATE_PORT,A_LEFT_GATE_IO)
#define       B_LEFT_GATE_SET   			GPIO_SetBits(B_LEFT_GATE_PORT, B_LEFT_GATE_IO)
#define       B_LEFT_GATE_RESET 	 	 	GPIO_ResetBits(B_LEFT_GATE_PORT,B_LEFT_GATE_IO)

#define       A_RIGHT_GATE_SET   			GPIO_SetBits(A_RIGHT_GATE_PORT, A_RIGHT_GATE_IO)
#define       A_RIGHT_GATE_RESET 	 	 	GPIO_ResetBits(A_RIGHT_GATE_PORT,A_RIGHT_GATE_IO)
#define       B_RIGHT_GATE_SET   			GPIO_SetBits(B_RIGHT_GATE_PORT, B_RIGHT_GATE_IO)
#define       B_RIGHT_GATE_RESET 	 	 	GPIO_ResetBits(B_RIGHT_GATE_PORT,B_RIGHT_GATE_IO)

#define      LEFT_GATE_OPEN  				do{A_LEFT_GATE_SET;B_LEFT_GATE_RESET;}while(0);	
#define      LEFT_GATE_CLOSE  			do{B_LEFT_GATE_SET;A_LEFT_GATE_RESET;}while(0);	

#define      RIGHT_GATE_OPEN  			do{A_RIGHT_GATE_SET;B_RIGHT_GATE_RESET;}while(0);	
#define      RIGHT_GATE_CLOSE  			do{B_RIGHT_GATE_SET;A_RIGHT_GATE_RESET;}while(0);	

void LGgate_GPIO_Config(void);
void LGgate_Control(void);
void LGgate_Time_Irq(void);
#endif 
