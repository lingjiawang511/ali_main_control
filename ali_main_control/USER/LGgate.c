#include "HeadType.h"	

LGgate_Work_Type lggate;

//=============================================================================
//函数名称:LGgate_GPIO_Config
//功能概要:发药口1相关引脚配置
//参数名称:无
//函数返回:无
//注意    :无
//=============================================================================
 void LGgate_GPIO_Config(void)
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
}
static u8 LGstart_Ok;
void LGgate_Control(void)
{
	static u16 delay_time = 0;
	switch(lggate.state){
	case RESERVE:	lggate.actual_time = 1000;							
								break;
	case READY:	if(LGstart_Ok == 0){

						   }else{
								  if(lggate.lgdir == LGLEFT){//此处需要通讯控制方向和时间
										LEFT_GATE_OPEN;
									}else if(lggate.lgdir == LGRIGHT){
										RIGHT_GATE_OPEN;
									}
									lggate.state = WORKING;
									delay_time = 0;
							}
								break ;	
	case WORKING:if(LGstart_Ok == 1){
									delay_time++; //将这个变量移到定时器中
									if(delay_time >= lggate.actual_time){
										lggate.state = END ;
										delay_time = 0;
									}
								}
								break;

	case END:	if(LGstart_Ok == 1){
								if(lggate.lgdir == LGLEFT){
									LEFT_GATE_CLOSE;
								}else if(lggate.lgdir == LGRIGHT){
									RIGHT_GATE_CLOSE;
								}								
						}else{
								belt.state = RESERVE;
						}
								break ;	
	default :
								break;	
	}	
}



