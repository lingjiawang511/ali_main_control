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

//����ʱ����target�궨��
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
		IWDG_Init(IWDG_Prescaler_128,625);    //���Ƶ��Ϊ128,����ֵΪ625,���ʱ��Ϊ2s	
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
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
	
	IWDG_SetPrescaler(prer);  //����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ64
	
	IWDG_SetReload(rlr);  //����IWDG��װ��ֵ
	
	IWDG_ReloadCounter();  //����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
	
	IWDG_Enable();  //ʹ��IWDG
}
//ι�������Ź�
void IWDG_Feed(void)
{   
 	IWDG_ReloadCounter();//reload										   
}


