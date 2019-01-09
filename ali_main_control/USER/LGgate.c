#include "HeadType.h"	

LRgate_Work_Type lrgate;
u16 auto_close_Lgate_time;
u16 auto_close_Rgate_time;

void LRsensor_GPIO_Config(void)
{
	//����һ��GPIO_InitTypeDef ���͵Ľṹ�壬���ֽ�GPIO_InitStructure 
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
//��������:LGgate_GPIO_Config
//���ܸ�Ҫ:��ҩ��1�����������
//��������:��
//��������:��
//ע��    :��
//=============================================================================
 void LRgate_GPIO_Config(void)
{
	//����һ��GPIO_InitTypeDef ���͵Ľṹ�壬���ֽ�GPIO_InitStructure 
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
								 if(lrgate.dir == GATELEFT){//�˴���ҪͨѶ���Ʒ����ʱ��
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
	case WORKING://�˴��ö�ʱ������ʱ�������NS֮�󿪹�û����ɣ��򿪹�բ�ų��ֹ���
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




