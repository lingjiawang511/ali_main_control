#include"HeadType.h"

MCU_State_Type MCU_State;
Answer_Type 	 PC_Answer;
COMM_Rec_Union_Type  MCU_Host_Rec;
COMM_Send_Union_Type  MCU_Host_Send;

COMM_SlaveRec_Union_Type  	MCU_Rec;
COMM_SlaveSend_Union_Type 	MCU_Send;
// COMM_HostSend_Union_Type 		Group1_Send,Group2_Send;
// COMM_HostRec_Union_Type  		Group1_Rec,Group2_Rec;
Group_COMM_Type Group1,Group2;
u16 Group_Check_Time = 40;  //200ms
//=============================================================================
//��������:SLAVE_Rec_Comm
//���ܸ�Ҫ:PC��ΪͨѶ����ʱ���յĿ����ִ�����Ӧ
//����˵��:��
//��������:��
//ע��    :��
//=============================================================================
static u8  SLAVE_Rec_Comm(void)
{
	u8 i,res;
	u16 crc;
	if(Usart1_Control_Data.rx_count < 18){
		res = 2;
		return res;
	}crc=CRC_GetCCITT(Usart1_Control_Data.rxbuf,Usart1_Control_Data.rx_count-4);//֡����β����У��
	if((Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-3]+\
 	    Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-4]*256 == crc)){	    
		for(i = 0;i < 18;i++){
            MCU_Host_Rec.rec_buf[i] = Usart1_Control_Data.rxbuf[i];
        }//�����ݸ��Ƹ�����ͨѶ�ṹ��
		switch(MCU_Host_Rec.control.scanner_result){
		case 0x00: 
							break;
// 		case 0x01:Baffle_Control.scanner_result_old=Baffle_Control.PC_send_scanner_result = MCU_Host_Rec.control.scanner_result;
// 							break;
// 		case 0x02:Baffle_Control.scanner_result_old=Baffle_Control.PC_send_scanner_result = MCU_Host_Rec.control.scanner_result;
// 							break;
		default :
							break;
		}
		switch(MCU_Host_Rec.control.device_control){
		case 0x00:
							break;
		case 0x01: 			if(Device_State == 3){
											Device_State = 3;
										}else{
											if(READ_UPPER_REACH==0){
												Device_State = 1; 
											}else{
												Device_State = 3;
											}
										}
							break;
		case 0x02:
							Device_State = 2;
							break;
		default :
							break;
		}
		if(Device_State == 2){//�豸ֹͣ״̬�²ſ����ֶ�����
				switch(MCU_Host_Rec.control.baffle_control){
				case 0x00:	 
									break;
				case 0x01:BAFFLE_INTER;		
									break;
				case 0x02:BAFFLE_OUTER;
									break;
				default :	
									break;
				}
		}
		switch(MCU_Host_Rec.control.printer_fix){
		case 0x00:	 
							break;
// 		case 0x01:Air_Control.PC_send_delay_time = (MCU_Host_Rec.control.printer_delay_H*256+MCU_Host_Rec.control.printer_delay_L+2)/5;//��λ������ʱ����1ms����Ƭ����׽��ʱ����5MS����һ��������Ч
// 							if(Air_Control.PC_send_delay_time >= 600){
// 									Air_Control.PC_send_delay_time = 600;
// 							}else if(Air_Control.PC_send_delay_time <= 200){
// 									Air_Control.PC_send_delay_time = 200;
// 							}
// 							break;
		case 0x02:break;
		default : break;
		}
		switch(MCU_Host_Rec.control.baffle_fix){
		case 0x00:	 
							break;
// 		case 0x01:Baffle_Control.PC_send_process_time = (MCU_Host_Rec.control.baffle_delay_H*256+MCU_Host_Rec.control.baffle_delay_L+2)/5;
// 							if(Baffle_Control.PC_send_process_time >= 400){
// 									Baffle_Control.PC_send_process_time = 300;
// 							}else if(Baffle_Control.PC_send_process_time <= 200){
// 									Baffle_Control.PC_send_process_time = 300;
// 							}
// 							break;
		default : break;
		}
		switch(MCU_Host_Rec.control.check_state){
		case 0x00:	
							break;
// 		case 0x01: 
// 		           MCU_Host_Send.control.crc_result = 1;
// 							 MCU_Host_Send.control.scanner_result = Baffle_Control.scanner_result_old;
// 							 MCU_Host_Send.control.device_state = Device_State;
// 							 if(Printer.input_state == 1){
// 									MCU_Host_Send.control.printer_state = 1;
// 							 }else{
//                   MCU_Host_Send.control.printer_state = 2;
// 							}
// 							if((Baffle_Control.bag_input_flag == 1)||(Control.scanner.state == 1)){
// 								if(Baffle_Control.PC_send_scanner_result==1){
// 									MCU_Host_Send.control.recom_state = 1;
// 								}else{
// 									MCU_Host_Send.control.recom_state = 2;
// 								}
// 							}else{
// 								if(Baffle_Control.PC_send_scanner_result >=1){
// 									MCU_Host_Send.control.recom_state = 3;
// 								}else{
// 									MCU_Host_Send.control.recom_state = 0;
// 								}
// 							}
// 							break;
		case 0x02:
							break;
		default : 
							break;
		}		
			res = 0;	
	}else{
		 res = 1;
	}
	Usart1_Control_Data.tx_count = 0;
	Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x01;
	Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x58;
	Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
	Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x06;
	if(res == 0)//���յ�������ȷ
	{
		 if(MCU_Host_Rec.control.check_state == 1){
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x01;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = MCU_Host_Send.control.device_state;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = MCU_Host_Send.control.printer_state;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = MCU_Host_Send.control.recom_state;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = MCU_Host_Send.control.err_message;
		 }else{
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x01;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = MCU_Host_Rec.control.scanner_result;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
		}
			crc=CRC_GetCCITT(Usart1_Control_Data.txbuf,Usart1_Control_Data.tx_count);
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = (crc>>8)&0xFF; ;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = crc&0xFF;	
	}else if(res == 1){//CRCУ�����
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x02;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
		  Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
		  Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
		  Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
		  Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x00;
			crc=CRC_GetCCITT(Usart1_Control_Data.txbuf,Usart1_Control_Data.tx_count);
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = (crc>>8)&0xFF; ;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = crc&0xFF;
	}
	Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x0D;
	Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x0A;
	return res;
}
//=============================================================================
//��������:SLAVE_Rec_Comm
//���ܸ�Ҫ:PC��ΪͨѶ�ӻ�ʱ���յ����ݣ�������
//����˵��:��
//��������:��
//ע��    :�����������λ��ߵȴ����ճ�ʱ�������ٷ��ͣ�����״̬������ת��Ϊ�ӻ�ģʽ
//=============================================================================
//static u8  Host_Rec_Comm(void)
//{
//  	u8 i,res;
//	u16 crc;
//	if(Usart1_Control_Data.rx_count != 10){
//		res = 2;
//		return res;
//	}crc=CRC_GetCCITT(Usart1_Control_Data.rxbuf,Usart1_Control_Data.rx_count-4);
//	if((Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-3]+\
// 	    Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-4]*256 == crc)){	    
//		for(i = 0;i < 10;i++){
//            MCU_Host_Rec.rec_buf[i] = Usart1_Control_Data.rxbuf[i];
//        }//�����ݸ��Ƹ�����ͨѶ�ṹ��
//		if(MCU_Host_Rec.control.comm_state == 1){//PC��Ӧ����ȷ����������
//				res = 0;	
//		}else{
//				res = 3;
//		}
//	}else{
//		 res = 1;
//	}
//	return res;



//}
static void response_pc_control(u8 usart,u8 *prdata,u16 reason,u8 MCUstate)
{
	 u16 crc;
	if(usart == 1){
		  while(Usart1_Control_Data.tx_count!=0);
			Usart1_Control_Data.tx_count = 0;
		  Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = *prdata++;
		  if(MCUstate == 0){
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = *prdata++;
			}else{
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x80 + *prdata++;
			}
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata++;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata++;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata++;
			if(MCUstate == 0){//CRCУ����ȷ�������������͵�����
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata++;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata;				
			}else if(MCUstate == 1){//CRC����
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0xFF;
			}else if(MCUstate ==2){//�������Ϸ���ʱ���ظ���λ��
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =reason >>8;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =reason;				
			}
			crc=CRC_GetModbus16(Usart1_Control_Data.txbuf,Usart1_Control_Data.tx_count);
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =crc;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =crc>>8;
			Usart1_Control_Data.tx_index = 0;
			USART_SendData(USART1,Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_index++]);
	}else if(usart == 2){
		  while(Usart2_Control_Data.tx_count!=0);
		  Usart2_Control_Data.tx_count = 0;
		  Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] = *prdata++;
		  if(MCUstate == 0){
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] = *prdata++;
			}else{
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x80 + *prdata++;
			}
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata++;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata++;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata++;
			if(MCUstate == 0){//CRCУ����ȷ�������������͵�����
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata++;
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata;				
			}else if(MCUstate == 1){//CRC����
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0xFF;
			}else if(MCUstate ==2){//�������Ϸ���ʱ���ظ���λ��
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =reason >>8;
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =reason;				
			}
			crc=CRC_GetModbus16(Usart2_Control_Data.txbuf,Usart2_Control_Data.tx_count);
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc>>8;
			Usart2_Control_Data.tx_index = 0;
			USART_SendData(USART2,Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_index++]);
	}
}
static void mcu_send_pc_status(u8 PCusart,u8 GRusart,u8 *prdata,u16 state,u8 result)
{
	 u16 crc;
	if(PCusart == 1){
		  while(Usart1_Control_Data.tx_count!=0);
			Usart1_Control_Data.tx_count = 0;
		  Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x06;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x06;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =GRusart - 2;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*(prdata+2);
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*(prdata+3);
			if(*(prdata+4) == 2){//����2��ʱ���ǵ�ָ��
// 				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*(prdata+4);
// 				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*(prdata+5);				
			}else {
					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*(prdata+5);
					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*(prdata+7);				
			}
			crc=CRC_GetModbus16(Usart1_Control_Data.txbuf,Usart1_Control_Data.tx_count);
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =crc;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =crc>>8;
			Usart1_Control_Data.tx_index = 0;
			USART_SendData(USART1,Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_index++]);
	}else if(PCusart == 2){
		  while(Usart2_Control_Data.tx_count!=0);
			Usart2_Control_Data.tx_count = 0;
		  Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] = 0x06;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x06;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =GRusart - 2;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*(prdata+2);
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*(prdata+3);
			if(*(prdata+4) == 2){//����2��ʱ���ǵ�ָ��
// 				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*(prdata+4);
// 				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*(prdata+5);				
			}else {
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*(prdata+5);
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*(prdata+7);				
			}
			crc=CRC_GetModbus16(Usart2_Control_Data.txbuf,Usart2_Control_Data.tx_count);
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc>>8;
			Usart2_Control_Data.tx_index = 0;
			USART_SendData(USART2,Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_index++]);
	}
}
static u8 group_send_medicine(u8 group,COMM_SlaveRec_Union_Type recdata)
{
	u16 crc;
	u8 i;
	if(group == 1){
		Usart3_Control_Data.tx_count = 0;
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = 0x04;
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = 0x06;
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = recdata.control.line;
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = recdata.control.colum;
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = recdata.control.command;	
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = recdata.control.medicine_num;	
    crc=CRC_GetModbus16(Usart3_Control_Data.txbuf,Usart3_Control_Data.tx_count);
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] =crc;
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] =crc>>8;
		for(i=0;i<Usart3_Control_Data.tx_count;i++){//������Ҫ���͸�ˮƽ������ݵ������У������յ���ҩ״̬ʱ����
			Group1.group_send[recdata.control.line -1][recdata.control.colum -1].send_buf[i] = Usart3_Control_Data.txbuf[i];
		}
		Group1.send_count++;
		Usart3_Control_Data.tx_index = 0;
		USART_SendData(USART3,Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_index++]);
		
	}else if(group == 2){
		Usart4_Control_Data.tx_count = 0;
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = 0x04;
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = 0x06;
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = recdata.control.line;
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = recdata.control.colum;
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = recdata.control.command;	
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = recdata.control.medicine_num;	
    crc=CRC_GetModbus16(Usart4_Control_Data.txbuf,Usart4_Control_Data.tx_count);
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] =crc;
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] =crc>>8;
		for(i=0;i<Usart4_Control_Data.tx_count;i++){//������Ҫ���͸�ˮƽ������ݵ������У������յ���ҩ״̬ʱ����
			Group2.group_send[recdata.control.line -1][recdata.control.colum -1].send_buf[i] = Usart4_Control_Data.txbuf[i];
		}		
		Group2.send_count++;
		Usart4_Control_Data.tx_index = 0;
		USART_SendData(UART4,Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_index++]);		
  }
	return 0;
}
static void resolve_host_command(u8 usart,COMM_SlaveRec_Union_Type recdata,u16 reason,u8 device_satate)
{
	 if(device_satate == 0){ //�豸OK��������ִ������
			switch(recdata.control.group_name){
				case 0://��������
					break;
				case 1:
					group_send_medicine(1,recdata);
					break;
				case 2:
					group_send_medicine(2,recdata);
					break;
				default:
					break;
			}
			response_pc_control(SELECT_USART1,recdata.rec_buf,0x00,0x00);
	}else{//�豸����
		response_pc_control(usart,recdata.rec_buf,reason,2);
	}
}
//=============================================================================
//��������:Execute_Host_Comm
//���ܸ�Ҫ:ִ����λ������������
//����˵��:��
//��������:��
//ע��    :��
//=============================================================================
static u8 Execute_Host_Comm(u8 usart)
{
	u8 res;
	u8 i;
	u16 crc;
	if(usart == SELECT_USART1){
		if((Usart1_Control_Data.rx_count < 9)||(Usart1_Control_Data.rxbuf[0] != 0x05)){
			res = 2;
			return res;
		}
		crc=CRC_GetModbus16(Usart1_Control_Data.rxbuf,Usart1_Control_Data.rx_count-2);//֡����β����У��
		if((Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-2]+\
				Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-1]*256 == crc)){	    
				for(i = 0;i < 9;i++){
							MCU_Rec.rec_buf[i] = Usart1_Control_Data.rxbuf[i];
				 }//�����ݸ��Ƹ�����ͨѶ�ṹ��
				resolve_host_command(SELECT_USART1, MCU_Rec,0x00,0x00);
				res = 0;
		}else{
				response_pc_control(SELECT_USART1,Usart1_Control_Data.rxbuf,0x00FF,1);
				res = 1;
		}
	}else if(usart == SELECT_USART2){
		if((Usart2_Control_Data.rx_count < 9)||(Usart2_Control_Data.rxbuf[0] != 0x05)){
			res = 2;
			return res;
		}
		crc=CRC_GetModbus16(Usart2_Control_Data.rxbuf,Usart2_Control_Data.rx_count-2);//֡����β����У��
		if((Usart2_Control_Data.rxbuf[Usart2_Control_Data.rx_count-2]+\
				Usart2_Control_Data.rxbuf[Usart2_Control_Data.rx_count-1]*256 == crc)){	    
				for(i = 0;i < 9;i++){
							MCU_Rec.rec_buf[i] = Usart2_Control_Data.rxbuf[i];
				 }//�����ݸ��Ƹ�����ͨѶ�ṹ��
				resolve_host_command(SELECT_USART2, MCU_Rec,0x00,0x00);
				res = 0;
		}else{
				response_pc_control(SELECT_USART2,Usart2_Control_Data.rxbuf,0x00FF,1);
				res = 1;
		}
	}
			return res;
}
void Group_Check_State(void)
{
	u8 i,j;
	u16 crc;
	if(Group_Check_Time == 0){
			if((Group1.send_count > 0)&&(Usart3_Control_Data.tx_count == 0)){
				for(i=0;i<GROUP_LINE_MAX;i++){
					for(j=0;j<GROUP_COLUM_MAX;j++){
						if((Group1.group_send[i][j].send_buf[3] != 0xFF)&&(Group1.group_send[i][j].send_buf[2] == (i+1))\
							&&(Group1.group_send[i][j].send_buf[3] == (j+1))){  //�е�ַ������0XFFʱ������ҩ����Ҫ���״̬
								Usart3_Control_Data.tx_count = 0;
								Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = 0x04;
								Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = 0x03;
								Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = Group1.group_send[i][j].control.line;
								Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = Group1.group_send[i][j].control.colum;
								Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = Group1.group_send[i][j].control.command;	
								Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = Group1.group_send[i][j].control.task;	
								crc=CRC_GetModbus16(Usart3_Control_Data.txbuf,Usart3_Control_Data.tx_count);
								Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] =crc;
								Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] =crc>>8;

								Usart3_Control_Data.tx_index = 0;
								USART_SendData(USART3,Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_index++]);	
								break;	
						}						
					}
				}
			}
			
			if((Group2.send_count > 0)&&(Usart4_Control_Data.tx_count == 0)){
				for(i=0;i<GROUP_LINE_MAX;i++){
					for(j=0;j<20;j++){
						if((Group2.group_send[i][j].send_buf[2] != 0xFF)&&(Group2.group_send[i][j].send_buf[2] == (i+1))\
							&&(Group2.group_send[i][j].send_buf[3] == (j+1))){  //�е�ַ������0XFFʱ������ҩ����Ҫ���״̬
								Usart4_Control_Data.tx_count = 0;
								Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = 0x04;
								Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = 0x03;
								Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = Group2.group_send[i][j].control.line;
								Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = Group2.group_send[i][j].control.colum;
								Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = Group2.group_send[i][j].control.command;	
								Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = Group2.group_send[i][j].control.task;	
								crc=CRC_GetModbus16(Usart4_Control_Data.txbuf,Usart4_Control_Data.tx_count);
								Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] =crc;
								Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] =crc>>8;

								Usart4_Control_Data.tx_index = 0;
								USART_SendData(UART4,Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_index++]);	
								break;				
						}
					}
				}
			}
		 Group_Check_Time = GROUP_CHECK_TIME;
	}
}
u8 Execute_level_Comm(u8 PCusart,u8 GRusart)
{
	u8 res;
	u8 i;
	u16 crc;
	if(GRusart == SELECT_USART3){
		if((Usart3_Control_Data.rx_count < 10)||(Usart3_Control_Data.rxbuf[0] != 0x04)){
			res = 2;
			return res;
		}
		crc=CRC_GetModbus16(Usart3_Control_Data.rxbuf,Usart3_Control_Data.rx_count-2);//֡����β����У��
		if((Usart3_Control_Data.rxbuf[Usart3_Control_Data.rx_count-2]+\
				Usart3_Control_Data.rxbuf[Usart3_Control_Data.rx_count-1]*256 == crc)){	 
        if(Usart3_Control_Data.rxbuf[1] == 0x03){				
						for(i = 0;i < 10;i++){
									Group1.group_rec[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].rec_buf[i] = Usart3_Control_Data.rxbuf[i];
						 }//�����ݸ��Ƹ�����ͨѶ�ṹ��
						 if(Group1.group_rec[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].feedback.command != 2){
							 switch(Group1.group_rec[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].feedback.result2){
									case 0x01://���ڷ�ҩ
										break;
									case 0x02:
									case 0x03:
									case 0x04:
										if(Group1.send_count == 2){
												Group1.send_count = 2;
											}
										if(Group1.send_count > 0){
											Group1.send_count--;
										}
// 										if(Group1.send_count == 0){ //�к��ж�Ҫ��ѯ��Ŀǰ������ܻ�û��ʵ�֣�Ӧ�÷ֿ��к���������
// 											Group1.group_send[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].send_buf[2] = 0xFF;
// 										}
										Group1.group_send[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].send_buf[3] = 0xFF;
										mcu_send_pc_status(PCusart,GRusart,Usart3_Control_Data.rxbuf,0x00,0x00);
										break;
									default:
										break;
							}
						}else{

						}
				 }else if(Usart3_Control_Data.rxbuf[1] == 0x06){
					 
				 }
				res = 0;
		}else{
// 				response_pc_control(SELECT_USART1,Usart1_Control_Data.rxbuf,0x00FF,1);
				res = 1;
		}
	}else if(GRusart == SELECT_USART4){
			if((Usart4_Control_Data.rx_count < 10)||(Usart4_Control_Data.rxbuf[0] != 0x04)){
			res = 2;
			return res;
		}
		crc=CRC_GetModbus16(Usart4_Control_Data.rxbuf,Usart4_Control_Data.rx_count-2);//֡����β����У��
		if((Usart4_Control_Data.rxbuf[Usart4_Control_Data.rx_count-2]+\
				Usart4_Control_Data.rxbuf[Usart4_Control_Data.rx_count-1]*256 == crc)){	 
        if(Usart4_Control_Data.rxbuf[1] == 0x03){				
						for(i = 0;i < 10;i++){
									Group2.group_rec[Usart4_Control_Data.rxbuf[2]-1][Usart4_Control_Data.rxbuf[3]-1].rec_buf[i] = Usart4_Control_Data.rxbuf[i];
						 }//�����ݸ��Ƹ�����ͨѶ�ṹ��
						 if(Group2.group_rec[Usart4_Control_Data.rxbuf[2]-1][Usart4_Control_Data.rxbuf[3]-1].feedback.command != 2){
							 switch(Group2.group_rec[Usart4_Control_Data.rxbuf[2]-1][Usart4_Control_Data.rxbuf[3]-1].feedback.result2){
									case 0x01://���ڷ�ҩ
										break;
									case 0x02:
									case 0x03:
									case 0x04:
										if(Group2.send_count > 0){
											Group2.send_count--;
										}
// 										Group2.group_send[Usart4_Control_Data.rxbuf[2]-1][Usart4_Control_Data.rxbuf[3]-1].send_buf[2] = 0xFF;
										Group2.group_send[Usart4_Control_Data.rxbuf[2]-1][Usart4_Control_Data.rxbuf[3]-1].send_buf[3] = 0xFF;
										mcu_send_pc_status(PCusart,GRusart,Usart4_Control_Data.rxbuf,0x00,0x00);
										break;
									default:
										break;
							}
						}else{

						}
				 }else if(Usart4_Control_Data.rxbuf[1] == 0x06){
					 
				 }
				res = 0;
		}else{
// 				response_pc_control(SELECT_USART1,Usart1_Control_Data.rxbuf,0x00FF,1);
				res = 1;
		}
	}
	return res;
}
//=============================================================================
//��������:Respond_Host_Comm
//���ܸ�Ҫ:��Ӧ��λ���ķ�����������������Ѿ��Ӵ���һ��������
//����˵��:��
//��������:��
//ע��    :��
//=============================================================================
void Communication_Process(void)
{
    if (1 == Usart1_Control_Data.rx_aframe){    
				Execute_Host_Comm(SELECT_USART1);		
				Usart1_Control_Data.rx_count = 0;
				Auto_Frame_Time1 = AUTO_FRAME_TIMEOUT1;
				Usart1_Control_Data.rx_aframe = 0;
    }
	  if (1 == Usart2_Control_Data.rx_aframe){    
				Execute_Host_Comm(SELECT_USART2);	
				Usart2_Control_Data.rx_count = 0;
				Auto_Frame_Time2 = AUTO_FRAME_TIMEOUT2;
				Usart2_Control_Data.rx_aframe = 0;
				
    }
		if (1 == Usart3_Control_Data.rx_aframe){    
	
				Execute_level_Comm(SELECT_USART1,SELECT_USART3);
				Usart3_Control_Data.rx_count = 0;
				Auto_Frame_Time3 = AUTO_FRAME_TIMEOUT3;
				Usart3_Control_Data.rx_aframe = 0;
			
    }
	  if (1 == Usart4_Control_Data.rx_aframe){    
				Execute_level_Comm(SELECT_USART1,SELECT_USART4);
				Usart4_Control_Data.rx_count = 0;
				Auto_Frame_Time4 = AUTO_FRAME_TIMEOUT4;
				Usart4_Control_Data.rx_aframe = 0;
    }
		Group_Check_State();
}

















