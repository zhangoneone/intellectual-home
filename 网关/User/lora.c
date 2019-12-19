#include "lora.h"
/*汇聚节点的lora用的是usart2,配置usart2*/

u8 USART2_RX_BUF[1024]; //数组用来存储接收到的数据，最多能接收1024字节
u16 USART2_RX_STA=0;         		//接收状态标记	 0-14位为接收字节数，15位接收完成标志位
int count2=0;
#define RX_BUF_MAX_LEN        1024
struct  STRUCT_USARTx_Fram Lora_Record = { 0 };//   lora接收到的数组

u8 USART2_TX_BUF[1024];
void lora_init()
{
	USART2_Config(9600);
	USART2_NVIC_Configuration();
}

//串口2初始化(lora串口)9600 8N1
void USART2_Config(unsigned int bound)   //初始化lora串口
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);                    //初始化GPIOA
	 
	//配置USART2参数
	USART_InitStructure.USART_BaudRate = bound;	                    //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(USART2, &USART_InitStructure);                       //初始化USART2
	
	//打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  // 使能指定的USART2发送中断
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能指定的USART2接收中断
	USART_ITConfig ( USART2, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	

	//使能 USART2， 配置完毕
	USART_Cmd(USART2, ENABLE);                             // USART2使能

	//如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(USART2, USART_FLAG_TC);                //清串口2发送标志
}

//配置lora串口NVIC中断
void USART2_NVIC_Configuration()//配置lora串口NVIC中断
{
	NVIC_InitTypeDef    NVIC_InitStructure; 					   //定义用于配置中断的结构体变量

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                //中断优先级分组  抢占式优先级别设置为2位；响应优先级占2位
	
  //串口2中断优先级设置(lora串口)
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			   //指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	   //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             //指定响应优先级别2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

}

//参数:要通过lora发出去的数据
//末尾有0x0d和0x0a????
void Lora_Printf(char *send)
{
	while((*send)!=0x00)
	{
		usart2_SendByte(*send);
		send++;
	}
//	usart2_SendByte(0x0d);
	//usart2_SendByte(0x0a);
}


void USART2_IRQHandler()//串口2(lora)的中断,此中断一定是另一个lora产生
{ 
	  u8 Res;//暂时缓存接收的数据
	  int i=0;
	   if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//判断是否为串口2接收中断
	  {
	     Res=USART_ReceiveData(USART2);//接收到数据放进res	
		if ( Lora_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
		{
			Lora_Record .Data_RX_BUF [ Lora_Record .InfBit .FramLength ++ ]  = Res;//数据被写到了Data_RX_BUF
		}

	}
	 	 
	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
	{
    Lora_Record .InfBit .FramFinishFlag = 1;
		
		Res= USART_ReceiveData( USART2 );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
		 
  }	
}

//void USART2_IRQHandler()//串口2(lora)的中断,此中断一定是另一个lora产生
//{ 
//	  u8 Res;//暂时缓存接收的数据
//	  int i=0;
//	   u8 *s = "+IPD,10,192.168.191.1,5000:test wifi is whether ok? ";
//	   if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)//判断是否为串口2接收中断
//	  {
//	     Res=USART_ReceiveData(USART2);//接收到数据放进res	
//			if(Res==0x0d)
//			{
//		//	  printf("lora模块收到信息:%s\r\n",USART2_RX_BUF);
//				//接收完毕，给wifi模块转发
//				for(i=0;i<count2;i++)
//				{
//					USART2_TX_BUF[i]=USART2_RX_BUF[i];
//				}
//				  sendtowifi(s);
//			//	sendtowifi(USART2_TX_BUF);
//				//+IPD,10,192.168.191.1,5000:test wifi is whether ok? 
//				//wifi要求数据格式"+IPD,<len>[,<remote IP>,<remoteport>]:<data>"
//	//			USART_printf(USART3,USART2_TX_BUF);
//				//本次数据发送后，就要清除掉了
//			  USART2_RX_STA=count2;
//				for(i=0;i<USART2_RX_STA;i++)
//				{
//					USART2_RX_BUF[i]=0x00;
//				}
//			  count2=0;
//			 }
//			else
//			{
//				if(Res==0x0a)
//				{}
//				else
//				{
//					USART2_RX_BUF[count2]=Res;
//					count2++;
//				}
//			}
//		 USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清除接收中断标志
//		}
//}

