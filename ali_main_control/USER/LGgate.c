#include "HeadType.h"	

LGgate_Work_Type lggate;

//=============================================================================
//��������:LGgate_GPIO_Config
//���ܸ�Ҫ:��ҩ��1�����������
//��������:��
//��������:��
//ע��    :��
//=============================================================================
 void LGgate_GPIO_Config(void)
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
								  if(lggate.lgdir == LGLEFT){//�˴���ҪͨѶ���Ʒ����ʱ��
										LEFT_GATE_OPEN;
									}else if(lggate.lgdir == LGRIGHT){
										RIGHT_GATE_OPEN;
									}
									lggate.state = WORKING;
									delay_time = 0;
							}
								break ;	
	case WORKING:if(LGstart_Ok == 1){
									delay_time++; //����������Ƶ���ʱ����
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



