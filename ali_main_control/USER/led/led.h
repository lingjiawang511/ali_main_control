#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define				GLED_IO					GPIO_Pin_5
#define				GLED_PORT			  GPIOB
#define				GLED_RCC				RCC_APB2Periph_GPIOB
#define				RLED_IO					GPIO_Pin_1
#define				RLED_PORT				GPIOC
#define				RLED_RCC			  RCC_APB2Periph_GPIOC

#define       GLED_OFF   			GPIO_SetBits(GLED_PORT, GLED_IO)
#define       GLED_ON  				GPIO_ResetBits(GLED_PORT, GLED_IO)
#define       RLED_OFF   			GPIO_SetBits(RLED_PORT, RLED_IO	)
#define       RLED_ON 				GPIO_ResetBits(RLED_PORT, RLED_IO	)

extern u8 Gled_Num;
extern u8 Rled_Num;
extern u8 Gled_Light_Static;
extern u8 Rled_Light_Static;
void LED_GPIO_Config(void);
void Led_Flash(void);

#endif
