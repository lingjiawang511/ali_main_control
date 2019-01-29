#include"HeadType.h"
#include "sht1x.h"

MCU_State_Type MCU_State;
Answer_Type 	 PC_Answer;
COMM_Rec_Union_Type  MCU_Host_Rec;
COMM_Send_Union_Type  MCU_Host_Send;

COMM_SlaveRec_Union_Type  	MCU_Rec;
COMM_SlaveSend_Union_Type 	MCU_Send;
// COMM_HostSend_Union_Type 		Group1_Send,Group2_Send;
// COMM_HostRec_Union_Type  		Group1_Rec,Group2_Rec;
Group_COMM_Type Group1,Group2;
u16 Group1_Check_Time = 40;  //200ms
u16 Group2_Check_Time = 40;  //200ms
static u8 pc_use_usart = 1;
extern u8 send_getout_to_pc;
//=============================================================================
//函数名称:SLAVE_Rec_Comm
//功能概要:PC作为通讯主机时接收的控制字处理并响应
//参数说明:无
//函数返回:无
//注意    :无
//=============================================================================
static u8  SLAVE_Rec_Comm(void)
{
	u8 i,res;
	u16 crc;
	if(Usart1_Control_Data.rx_count < 18){
		res = 2;
		return res;
	}crc=CRC_GetCCITT(Usart1_Control_Data.rxbuf,Usart1_Control_Data.rx_count-4);//帧结束尾不做校验
	if((Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-3]+\
 	    Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-4]*256 == crc)){	    
		for(i = 0;i < 18;i++){
            MCU_Host_Rec.rec_buf[i] = Usart1_Control_Data.rxbuf[i];
        }//把数据复制给主机通讯结构体
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
		if(Device_State == 2){//设备停止状态下才可以手动翻板
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
// 		case 0x01:Air_Control.PC_send_delay_time = (MCU_Host_Rec.control.printer_delay_H*256+MCU_Host_Rec.control.printer_delay_L+2)/5;//上位机发的时间是1ms，单片机捕捉的时间是5MS，下一个周期生效
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
	if(res == 0)//接收的数据正确
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
	}else if(res == 1){//CRC校验错误
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
//函数名称:SLAVE_Rec_Comm
//功能概要:PC作为通讯从机时接收的数据，并处理
//参数说明:无
//函数返回:无
//注意    :连续发送三次或者等待接收超时，将不再发送，并且状态机重新转化为从机模式
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
//        }//把数据复制给主机通讯结构体
//		if(MCU_Host_Rec.control.comm_state == 1){//PC机应答正确接收了数据
//				res = 0;	
//		}else{
//				res = 3;
//		}
//	}else{
//		 res = 1;
//	}
//	return res;



//}
void Init_Group_Param(void)
{
	u8 i,j;
	Group1.send_count = 0;
	for(i=0;i<GROUP_LINE_MAX;i++){
		for(j=0;j<GROUP_COLUM_MAX;j++){
			Group1.group_send[i][j].send_buf[2] = 0xFF;
			Group1.group_send[i][j].send_buf[3] = 0xFF;	
		}
	}
	Group2.send_count = 0;
	for(i=0;i<GROUP_LINE_MAX;i++){
		for(j=0;j<GROUP_COLUM_MAX;j++){
			Group2.group_send[i][j].send_buf[2] = 0xFF;
			Group2.group_send[i][j].send_buf[3] = 0xFF;	
		}
	}
}
static void response_pc_control(u8 usart,u8 *prdata,u16 reason,u8 MCUstate)
{
	 u16 crc;
	if(usart == 1){
		  while(Usart1_Control_Data.tx_count!=0);
			Usart1_Control_Data.tx_count = 0;
		  Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = *prdata++;
		  if(MCUstate != 1){
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = *prdata++;
			}else{
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x80 + *prdata++;
			}
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata++;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata++;
			Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata++;
			if(MCUstate == 0){//CRC校验正确，返回主机发送的数据
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata++;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =*prdata;				
			}else if(MCUstate == 1){//CRC错误
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0xFF;
			}else if(MCUstate ==2){//其他故障发生时，回复上位机,或者读取数据
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =reason;
				Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =reason>>8;				
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
		  if(MCUstate != 1){
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] = *prdata++;
			}else{
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x80 + *prdata++;
			}
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata++;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata++;
			Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata++;
			if(MCUstate == 0){//CRC校验正确，返回主机发送的数据
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata++;
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =*prdata;				
			}else if(MCUstate == 1){//CRC错误
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0xFF;
			}else if(MCUstate ==2){//其他故障发生时，回复上位机
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =reason;
				Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =reason>>8;				
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
			if(*(prdata+4) == 2){//等于2的时候是灯指令
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
			if(*(prdata+4) == 2){//等于2的时候是灯指令
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
// 		if(recdata.control.command == 0x11){
// 			recdata.control.command = 0x0B;
// 		}
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = recdata.control.command;	
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] = recdata.control.medicine_num;	
    crc=CRC_GetModbus16(Usart3_Control_Data.txbuf,Usart3_Control_Data.tx_count);
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] =crc;
		Usart3_Control_Data.txbuf[Usart3_Control_Data.tx_count++] =crc>>8;
		for(i=0;i<Usart3_Control_Data.tx_count;i++){//复制需要发送给水平板的数据到数组中，当接收到发药状态时清零
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
// 		if(recdata.control.command == 0x11){
// 			recdata.control.command = 0x0B;
// 		}
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = recdata.control.command;	
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] = recdata.control.medicine_num;	
    crc=CRC_GetModbus16(Usart4_Control_Data.txbuf,Usart4_Control_Data.tx_count);
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] =crc;
		Usart4_Control_Data.txbuf[Usart4_Control_Data.tx_count++] =crc>>8;
		for(i=0;i<Usart4_Control_Data.tx_count;i++){//复制需要发送给水平板的数据到数组中，当接收到发药状态时清零
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
	 if(device_satate == 0){ //设备OK，解析并执行命令
			switch(recdata.control.group_name){
				case 0://其他控制
					if(recdata.control.colum == 0x11){  //皮带控制
						 if(recdata.control.command == 0x00){
								Belt_Speed(0,0,0);
								belt.state = RESERVE;
							  Start_Ok = 0;
							  belt.actual_state = BELT_STOP;
						 }else if(recdata.control.command == 0x10){
								if(recdata.control.medicine_num == 0x00){
										BELT_DIR = BELT_IO_ON;
										belt.state = RESERVE;
										Start_Ok = 0;
										Belt_Speed(1,1,1);
									  belt.actual_state = BELT_LRUN;
								}else{
										belt.actual_time = recdata.control.medicine_num * 200;
										belt.state = READY;
										Start_Ok = 0;
									  belt.dir = 1;
									  belt.actual_state = BELT_LRUN;
								}
						 }else if(recdata.control.command == 0x20){
								if(recdata.control.medicine_num == 0x00){
										BELT_DIR = BELT_IO_OFF;
										belt.state = RESERVE;
										Start_Ok = 0;
										Belt_Speed(1,1,1);
									  belt.actual_state = BELT_RRUN;
								}else{
										belt.actual_time = recdata.control.medicine_num * 200;
										belt.state = READY;
										Start_Ok = 0;
									  belt.dir = 0;
										belt.actual_state = BELT_RRUN;
								}
						 }
						 Speed_Step = 0;
					}else if(recdata.control.colum == 0x22){//闸门控制
						if(recdata.control.medicine_num == 0){
								recdata.control.medicine_num = 2;
						}
						if(recdata.control.command == 0x10){//右闸门关
								Lgate.dir = GATELEFT;
								Lgate.action = LGCLOSE;
							  Lgate.state = READY;
							  Lgate.send_time = recdata.control.medicine_num;
						}else if(recdata.control.command == 0x11){//右闸门开
								Lgate.dir = GATELEFT;
								Lgate.action = LGOPEN;
							  Lgate.state = READY;
							  Lgate.send_time = recdata.control.medicine_num;
						}else if(recdata.control.command == 0x00){//左闸门关
								Rgate.dir = GATERIGHT;
								Rgate.action = LGCLOSE;
							  Rgate.state = READY;
							  Rgate.send_time = recdata.control.medicine_num;
					 }else if(recdata.control.command == 0x01){//左闸门开
								Rgate.dir = GATERIGHT;
								Rgate.action = LGOPEN;
						    Rgate.state = READY;
						    Rgate.send_time = recdata.control.medicine_num;
						}
// 								Rgate.dir = GATERIGHT;
// 								if(recdata.control.medicine_num == 0x00){
// 									Rgate.action = LGCLOSE;
// 								}else{
// 									Rgate.action = LGOPEN;
// 								}
// 								Rgate.state = READY;
// 						}else{
// 								Lgate.dir = GATELEFT;
// 								if(recdata.control.medicine_num == 0x00){
// 									Lgate.action = LGCLOSE;
// 								}else{
// 									Lgate.action = LGOPEN;
// 								}
// 								Lgate.state = READY;							
// 						}

					}else if(recdata.control.colum == 0x33){//通讯超时复位
							Init_Group_Param();
					}
					break;
				case 1:
					Group1_Check_Time = GROUP_CHECK_TIME;
					Group2_Check_Time = GROUP_CHECK_TIME;
					group_send_medicine(1,recdata);
				  if(usart == SELECT_USART1){
						pc_use_usart = SELECT_USART1;
					}else if(usart == SELECT_USART2){
						pc_use_usart = SELECT_USART2;
					}
					break;
				case 2:
					Group1_Check_Time = GROUP_CHECK_TIME;
					Group2_Check_Time = GROUP_CHECK_TIME;
					group_send_medicine(2,recdata);
					if(usart == SELECT_USART1){
						pc_use_usart = SELECT_USART1;
					}else if(usart == SELECT_USART2){
						pc_use_usart = SELECT_USART2;
					}
					break;
				default:
					break;
			}
			response_pc_control(usart,recdata.rec_buf,0x00,0x00);
	}else{//设备故障
		response_pc_control(usart,recdata.rec_buf,reason,2);
	}
}
static void host_read_command(u8 usart,COMM_SlaveRec_Union_Type recdata,u16 reason,u8 device_satate)
{
	u8 tempH,tempL,humiH,humiL;
	u8 device_s1,device_s2;
	 if(device_satate == 0){ //设备OK，解析并执行命令
			switch(recdata.control.colum){
				case 0x0F:
					device_s1 = ((belt.actual_state&0x0F)<<4) + (Lgate.Lactual_state&0x0F);
				  device_s2 = ((Rgate.Ractual_state&0x0F)<<4) + 0x00;//电动滚动是否有错误报警暂时不检测
				  reason = device_s2*256 + device_s1;
				  device_satate = 2;
					break;
				case 0x1F:
				   if(param.temperatureC < 0){
							tempH = ((-1*param.temperatureC)/256)|0x80;
						  tempL = (-1*param.temperatureC)%256;
						}else{
							tempH = param.temperatureC/256;
							tempL = param.temperatureC%256;
						}
// 				   tempH = (u8)(param.temperatureC/10);
// 				   humi = (u8)(param.humidityRH/10);
// 				   reason = humi*256 + temp;
							reason = tempL*256 + tempH;
							device_satate = 2;
					break;
				case 0x2F:
							humiH = param.humidityRH/256;
							humiL = param.humidityRH%256;
							reason = humiL*256 + humiH;
							device_satate = 2;
					break;				
				case 0X63:
				   reason = SOFTWARE_VERSIONS;
				   device_satate = 2;
					break;
				default:
					break;
			}
			response_pc_control(usart,recdata.rec_buf,reason,device_satate);
	}else{//设备故障
		response_pc_control(usart,recdata.rec_buf,reason,2);
	}
}
//=============================================================================
//函数名称:Execute_Host_Comm
//功能概要:执行上位机发出的命令
//参数说明:无
//函数返回:无
//注意    :无
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
		crc=CRC_GetModbus16(Usart1_Control_Data.rxbuf,Usart1_Control_Data.rx_count-2);//帧结束尾不做校验
		if((Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-2]+\
				Usart1_Control_Data.rxbuf[Usart1_Control_Data.rx_count-1]*256 == crc)){	
				for(i = 0;i < 9;i++){
							MCU_Rec.rec_buf[i] = Usart1_Control_Data.rxbuf[i];
				 }//把数据复制给主机通讯结构体
				if(MCU_Rec.control.function == 0x06){
						resolve_host_command(SELECT_USART1, MCU_Rec,0x00,0x00);
				}else if(MCU_Rec.control.function == 0x03){
						host_read_command(SELECT_USART1,MCU_Rec,0x00,0x00);
				}
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
		crc=CRC_GetModbus16(Usart2_Control_Data.rxbuf,Usart2_Control_Data.rx_count-2);//帧结束尾不做校验
		if((Usart2_Control_Data.rxbuf[Usart2_Control_Data.rx_count-2]+\
				Usart2_Control_Data.rxbuf[Usart2_Control_Data.rx_count-1]*256 == crc)){	    
				for(i = 0;i < 9;i++){
							MCU_Rec.rec_buf[i] = Usart2_Control_Data.rxbuf[i];
				 }//把数据复制给主机通讯结构体
				if(MCU_Rec.control.function == 0x06){
						Group1_Check_Time = GROUP_CHECK_TIME;
						Group2_Check_Time = GROUP_CHECK_TIME;
						resolve_host_command(SELECT_USART2, MCU_Rec,0x00,0x00);
				}else if(MCU_Rec.control.function == 0x03){
						host_read_command(SELECT_USART2,MCU_Rec,0x00,0x00);
				}
				res = 0;
		}else{
				response_pc_control(SELECT_USART2,Usart2_Control_Data.rxbuf,0x00FF,1);
				res = 1;
		}
	}
			return res;
}
void Group1_Check_State(void)
{
	u8 i,j;
	u16 crc;
	if(Group1_Check_Time == 0){
			if((Group1.send_count > 0)&&(Usart3_Control_Data.tx_count == 0)){
				for(i=0;i<GROUP_LINE_MAX;i++){
					for(j=0;j<GROUP_COLUM_MAX;j++){
						if((Group1.group_send[i][j].send_buf[3] != 0xFF)&&(Group1.group_send[i][j].send_buf[2] == (i+1))\
							&&(Group1.group_send[i][j].send_buf[3] == (j+1))){  //行地址不等于0XFF时代表发送药，需要检查状态
								if(Usart3_Control_Data.tx_count !=0){
										return;
								}
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
// 								Group1.send_count--;
			}
					 Group1_Check_Time = GROUP_CHECK_TIME;
		}
}
void Group2_Check_State(void)
{
	u8 i,j;
	u16 crc;
	if(Group2_Check_Time == 0){
		if((Group2.send_count > 0)&&(Usart4_Control_Data.tx_count == 0)){
				for(i=0;i<GROUP_LINE_MAX;i++){
					for(j=0;j<GROUP_COLUM_MAX;j++){
						if((Group2.group_send[i][j].send_buf[2] != 0xFF)&&(Group2.group_send[i][j].send_buf[2] == (i+1))\
							&&(Group2.group_send[i][j].send_buf[3] == (j+1))){  //行地址不等于0XFF时代表发送药，需要检查状态
							  if(Usart4_Control_Data.tx_count !=0){
										return;
								}
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
// 				Group2.send_count--;
			}
		 Group2_Check_Time = GROUP_CHECK_TIME;
	}
}
void Group_Check_State(void)
{
	u8 i,j;
	u16 crc;
	if(Group1_Check_Time == 0){
			if((Group1.send_count > 0)&&(Usart3_Control_Data.tx_count == 0)){
				for(i=0;i<GROUP_LINE_MAX;i++){
					for(j=0;j<GROUP_COLUM_MAX;j++){
						if((Group1.group_send[i][j].send_buf[3] != 0xFF)&&(Group1.group_send[i][j].send_buf[2] == (i+1))\
							&&(Group1.group_send[i][j].send_buf[3] == (j+1))){  //行地址不等于0XFF时代表发送药，需要检查状态
								if(Usart3_Control_Data.tx_count !=0){
										return;
								}
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
// 								Group1.send_count--;
			}
			
			if((Group2.send_count > 0)&&(Usart4_Control_Data.tx_count == 0)){
				for(i=0;i<GROUP_LINE_MAX;i++){
					for(j=0;j<GROUP_COLUM_MAX;j++){
						if((Group2.group_send[i][j].send_buf[2] != 0xFF)&&(Group2.group_send[i][j].send_buf[2] == (i+1))\
							&&(Group2.group_send[i][j].send_buf[3] == (j+1))){  //行地址不等于0XFF时代表发送药，需要检查状态
							  if(Usart4_Control_Data.tx_count !=0){
										return;
								}
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
// 				Group2.send_count--;
			}
		 Group1_Check_Time = GROUP_CHECK_TIME;
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
		crc=CRC_GetModbus16(Usart3_Control_Data.rxbuf,Usart3_Control_Data.rx_count-2);//帧结束尾不做校验
		if((Usart3_Control_Data.rxbuf[Usart3_Control_Data.rx_count-2]+\
				Usart3_Control_Data.rxbuf[Usart3_Control_Data.rx_count-1]*256 == crc)){	 
        if(Usart3_Control_Data.rxbuf[1] == 0x03){				
						for(i = 0;i < 10;i++){
									Group1.group_rec[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].rec_buf[i] = Usart3_Control_Data.rxbuf[i];
						 }//把数据复制给主机通讯结构体
						 if(Group1.group_rec[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].feedback.command != 2){
							 switch(Group1.group_rec[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].feedback.result2){
									case 0x01://正在发药
										  //测试代码
// 											mcu_send_pc_status(PCusart,GRusart,Usart3_Control_Data.rxbuf,0x00,0x00);
										break;
									case 0x02:
									case 0x03:
									case 0x04:
										if(Group1.send_count > 0){
											Group1.send_count--;
										}
// 										if(Group1.send_count == 0){ //行和列都要查询，目前这个功能还没有实现，应该分开行和列来计数
// 											Group1.group_send[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].send_buf[2] = 0xFF;
// 										}
										Group1.group_send[Usart3_Control_Data.rxbuf[2]-1][Usart3_Control_Data.rxbuf[3]-1].send_buf[3] = 0xFF;
										mcu_send_pc_status(PCusart,GRusart,Usart3_Control_Data.rxbuf,0x00,0x00);
										break;
									default:
// 										Usart3_Control_Data.rxbuf[7] = 0X0F;  //测试代码
// 										mcu_send_pc_status(PCusart,GRusart,Usart3_Control_Data.rxbuf,0x00,0x00);
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
		crc=CRC_GetModbus16(Usart4_Control_Data.rxbuf,Usart4_Control_Data.rx_count-2);//帧结束尾不做校验
		if((Usart4_Control_Data.rxbuf[Usart4_Control_Data.rx_count-2]+\
				Usart4_Control_Data.rxbuf[Usart4_Control_Data.rx_count-1]*256 == crc)){	 
        if(Usart4_Control_Data.rxbuf[1] == 0x03){				
						for(i = 0;i < 10;i++){
									Group2.group_rec[Usart4_Control_Data.rxbuf[2]-1][Usart4_Control_Data.rxbuf[3]-1].rec_buf[i] = Usart4_Control_Data.rxbuf[i];
						 }//把数据复制给主机通讯结构体
						 if(Group2.group_rec[Usart4_Control_Data.rxbuf[2]-1][Usart4_Control_Data.rxbuf[3]-1].feedback.command != 2){
							 switch(Group2.group_rec[Usart4_Control_Data.rxbuf[2]-1][Usart4_Control_Data.rxbuf[3]-1].feedback.result2){
									case 0x01://正在发药
										//测试代码
// 										mcu_send_pc_status(PCusart,GRusart,Usart4_Control_Data.rxbuf,0x00,0x00);
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
											//测试代码
// 										Usart4_Control_Data.rxbuf[7] = 0x0F;
// 										mcu_send_pc_status(PCusart,GRusart,Usart4_Control_Data.rxbuf,0x00,0x00);
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
void Send_getout_to_pc(u8 usart)
{
	 u16 crc;
		if(send_getout_to_pc == 1){
// 			if(usart == SELECT_USART1){
// 					while(Usart1_Control_Data.tx_count!=0);
// 					Usart1_Control_Data.tx_count = 0;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x06;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x06;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x51;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x01;
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
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x51;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x01;
					crc=CRC_GetModbus16(Usart2_Control_Data.txbuf,Usart2_Control_Data.tx_count);
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc>>8;
					Usart2_Control_Data.tx_index = 0;
					USART_SendData(USART2,Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_index++]);
// 			}			
			send_getout_to_pc = 0;
		}		
		if((Lshipment_send_state ==1)||(Rshipment_send_state ==1) ){
// 			if(usart == SELECT_USART1){
// 					while(Usart1_Control_Data.tx_count!=0);
// 					Usart1_Control_Data.tx_count = 0;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x06;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x06;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x61;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					if(shipment_send_state == 1){
// 						Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					}else{
// 						Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x01;
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
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x61;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
					if(Lshipment_send_state == 1){
						Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x01;
						Lshipment_send_state = 0;
					}else{
						Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
						Rshipment_send_state = 0;
					}
					crc=CRC_GetModbus16(Usart2_Control_Data.txbuf,Usart2_Control_Data.tx_count);
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc>>8;
					Usart2_Control_Data.tx_index = 0;
					USART_SendData(USART2,Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_index++]);
// 			}			
		}	
		if((Lshipment_send_errstate ==1)||(Rshipment_send_errstate ==1) ){
// 			if(usart == SELECT_USART1){
// 					while(Usart1_Control_Data.tx_count!=0);
// 					Usart1_Control_Data.tx_count = 0;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] = 0x06;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x06;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x61;
// 					Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 					if(Lshipment_send_errstate == 1){
// 						Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x00;
// 						Lshipment_send_errstate = 0;
// 					}else{
// 						Usart1_Control_Data.txbuf[Usart1_Control_Data.tx_count++] =0x01;
// 						Rshipment_send_errstate = 0;
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
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x71;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
					if(Lshipment_send_errstate == 1){
						Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x01;
						Lshipment_send_errstate = 0;
					}else{
						Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =0x00;
						Rshipment_send_errstate = 0;
					}
					crc=CRC_GetModbus16(Usart2_Control_Data.txbuf,Usart2_Control_Data.tx_count);
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc;
					Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_count++] =crc>>8;
					Usart2_Control_Data.tx_index = 0;
					USART_SendData(USART2,Usart2_Control_Data.txbuf[Usart2_Control_Data.tx_index++]);
// 			}			
		}		
}
//=============================================================================
//函数名称:Respond_Host_Comm
//功能概要:响应上位机的发出的数据命令，数据已经从串口一接收完整
//参数说明:无
//函数返回:无
//注意    :无
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
	
				Execute_level_Comm(pc_use_usart,SELECT_USART3);
				Usart3_Control_Data.rx_count = 0;
				Auto_Frame_Time3 = AUTO_FRAME_TIMEOUT3;
				Usart3_Control_Data.rx_aframe = 0;
			
    }
	  if (1 == Usart4_Control_Data.rx_aframe){    
 				Execute_level_Comm(pc_use_usart,SELECT_USART4);
				Usart4_Control_Data.rx_count = 0;
				Auto_Frame_Time4 = AUTO_FRAME_TIMEOUT4;
				Usart4_Control_Data.rx_aframe = 0;
    }
		Send_getout_to_pc(pc_use_usart);
		Group1_Check_State();
		Group2_Check_State();
}

















