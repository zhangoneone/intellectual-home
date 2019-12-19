#include "lora.h"
/*执行节点的lora用的是usart3,配置usart3*/

u8 USART3_RX_BUF[1024]; //数组用来存储接收到的数据，最多能接收1024字节
u16 USART3_RX_STA=0;         		//接收状态标记	 0-14位为接收字节数，15位接收完成标志位
int count3=0;
#define RX_BUF_MAX_LEN        1024
struct  STRUCT_USARTx_Fram Lora_Record = { 0 };//   lora接收到的数组

u8 USART3_TX_BUF[1024];
void lora_init()
{
	USART3_Config(9600);
	USART3_NVIC_Configuration();
}

//串口3初始化(lora串口)9600 8N1
void USART3_Config(unsigned int bound)   //初始化lora串口
{
		GPIO_InitTypeDef GPIO_InitStructure;	  //串口端口配置结构体变量
	USART_InitTypeDef USART_InitStructure;	  //串口参数配置结构体变量

	//使能 USART3 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//打开串口复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //打开PB端口时钟
  	
	//将USART3 Tx（发送脚）的GPIO配置为推挽复用模式   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				  //选定哪个IO口 现选定PB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);    				  //初始化GPIOB
	
	//将USART3 Rx（接收脚）的GPIO配置为浮空输入模式														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //选定哪个IO口 现选定PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);                    //初始化GPIOB
	  
	//配置USART3参数
//		USART_InitStructure.USART_BaudRate = 115200;	                    //波特率设置：115200
	USART_InitStructure.USART_BaudRate =bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(USART3, &USART_InitStructure);                       //初始化USART3
   
	//打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(USART3, USART_IT_TXE, ENABLE);  // 使能指定的USART3发送中断
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 使能指定的USART3接收中断
	USART_ITConfig ( USART3, USART_IT_IDLE, ENABLE ); //使能串口总线空闲中断 	

	//使能 USART3， 配置完毕
	USART_Cmd(USART3, ENABLE);                             // USART3使能

	//如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(USART3, USART_FLAG_TC);                //清串口3发送标志
}

//配置lora串口NVIC中断
void USART3_NVIC_Configuration()//配置lora串口NVIC中断
{
	NVIC_InitTypeDef    NVIC_InitStructure; 					   //定义用于配置中断的结构体变量

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                //中断优先级分组  抢占式优先级别设置为2位；响应优先级占2位
	
  //串口3中断优先级设置(lora串口)
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			   //指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	   //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;             //指定响应优先级别2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

}

//参数:要通过lora发出去的数据
//末尾有0x0d和0x0a
void Lora_Printf(char *send)
{
	while((*send)!=0x00)
	{
		usart3_SendByte(*send);
		send++;
	}
	usart3_SendByte(0x0d);
	usart3_SendByte(0x0a);
}

//返回lora接收到的字符串
//成功则返回字符串首地址，失败则返回NULL
char* lora_get()
{
		if(Lora_Record .InfBit.FramFinishFlag)	
		{
			int i=0;
	//		USART_ITConfig (USART3, USART_IT_RXNE, DISABLE ); //失能串口接收中断
			Lora_Record .Data_RX_BUF [ Lora_Record .InfBit .FramLength ]  = '\0';//帧尾加上0x00,为了断开字符串
			
			Lora_Record .InfBit .FramLength = 0;//帧长和帧结束标志都清0，下次从buf的0处开始写
	    Lora_Record .InfBit .FramFinishFlag = 0;	
			
			  	return Lora_Record .Data_RX_BUF;
	//		USART_ITConfig (USART3, USART_IT_RXNE, ENABLE ); //能串口接收中断  加这两个整个系统就崩了
		}
		else return NULL;
}

void USART3_IRQHandler()//串口3(lora)的中断事件,此中断事件是另一个lora产生,接收数据并响应
{ 
	  u8 Res;//暂时缓存接收的数据
	  int i=0;
	   if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//判断是否为串口3接收中断
	  {
					Res=USART_ReceiveData(USART3);//接收到数据放进res	
					if ( Lora_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
					{
							Lora_Record .Data_RX_BUF [ Lora_Record .InfBit .FramLength  ]  = Res;//数据被写到了Data_RX_BUF
							Lora_Record .InfBit .FramLength++;
					}	
	  }
		if ( USART_GetITStatus( USART3, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
		{
			Lora_Record .InfBit .FramFinishFlag = 1;
		 	printf("接收的字符串%s\r\n%s\r\n",Lora_Record .Data_RX_BUF,Lora_Record .Data_RX_BUF+1);																																									//响应事件，做出回复
						
			Res= USART_ReceiveData( USART3 );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
			handle_lora();
		}
	
}
//处理lora接收到的控制字/字符串
//第一位控制字，剩下位是数据
void handle_lora()
{
	   char * s="";

			s=lora_get();	//得到控制字
			//检查控制字
			if(s!=NULL)
			{
				 printf("lora get message:%s\r\n",s);
				//根据控制字处理控制事件
				 lora_ctr(s[0]);
			//	Lora_Printf(s);
				//处理数据
				printf("I will handle data about %s\r\n",s+1);//此处暂时打印出来
			}
			
			else
			{
				printf("not get message!\r\n");//发生异常，没得到控制字
			}
}

//功能：通过不同参数，控制相应功能
//参数:传入的参数

//1 开灯 2 关灯 3 蜂鸣器发声 4停止发声 5 加速度传感器开始工作 6 停止工作 7发送时间 8发送温湿度 9 发送当前压力值
void lora_ctr(char num)
{
	switch(num)
	{
		case '1':Lora_Printf("1");LED_ON();break;
		case '2':Lora_Printf("2");LED_OFF();break;
		case '3':Lora_Printf("3");BEEP_OFF();BEEP_ON();break;
		case '4':Lora_Printf("4");BEEP_OFF();break;
		case '5':Lora_Printf("5");mpu6050_open();break;
		case '6':Lora_Printf("6");mpu6050_close();break;
		case '7':Lora_Printf("7");sendtime();break;
		case '8':Lora_Printf("8");sendsht();break;
		case '9':Lora_Printf("9");sendpress();break;
		case '0':Lora_Printf("0");break;
		default:break;
	}
}