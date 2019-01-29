#include "HeadType.h"	

u8 send_vibration1_to_pc = 0;
u8 send_vibration2_to_pc = 0;

void Vibration_GPIO_Config(void)
{
	//定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(VIBRATION1_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = VIBRATION1_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(VIBRATION1_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(VIBRATION2_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = VIBRATION2_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(VIBRATION2_PORT, &GPIO_InitStructure);

}

void Vibration_sensor_irq(void )
{
	static u8 vibration1_filter = 0;
	static u16 vibration1_resetfilter = 0;
	static u8 vibration1_state = 0;
	static u8 vibration2_filter = 0;
	static u16 vibration2_resetfilter = 0;
	static u8 vibration2_state = 0;

	#define RESET_FILTER_TIME	  2000  //不震动10秒后再震动需要当做下一次震动
	
	if(READ_VIBRATION1_SENSOR == READLOW){
		if(vibration1_state == 0){
			vibration1_filter++;
		  if(vibration1_filter > 16){
				if(READ_VIBRATION1_SENSOR == READLOW){
					send_vibration1_to_pc = 1;
					vibration1_state = 1;
				}
				vibration1_filter = 0;
			}
		}
		vibration1_resetfilter = 0;
	}else{
		if(vibration1_state == 1){
			vibration1_resetfilter++;
			if(vibration1_resetfilter > RESET_FILTER_TIME){
				if(READ_VIBRATION1_SENSOR == READHIGH){
					vibration1_state = 0;
				}
				vibration1_resetfilter = 0;
			}
		}
		vibration1_filter = 0;
	}
	
	if(READ_VIBRATION2_SENSOR == READLOW){
		if(vibration2_state == 0){
			vibration2_filter++;
		  if(vibration2_filter > 16){
				if(READ_VIBRATION2_SENSOR == READLOW){
					send_vibration2_to_pc = 1;
					vibration2_state = 1;
				}
				vibration2_filter = 0;
			}
		}
		vibration2_resetfilter = 0;
	}else{
		if(vibration2_state == 1){
			vibration2_resetfilter++;
			if(vibration2_resetfilter > RESET_FILTER_TIME){
				if(READ_VIBRATION2_SENSOR == READHIGH){
					vibration2_state = 0;
				}
				vibration2_resetfilter = 0;
			}
		}
		vibration2_filter = 0;
	}
}


void Send_vibration_to_pc(u8 usart)
{
	 u16 crc;

		if((send_vibration1_to_pc ==1)||(send_vibration2_to_pc ==1) ){
// 			if(usart == SELECT_USART1){
// 					while(Usart1_Control_Data.tx_count!=0);
// 					Usart1_Control_Data.tx_count = 0;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x06;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x06;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x41;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					if(send_vibration1_to_pc == 1){
// 						Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x01;
// 						send_vibration1_to_pc = 0;
// 					}else{
// 						Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x02;
// 						send_vibration2_to_pc = 0;
// 					}
// 					crc=CRC_GetModbus16(Usart1_Control_Data.txbuf,Usart1_Control_Data.tx_count);
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =crc;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =crc>>8;
// 					Usart1_Control_Data.tx_index = 0;
// 					USART_SendData(USART1,Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_index++]);
// 			}else if(usart == SELECT_USART2){
					while(Usart2_Control_Data.tx_count!=0);
					Usart2_Control_Data.tx_count = 0;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] = 0x06;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x06;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x41;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
					if(send_vibration1_to_pc == 1){
						Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x01;
						send_vibration1_to_pc = 0;
					}else{
						Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x02;
						send_vibration2_to_pc = 0;
					}
					crc=CRC_GetModbus16(Usart2_Control_Data.txbuf,Usart2_Control_Data.tx_count);
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc>>8;
					Usart2_Control_Data.tx_index = 0;
					USART_SendData(USART2,Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_index++]);
// 			}			
		}		
}


