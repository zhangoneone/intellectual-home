#include "sys.h"
#include "usart.h"	  
#include "rtc_time.h"
#include "makefile.h"

#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	

//#include "bluetooth.h"
u8 cmd[100];//保存命令字
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

 char bufrec[100];
 int count=0;
 rtc_time_t cur_time={2017,6,17,6,17,10,0};//当前时间
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	 
u16 temp; 

//串口1初始化
void USART1_Config(void)		//初始化 配置USART1
{
	GPIO_InitTypeDef     GPIO_InitStructure;   //串口端口配置结构体变量
	USART_InitTypeDef    USART_InitStructure;  //串口参数配置结构体变量
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//第1步：打开GPIO和USART部件的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	 //打开GPIOA时钟和GPIOA复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//打开串口复用时钟
	USART_DeInit(USART1);  //复位串口1

	//第2步：将USART1 Tx（发送脚）的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			   //串口1发送脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //输出速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   //初始化GPIOA
														  
	//第3步：将USART Rx（接收脚）的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			   //串口1接收脚
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   //初始化GPIOA

//	 //第4步：Usart1 NVIC 配置
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	//第5步：配置USART1参数
	USART_InitStructure.USART_BaudRate             = 115200;							 //波特率设置：115200
	USART_InitStructure.USART_WordLength           = USART_WordLength_8b;			 //数据位数设置：8位
	USART_InitStructure.USART_StopBits             = USART_StopBits_1;				 //停止位设置：1位
	USART_InitStructure.USART_Parity               = USART_Parity_No;				 //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl  = USART_HardwareFlowControl_None; //硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;	 //接收与发送都使能
	USART_Init(USART1, &USART_InitStructure);										 //初始化USART1

    //打开发送中断和接收中断(如果需要中断)
	//  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  // 使能指定的USART1发送中断 ；此处使能导致不停进入串口中断
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能指定的USART1接收中断 ；

	//第6步：使能 USART1， 配置完毕
	USART_Cmd(USART1, ENABLE);							   //使能 USART1

    //如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(USART1, USART_FLAG_TC);                //清串口1发送标志
	
}

//串口2初始化
void USART2_Config(void)   //初始化 配置USART2
{
	GPIO_InitTypeDef    GPIO_InitStructure;	   //串口端口配置结构体变量
	USART_InitTypeDef   USART_InitStructure;   //串口参数配置结构体变量

	//使能 USART2 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//打开串口复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);   //打开PA端口时钟

	//将USART2 Tx（发送脚）的GPIO配置为推挽复用模式   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				  //选定哪个IO口 现选定PA2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);    				  //初始化GPIOA

	//将USART2 Rx（接收脚）的GPIO配置为浮空输入模式														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				  //选定哪个IO口 现选定PA3
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);                    //初始化GPIOA
	  
	//配置USART2参数
	USART_InitStructure.USART_BaudRate = 115200;	                    //波特率设置：115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(USART2, &USART_InitStructure);                       //初始化USART2
	
	//打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  // 使能指定的USART2发送中断
  //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能指定的USART2接收中断

	//使能 USART2， 配置完毕
	USART_Cmd(USART2, ENABLE);                             // USART2使能

	//如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(USART2, USART_FLAG_TC);                //清串口2发送标志
}

//串口3初始化
//void USART3_Config(u32 bound)   //初始化 配置USART3
//{
//	GPIO_InitTypeDef GPIO_InitStructure;	  //串口端口配置结构体变量
//	USART_InitTypeDef USART_InitStructure;	  //串口参数配置结构体变量

//	//使能 USART3 时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//打开串口复用时钟
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //打开PB端口时钟
//  	
//	//将USART3 Tx（发送脚）的GPIO配置为推挽复用模式   
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				  //选定哪个IO口 现选定PB10
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
//	GPIO_Init(GPIOB, &GPIO_InitStructure);    				  //初始化GPIOB
//	
//	//将USART3 Rx（接收脚）的GPIO配置为浮空输入模式														  
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //选定哪个IO口 现选定PB11
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
//	GPIO_Init(GPIOB, &GPIO_InitStructure);                    //初始化GPIOA
//	  
//	//配置USART3参数
////		USART_InitStructure.USART_BaudRate = 115200;	                    //波特率设置：115200
//	USART_InitStructure.USART_BaudRate =bound;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
//	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
//	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
//	USART_Init(USART3, &USART_InitStructure);                       //初始化USART3
//	
//    //打开发送中断和接收中断(如果需要中断)
//	//  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);  // 使能指定的USART3发送中断
//  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 使能指定的USART3接收中断

//	//使能 USART3， 配置完毕
//	USART_Cmd(USART3, ENABLE);                             // USART3使能

//	//如下语句解决第1个字节无法正确发送出去的问题
//    USART_ClearFlag(USART3, USART_FLAG_TC);                //清串口3发送标志
//}

//串口4初始化
void UART4_Config(void)   //初始化 配置USART4
{
	GPIO_InitTypeDef GPIO_InitStructure;	   //串口端口配置结构体变量
	USART_InitTypeDef USART_InitStructure;	   //串口参数配置结构体变量

	//使能 UART4 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//打开串口复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   //打开PC端口时钟
	
	//将USART4 Tx（发送脚）的GPIO配置为推挽复用模式   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				  //选定哪个IO口 现选定PC10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);    				  //初始化GPIOC

	//将UART4 Rx（接收脚）的GPIO配置为浮空输入模式														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //选定哪个IO口 现选定PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);                    //初始化GPIOC
	  
	//配置UART4参数
	USART_InitStructure.USART_BaudRate = 9600;	                    //波特率设置：9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(UART4, &USART_InitStructure);                       //初始化UART4
	
	//打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(UART4, USART_IT_TXE, ENABLE);  // 使能指定的UART4发送中断
  	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); // 使能指定的UART4接收中断

	//使能 UART4， 配置完毕
	USART_Cmd(UART4, ENABLE);                             // UART4使能

	//如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(UART4, USART_FLAG_TC);                //清串口4发送标志
}

//串口5初始化
void UART5_Config(void)   //初始化 配置UART5
{
	GPIO_InitTypeDef GPIO_InitStructure;		//串口端口配置结构体变量
	USART_InitTypeDef USART_InitStructure;		//串口参数配置结构体变量

	//使能 UART5 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//打开串口复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   //打开PC端口时钟
	
	//将UART5 Tx（发送脚）的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				  //选定哪个IO口 现选定PC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);    				  //初始化GPIOC

	//将UART5 Rx（接收脚）的GPIO配置为浮空输入模式															  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				  //选定哪个IO口 现选定PD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);                    //初始化GPIOC
	  
	//配置UART5参数
	USART_InitStructure.USART_BaudRate = 2400;	                    //波特率设置：2400
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(UART5, &USART_InitStructure);                       //初始化UART5

	//打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(UART5, USART_IT_TXE, ENABLE);  // 使能指定的UART4发送中断
  	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // 使能指定的UART4接收中断

	//使能 UART5， 配置完毕
	USART_Cmd(UART5, ENABLE);                             // UART5使能

	//如下语句解决第1个字节无法正确发送出去的问题 */
    USART_ClearFlag(UART5, USART_FLAG_TC);                //清串口5发送标志
}

void usart3_SendByte(unsigned char SendData)	  //用串口3发送一个字节数据
{	  
	USART_SendData(USART3,SendData);   //用串口3发送一个字节
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);  //等待串口3发送完毕
}  

void uart4_SendByte(unsigned char SendData)	  //用串口4发送一个字节数据
{	  
	USART_SendData(UART4,SendData);   //用串口3发送一个字节
	while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);  //等待串口4发送完毕
} 
void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
		int i;
		USART_RX_STA=0;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		printf("%c",Res);//回写
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else {USART_RX_STA|=0x8000;}//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}
			if(Res==13)
			{
				printf("\r\n");
				printf("root#");
				for(i=0;i<count;i++)//命令行输入的，在手机蓝牙上显示
				{
					cmd[i]=bufrec[i];
		 			usart3_SendByte(cmd[i]);
				}
		  	make(bufrec);//执行相应的makefile
				//清除buf
			 for(i=0;i<count;i++)
			{
				bufrec[i]='\0';
			}
			count=0;
			}
			else
			{
				bufrec[count]=Res;
				count++;
			}
			if(Res==0x7f)//删除键
			{
				bufrec[count]='\0';
				count--;
				bufrec[count]='\0';
				count--;
			}
     }	
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif

//void USART3_IRQHandler()//串口3的中断
//{ 
//	 u8 Res;//暂时缓存接收的数据
//	  int i=0;
//	   if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//判断是否为串口3接收中断
//	  {
//	     Res=USART_ReceiveData(USART3);//接收到数据放进res		
//			if(Res==0x0d)
//			{
//			printf("蓝牙接收到是%s\r\n",USART3_RX_BUF);
//				usart3_SendByte('o');
//				usart3_SendByte('k');
//			USART3_RX_STA=count3;
//				for(i=0;i<USART3_RX_STA;i++)
//				{
//					USART3_RX_BUF[i]=0x00;
//				}
//			count3=0;
//			}
//			else
//			{
//				if(Res==0x0a)
//				{}
//				else
//				{
//					USART3_RX_BUF[count3]=Res;
//					count3++;
//				}
//			}
//		 USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清除接收中断标志
//			}
//}



