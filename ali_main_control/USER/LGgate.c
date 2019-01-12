#include "HeadType.h"	

LRgate_Work_Type Lgate,Rgate;
u16 auto_close_Lgate_time;
u16 auto_close_Rgate_time;
u8 send_getout_to_pc = 0;
u8 Lshipment_send_state = 0;
u8 Rshipment_send_state = 0;
u8 Lshipment_send_errstate = 0;
u8 Rshipment_send_errstate = 0;
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
void LRgetout_GPIO_Config(void)
{
	//定义一个GPIO_InitTypeDef 类型的结构体，名字叫GPIO_InitStructure 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LEFT_SHIPMENT_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = LEFT_SHIPMENT_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LEFT_SHIPMENT_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RIGHT_SHIPMENT_RCC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = RIGHT_SHIPMENT_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(RIGHT_SHIPMENT_PORT, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|GETOUT_MEDICINE_RCC,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GETOUT_MEDICINE_IO;		  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GETOUT_MEDICINE_PORT, &GPIO_InitStructure);	
	
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
	LRgetout_GPIO_Config();
	LEFT_GATE_RELEASE;
	RIGHT_GATE_RELEASE;
	Lgate.state = RESERVE;
	Rgate.state = RESERVE;
	auto_close_Rgate_time = 400;
	auto_close_Lgate_time = 400;
	Rgate.send_time = 2;
	Lgate.send_time = 2;
	Lgate.dir = GATELEFT;
	Lgate.action = LGOPEN;
	Lgate.state = READY;
	Rgate.dir = GATERIGHT;
	Rgate.action = LGOPEN;
	Rgate.state = READY;
}
#define LGGATE_TIMEOUT	1000 
#define ATUO_GATE_CLOSE_TIME	1000

void Lgate_Control(void)
{
// 	static u16 delay_time = 0;
	switch(Lgate.state){
	case RESERVE:	Lgate.actual_time = 0;	
								if((auto_close_Lgate_time == 0)&&(Lgate.Lactual_state == GATEOPEN)){
										Lgate.state = READY;
										Lgate.dir = GATELEFT;
										Lgate.action = LGCLOSE;
								}
								if((auto_close_Rgate_time == 0)&&(Lgate.Ractual_state == GATEOPEN)){
										Lgate.state = READY;
									  Lgate.dir = GATERIGHT;
										Lgate.action = LGCLOSE;
								}
								break;
	case READY:	
								 if(Lgate.dir == GATELEFT){//此处需要通讯控制方向和时间
										if(Lgate.action == LGOPEN){
												LEFT_GATE_OPEN;
											  Lgate.Lactual_state = GATEOPENNING;
										}else if(Lgate.action == LGCLOSE){
												LEFT_GATE_CLOSE;
											  Lgate.Lactual_state = GATECLOSING;
										}
									}else{
									}
									Lgate.actual_time = LGGATE_TIMEOUT;
									Lgate.state = WORKING;
								break ;	
	case WORKING://此处用定时器做超时处理，如果NS之后开关没有完成，则开关闸门出现故障
								if(Lgate.actual_time == 0){
										if(Lgate.Lactual_state == GATEOPENNING){
												Lgate.state = END;
											  LEFT_GATE_RELEASE;
												Lgate.Lactual_state = GATEERR;											
										}
								}
								break;

	case END:	
					 if(Lgate.Lactual_state == GATEOPEN){
// 							auto_close_Lgate_time = ATUO_GATE_CLOSE_TIME;
						 auto_close_Lgate_time = Lgate.send_time*200;
						  Lgate.state = RESERVE;
						}else{
								Lgate.state = RESERVE;
						}
								break ;	
	default :
								break;	
	}	
}
void Rgate_Control(void)
{
// 	static u16 delay_time = 0;
	switch(Rgate.state){
	case RESERVE:	Rgate.actual_time = 0;	
								if((auto_close_Rgate_time == 0)&&(Rgate.Ractual_state == GATEOPEN)){
										Rgate.state = READY;
									  Rgate.dir = GATERIGHT;
										Rgate.action = LGCLOSE;
								}
								break;
	case READY:	
									if(Rgate.dir == GATERIGHT){
										if(Rgate.action == LGOPEN){
												RIGHT_GATE_OPEN;
												Rgate.Ractual_state = GATEOPENNING;
									}else if(Rgate.action == LGCLOSE){
												RIGHT_GATE_CLOSE;
												Rgate.Ractual_state = GATECLOSING;
										}
									}
									Rgate.actual_time = LGGATE_TIMEOUT;
									Rgate.state = WORKING;
								break ;	
	case WORKING://此处用定时器做超时处理，如果NS之后开关没有完成，则开关闸门出现故障
								if(Rgate.actual_time == 0){
										if(Rgate.Ractual_state == GATEOPENNING){
												Rgate.state = END;
											  RIGHT_GATE_RELEASE;
												Rgate.Ractual_state = GATEERR;											
										}
								}	
								break;

	case END:	
						if(Rgate.Ractual_state == GATEOPEN){
// 							  auto_close_Rgate_time = ATUO_GATE_CLOSE_TIME;
								auto_close_Rgate_time = Rgate.send_time * 200;
								Rgate.state = RESERVE;
						}else{
								Rgate.state = RESERVE;
						}
								break ;	
	default :
								break;	
	}	
}

void LRgate_Control(void)
{
	Lgate_Control();
	Rgate_Control();
}

void LRgate_sensor_irq(void )
{
	static u8 getout_filter = 0;
	static u8 getout_resetfilter = 0;
	static u8 getout_state = 0;
	static u8 Lshiment_filter = 0;
	static u8 Rshiment_filter = 0;
	static u8 Lshimen_state = 0;
	static u8 Rshimen_state = 0;
	static u8 Lshiment_errfilter = 0;
	static u8 Rshiment_errfilter = 0;	
	static u8 Lshimen_errstate = 0;
	static u8 Rshimen_errstate = 0;
	if(Lgate.Lactual_state == GATEOPENNING){
		 if(READ_LOPEN_SENSOR == READHIGH){
			 LEFT_GATE_RELEASE;
			 Lgate.Lactual_state = GATEOPEN;
			 Lgate.state = END;
		}
		Lshimen_errstate = 0;
	}
	if(Lgate.Lactual_state == GATECLOSING){
		 if(READ_LCLOSE_SENSOR == READHIGH){
			 LEFT_GATE_RELEASE;
			 Lgate.Lactual_state = GATECLOSE;
			 Lgate.state = END;
		}
		Lshimen_state =  0;
	}
	
	if(Rgate.Ractual_state == GATEOPENNING){
		 if(READ_ROPEN_SENSOR == READHIGH){
				RIGHT_GATE_RELEASE;
			  Rgate.Ractual_state = GATEOPEN;
			  Rgate.state = END;
		}
		Rshimen_errstate = 0;
	}
	if(Rgate.Ractual_state == GATECLOSING){
		 if(READ_RCLOSE_SENSOR == READHIGH){
				RIGHT_GATE_RELEASE;
			  Rgate.Ractual_state = GATECLOSE;
			  Rgate.state = END;
		}
		Rshimen_state =  0;
	}	
	if(READ_LEFT_SHIPMENT_SENSOR == READHIGH){
		if((Lgate.Lactual_state == (u8)GATEOPEN)||(Lgate.Lactual_state == (u8)GATEOPENNING)){
			Lshiment_filter++;
			if((Lshiment_filter >=2)&&(Lshimen_state == 0)){
				Lshipment_send_state = 1;
				Lshiment_filter = 0;
				Lshimen_state = 1;
			}
		}else if(Lgate.Lactual_state == (u8)GATECLOSING){ //关门的时候有药需要处理
				Lshiment_errfilter++;
				if((Lshiment_errfilter >=2)&&(Lshimen_errstate == 0)){
					Lshipment_send_errstate = 1;
					Lshiment_filter = 0;
					Lshimen_errstate = 1;
					LEFT_GATE_RELEASE;
				}
		}
	}else{
		Lshiment_filter = 0;
	}
	if(READ_RIGHT_SHIPMENT_SENSOR == READHIGH){
		if((Rgate.Ractual_state == (u8)GATEOPEN)||(Rgate.Ractual_state == (u8)GATEOPENNING)){
			Rshiment_filter++;
			if((Rshiment_filter >=2)&&(Rshimen_state == 0)){
				Rshipment_send_state = 1;
				Rshiment_filter = 0;
				Rshimen_state = 1;
			}
		}else if(Rgate.Ractual_state == (u8)GATECLOSING){
				Rshiment_errfilter++;
				if((Rshiment_errfilter >=2)&&(Rshimen_errstate == 0)){
					Rshipment_send_errstate = 1;
					Rshiment_filter = 0;
					Rshimen_errstate = 1;
					RIGHT_GATE_RELEASE;					
				}			
		}
	}else{
		Rshiment_filter = 0;
	}
  if(auto_close_Lgate_time >0){
		auto_close_Lgate_time--;
	}
  if(auto_close_Rgate_time >0){
		auto_close_Rgate_time--;
	}	
	
	if(READ_GETOUT_MEDICINE_SENSOR == READLOW){
		if(getout_state == 0){
			getout_filter++;
		  if(getout_filter > 20){
				if(READ_GETOUT_MEDICINE_SENSOR == READLOW){
					send_getout_to_pc = 1;
					getout_state = 1;
				}
				getout_filter = 0;
			}
		}
	}else{
		if(getout_state == 1){
			getout_resetfilter++;
			if(getout_resetfilter > 20){
				if(READ_GETOUT_MEDICINE_SENSOR == READHIGH){
					getout_state = 0;
				}
				getout_resetfilter = 0;
			}
		}
	}
}




