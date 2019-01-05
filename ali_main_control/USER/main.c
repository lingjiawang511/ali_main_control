#include"HeadType.h"	
#include "usart.h"
#include "TIM.h"

Belt_Work_Type belt;
tag_param param = {0};
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
static void init_interface(void)
{
		delay_init();
// 		Key_Light_Init();
// 		Belt_Config();
// 		LED_GPIO_Config();
// 		EXTIX_Init();
// 		sensor_init_sht1x(0);
    USART1_Config();
// 		USART2_Config();
// 		USART3_Config();
// 		USART4_Config();
    TIM2_Config();
    TIM3_Config();
// 		param_init();	
}
//配置时钟在target宏定义
int main(void)
{
		init_interface();
// 		delay_ms(1000);
    while(1){
					Communication_Process();
//           if(fsm_cpl == (*(read_sensor_hook))(&param)) {
// 							delay_ms(10);
//             }
					}
// 					delay_ms(100);
        
}

// void param_init(void)
// {

// 	Air_Control.process = RESERVE;
// 	Air_Control.complete = 1;
// 	Air_Control.air_cylinder_position =IN_UP;

// 	Printer.complete = 0;
// 	Printer.process = PRINTER_RESERVE;
// 	PRINTER_START_OFF;
//   PRINTER_RESTART_OFF;
// 	
// 	Control.upper_reach.state = 0;
// 	Control.scanner.state = 0;
// 	Control.fit_reach.state = 0;
// 	Control.fluid_bag.state = 0;
// 	Printer.color_end.state = 0;
// 	Printer.color_less.state = 0;
// 	Printer.end.state = 0;
// 	Printer.err.state = 0;
// 	Printer.pinline.state = 0;
// 	Printer.tag_end.state = 0;
// 	Printer.input_state = 0;
// 	MCU_Host_Send.control.err_message = 0;
// 	Baffle_Control.process_time = Baffle_Control.PC_send_process_time;
//   MCU_Host_Send.control.recom_state = 0;
// 	Baffle_Control.PC_send_scanner_result = 0;
// 	Baffle_Control.scanner_result_old = 0;
// 	Update_Baffle_State(1);
// 	baffle_err_timeout = BAFFLE_ERR_TIMEOUT;
// }


