#include "test.h"
#include "bsp_esp8266.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "lora.h"

uint8_t ESP8266_CWLIF ( char * pStaIp )//获取连接到softap的station信息
{
	uint8_t uc, ucLen;
	
	char * pCh, * pCh1;
	
	
  ESP8266_Cmd ( "AT+CWLIF", "OK", 0, 100 );
	
	pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "," );
	
	if ( pCh )
	{
		pCh1 = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "AT+CWLIF\r\r\n" ) + 11;
	  ucLen = pCh - pCh1;
	}

	else
		return 0;
	
	for ( uc = 0; uc < ucLen; uc ++ )
		pStaIp [ uc ] = * ( pCh1 + uc);
	
	pStaIp [ ucLen ] = '\0';
	
	return 1;
	
}



uint8_t ESP8266_CIPAP ( char * pApIp )//设置softap的ip地址
{
	char cCmd [ 30 ];
		
	
	sprintf ( cCmd, "AT+CIPAP=\"%s\"", pApIp );
	
  if ( ESP8266_Cmd ( cCmd, "OK", 0, 5000 ) )
		return 1;
 
	else 
		return 0;
	
}



/**
  * @brief  ESP8266 （Sta Tcp Client）透传
  * @param  无
  * @retval 无
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
	uint8_t ucId;
	char cStr [ 100 ], cCh;

  char * pCh;
	
	char *CmdOk="已发送到远端设备!\r\n";
	char *SumtOk="send to server OK!\r\n";
	macESP8266_CH_ENABLE();
	
	//测试模块，设置wifi的工作模式
	ESP8266_AT_Test ();    //测试AT启动,发送联机命令
	
	ESP8266_Net_Mode_Choose ( STA );   //station模式bool  ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
	
  while ( ! ESP8266_JoinAP ( macUser_ESP8266_CwjServer_M , macUser_ESP8266_CwjServer_N) );//连接WiFi热点

	while ( ! ESP8266_Enable_MultipleId( ENABLE ) );//设置模块为tcp多连接模式
	
 // while ( ! ESP8266_Link_Server ( macUser_ESP8266_TcpServer_Enum, macUser_ESP8266_TcpServer_IP,macUser_ESP8266_TcpServer_Port,Single_ID_0 ) );//连接服务器	
	
	//printf("模块在路由器上的ID:%d\r\n",ESP8266_Get_IdLinkStatus());//端口（Id）的连接状态，低5位为有效位，分别对应Id5~0，某位若置1表该Id建立了连接，若被清0表该Id未建立连接
	
	//接下来配置模块为tcp服务器
	
	  while(!ESP8266_StartOrShutServer ( ENABLE, "8080", "100"));//建立一个tcp服务器，8080端口侦听，接受远端tcp客户端发来的指令
		
		
	while(! ESP8266_Link_Server ( enumTCP, "106.15.196.133", "8080" , Multiple_ID_1));//建立一个tcp客户端，连接远端服务器,发送数据,id是Multiple_ID_1
  //	while(!ESP8266_UnvarnishSend())//设置透传模式
		
	while ( 1 )
	{		
		//处理wifi的缓存字符数组
		//数据来自pc,是下发的命令字和数据,需要转发给远端lora
		//转发完成后，给pc回执
		if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag )
		{
			int i=0;
			USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //失能串口接收中断
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';//帧尾加上0x00,为了断开字符串
			
			
			if ( ( pCh = strstr ( (strEsp8266_Fram_Record .Data_RX_BUF+1), "+IPD," ) ) != 0 ) //有wifi主机发送的信息
			{
				printf("pc下发指令/数据到汇聚节点，内容:%s\r\n",pCh+9);
				//通过lora转发出去
			  Lora_Printf(pCh+9);
				
				ESP8266_SendString (DISABLE, CmdOk, strlen(CmdOk), Multiple_ID_1 );//向主机发送回执
			}			
		  strEsp8266_Fram_Record .InfBit .FramLength = 0;//帧长和帧结束标志都清0，下次从buf的0处开始写
	    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
			
			USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //使能串口接收中断
			
		}
		//处理lora递交上来的字符串
		if(Lora_Record .InfBit.FramFinishFlag)	
		{
			int i=0;
			USART_ITConfig ( USART2, USART_IT_RXNE, DISABLE ); //失能串口接收中断，lora串口
			Lora_Record .Data_RX_BUF [ Lora_Record .InfBit .FramLength ]  = '\0';//帧尾加上0x00,为了断开字符串
			
			//lora递交来的数据，递交给wifi模块，给pc
			
			printf("get remote lora's message:%s\r\n",Lora_Record .Data_RX_BUF);
			printf("submit to server...\r\n");
//			if(ESP8266_Get_LinkStatus()!=3)// * 返回  : 2，获得ip      3，建立连接  4，失去连接     0，获取状态失败
//			{
//				while(! ESP8266_Link_Server ( enumTCP, "106.15.196.133", "8080" , Multiple_ID_1));//建立一个tcp客户端，连接远端服务器,发送数据,id是Multiple_ID_1
//			}
			
			if(ESP8266_SendString (DISABLE, Lora_Record .Data_RX_BUF, strlen(Lora_Record .Data_RX_BUF), Multiple_ID_1 ));//发给wifi
			{
				//lora传给服务器信息成功，发送回执
			//	Lora_Printf(SumtOk);
			}
			
			Lora_Record .InfBit .FramLength = 0;//帧长和帧结束标志都清0，下次从buf的0处开始写
	    Lora_Record .InfBit .FramFinishFlag = 0;	
			
			USART_ITConfig ( USART2, USART_IT_RXNE, ENABLE ); //使能串口接收中断
		}
	}	
}


