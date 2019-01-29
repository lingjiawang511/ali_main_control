#ifndef __VIBRATION_H
#define __VIBRATION_H

#include "stm32f10x.h"

//baffle_inter 先定义为错误位置，baffle_outer定义为确认位置,BAFFLE_OFF失电是错误位置

#define				VIBRATION1_IO					 GPIO_Pin_8
#define				VIBRATION1_PORT			   GPIOC
#define				VIBRATION1_RCC				 RCC_APB2Periph_GPIOC
#define				VIBRATION2_IO					 GPIO_Pin_9
#define				VIBRATION2_PORT				 GPIOC
#define				VIBRATION2_RCC			   RCC_APB2Periph_GPIOC



#define READ_VIBRATION1_SENSOR    		GPIO_ReadInputDataBit(VIBRATION1_PORT,VIBRATION1_IO)
#define READ_VIBRATION2_SENSOR    		GPIO_ReadInputDataBit(VIBRATION2_PORT,VIBRATION2_IO)



void Vibration_GPIO_Config(void);
void Vibration_Control(void);
void Vibration_Time_Irq(void);
void Vibration_sensor_irq(void );
void Send_vibration_to_pc(u8 usart);
#endif 
