#include"HeadType.h"	
#include "usart.h"
#include "TIM.h"
#include "sht1x.h"

Belt_Work_Type belt;
Printer_Type Printer;
Control_Input_Type Control;
tag_param param = {0};
void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);
unsigned int sht10_read_time;
void delay_ms1(u16 ms)
{
	u16  i;
	u32  j;
	for(i=ms;i>0;i--)
	{
		for(j=5000;j>0;j--)
		;
	}
}
void read_TEMP_RH(void)
{
	if(sht10_read_time == 0){
		if(fsm_cpl == (*(read_sensor_hook))(&param)) {
			sht10_read_time = SHT10_READ_TIME;
		}
	}
}

//配置时钟在target宏定义
int main(void)
{
		delay_init();
		Key_Light_Init();
		Belt_Config();
		LED_GPIO_Config();
		LRgate_GPIO_Config();
	  sensor_init_sht1x();
// 		EXTIX_Init();
    USART1_Config();
		USART2_Config();
		USART3_Config();
		USART4_Config();
    TIM2_Config();
    TIM3_Config();
		param_init();
		IWDG_Init(IWDG_Prescaler_128,625);    //与分频数为128,重载值为625,溢出时间为2s	
		delay_ms(100);	
    while(1){
			IWDG_Feed();
			Communication_Process();
			LRgate_Control();
			read_TEMP_RH();	
		}      
}

void param_init(void)
{
	Init_Group_Param();
}

void IWDG_Init(u8 prer,u16 rlr) 
{	
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作
	
	IWDG_SetPrescaler(prer);  //设置IWDG预分频值:设置IWDG预分频值为64
	
	IWDG_SetReload(rlr);  //设置IWDG重装载值
	
	IWDG_ReloadCounter();  //按照IWDG重装载寄存器的值重装载IWDG计数器
	
	IWDG_Enable();  //使能IWDG
}
//喂独立看门狗
void IWDG_Feed(void)
{   
 	IWDG_ReloadCounter();//reload										   
}


