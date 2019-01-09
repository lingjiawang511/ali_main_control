#include "HeadType.h"	

LRgate_Work_Type lrgate;
u16 auto_close_Lgate_time;
u16 auto_close_Rgate_time;

void LRsensor_GPIO_Config(void)
{
	//定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LEFT_OPEN_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = LEFT_OPEN_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LEFT_OPEN_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(LEFT_CLOSE_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = LEFT_CLOSE_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LEFT_CLOSE_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RIGHT_OPEN_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = RIGHT_OPEN_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(RIGHT_OPEN_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RIGHT_CLOSE_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = RIGHT_CLOSE_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(RIGHT_CLOSE_PORT, &GPIO_InitStructure);
	
}
//=============================================================================
//函数名称:LGgate_GPIO_Config
//功能概要:发药口1相关引脚配置
//参数名称:无
//函数返回:无
//注意    :无
//=============================================================================
 void LRgate_GPIO_Config(void)
{
	//定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(A_LEFT_GATE_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = A_LEFT_GATE_IO;		 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(A_LEFT_GATE_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(B_LEFT_GATE_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = B_LEFT_GATE_IO;		 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(B_LEFT_GATE_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(A_RIGHT_GATE_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = A_RIGHT_GATE_IO;		 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(A_RIGHT_GATE_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(B_RIGHT_GATE_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = B_RIGHT_GATE_IO;		 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(B_RIGHT_GATE_PORT, &GPIO_InitStructure);
	
	LRsensor_GPIO_Config();
	LEFT_GATE_RELEASE;
	RIGHT_GATE_RELEASE;
	lrgate.state = RESERVE;
}
#define LGGATE_TIMEOUT	1000 
#define ATUO_GATE_CLOSE_TIME	1000
void LRgate_Control(void)
{
// 	static u16 delay_time = 0;
	switch(lrgate.state){
	case RESERVE:	lrgate.actual_time = 0;	
								if((auto_close_Lgate_time == 0)&&(lrgate.Lactual_state == GATEOPEN)){
										lrgate.state = READY;
										lrgate.dir = GATELEFT;
										lrgate.action = LGCLOSE;
								}
								if((auto_close_Rgate_time == 0)&&(lrgate.Ractual_state == GATEOPEN)){
										lrgate.state = READY;
									  lrgate.dir = GATERIGHT;
										lrgate.action = LGCLOSE;
								}
								break;
	case READY:	
								 if(lrgate.dir == GATELEFT){//此处需要通讯控制方向和时间
										if(lrgate.action == LGOPEN){
												LEFT_GATE_OPEN;
											  lrgate.Lactual_state = GATEOPENNING;
										}else if(lrgate.action == LGCLOSE){
												LEFT_GATE_CLOSE;
											  lrgate.Lactual_state = GATECLOSING;
										}
									}else{
										if(lrgate.action == LGOPEN){
												RIGHT_GATE_OPEN;
												lrgate.Ractual_state = GATEOPENNING;
										}else if(lrgate.action == LGCLOSE){
												RIGHT_GATE_CLOSE;
												lrgate.Ractual_state = GATECLOSING;
										}
									}
									lrgate.actual_time = LGGATE_TIMEOUT;
									lrgate.state = WORKING;
								break ;	
	case WORKING://此处用定时器做超时处理，如果NS之后开关没有完成，则开关闸门出现故障
								if(lrgate.actual_time == 0){
										if(lrgate.Lactual_state == GATEOPENNING){
												lrgate.state = END;
											  LEFT_GATE_RELEASE;
												lrgate.Lactual_state = GATEERR;											
										}
								}
								if(lrgate.actual_time == 0){
										if(lrgate.Ractual_state == GATEOPENNING){
												lrgate.state = END;
											  RIGHT_GATE_RELEASE;
												lrgate.Ractual_state = GATEERR;											
										}
								}	
								break;

	case END:	
					 if(lrgate.Lactual_state == GATEOPEN){
							auto_close_Lgate_time = ATUO_GATE_CLOSE_TIME;
						  lrgate.state = RESERVE;
						}else if(lrgate.Ractual_state == GATEOPEN){
							  auto_close_Rgate_time = ATUO_GATE_CLOSE_TIME;
								lrgate.state = RESERVE;
						}else{
								lrgate.state = RESERVE;
						}
								break ;	
	default :
								break;	
	}	
}

void LRgate_sensor_irq(void )
{
	if(lrgate.Lactual_state == GATEOPENNING){
		 if(READ_LOPEN_SENSOR == READHIGH){
				LEFT_GATE_RELEASE;
			 lrgate.Lactual_state = GATEOPEN;
			 lrgate.state = END;
		}
	}
	if(lrgate.Lactual_state == GATECLOSING){
		 if(READ_LCLOSE_SENSOR == READHIGH){
				LEFT_GATE_RELEASE;
			 lrgate.Lactual_state = GATECLOSE;
			 lrgate.state = END;
		}
	}
	
	if(lrgate.Ractual_state == GATEOPENNING){
		 if(READ_ROPEN_SENSOR == READHIGH){
				RIGHT_GATE_RELEASE;
			  lrgate.Ractual_state = GATEOPEN;
			  lrgate.state = END;
		}
	}
	if(lrgate.Ractual_state == GATECLOSING){
		 if(READ_RCLOSE_SENSOR == READHIGH){
				RIGHT_GATE_RELEASE;
			  lrgate.Ractual_state = GATECLOSE;
			  lrgate.state = END;
		}
	}	
  if(auto_close_Lgate_time >0){
		auto_close_Lgate_time--;
	}
  if(auto_close_Rgate_time >0){
		auto_close_Rgate_time--;
	}	
}




