#include "bluetooth.h"
#include "led.h"
#include "makefile.h"
#include "buzzer.h"

u8 USART3_RX_BUF[USART3_REC_LEN]; //数组用来存储接收到的数据，而USART3_REC_LEN为最多能接收的字节限度
__align(8) u8 USART3_TX_BUF[USART3_REC_LEN]; 	//发送缓冲,最大USART3_REC_LEN字节
u16 USART3_RX_STA=0;         		//接收状态标记	 0-14位为接收字节数，15位接收完成标志位

int count3=0;

//蓝牙初始化
void bluetooth_init()
{
	NVIC_InitTypeDef    NVIC_InitStructure; 					   //定义用于配置中断的结构体变量

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                //中断优先级分组  抢占式优先级别设置为2位；响应优先级占2位
	
  //串口3中断优先级设置(蓝牙串口)
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			   //指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	   //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             //指定响应优先级别2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	
	USART3_Config(9600);//蓝牙接收串口3初始化
}

void USART3_IRQHandler()//串口3的中断
{ 
	 u8 Res;//暂时缓存接收的数据
	  int i=0;
	   if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//判断是否为串口3接收中断
	  {
	     Res=USART_ReceiveData(USART3);//接收到数据放进res		
			if(Res==0x0d)
			{
			  printf("蓝牙收到信息:%s\r\n",USART3_RX_BUF);
				make(USART3_RX_BUF);//执行相应的makefile
				blcontrol(USART3_RX_BUF[0]-48);
//				usart3_SendByte('o');
//				usart3_SendByte('k');
			USART3_RX_STA=count3;
				for(i=0;i<USART3_RX_STA;i++)
				{
					USART3_RX_BUF[i]=0x00;
				}
			count3=0;
			}
			else
			{
				if(Res==0x0a)
				{}
				else
				{
					USART3_RX_BUF[count3]=Res;
					count3++;
				}
			}
		 USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清除接收中断标志
			}
}

//返回值：蓝牙接收到的字符串/数组
char* blrec()
{
		return (*(& USART3_RX_BUF));
}

//参数:蓝牙要发送的字符串/数组
void blsend(u8 *send)
{
	while((*send)!=0x00)
	{
		usart3_SendByte(*send);
		send++;
	}
}

//功能：通过不同参数，控制相应功能
//参数:传入的参数

//1 开灯 2 关灯 3 蜂鸣器发声 4停止发声 5 加速度传感器开始工作 6 停止工作 7发送时间 8发送温湿度 9 发送当前压力值
int blcontrol(char num)//1成功返回
{
	switch(num)
	{
		case 1:LED_ON();blsend("1");break;
		case 2:LED_OFF();blsend("2");break;
		case 3:BEEP_ON();blsend("3");break;
		case 4:BEEP_OFF();;blsend("4");break;
		case 5:mpu6050_open();;blsend("5");break;
		case 6:mpu6050_close();blsend("6");break;
		case 7:blsend("7");sendtime();break;
		case 8:blsend("8");sendsht();break;
		case 9:blsend("9");sendpress();break;
		case 0:blsend("0");break;
		default:break;
	}
}

void sendtime()
{
	char dis[60];
	sprintf(dis,"current time:%d.%d.%d  %d:%d:%d\r\n",get_time().year,get_time().month,get_time().day,get_time().hour,get_time().minute,get_time().second);
	blsend(dis);
}

void testblue(void)
{
	int i=0;
	if(USART3_RX_STA)
	{
		if(USART3_RX_BUF[0]=='1'&&USART3_RX_BUF[1]=='2')
		{
			LED_ON();
		}
		if(USART3_RX_BUF[0]=='0'&&USART3_RX_BUF[1]=='1')
		{
			LED_OFF();
		}
		USART3_RX_STA=0;
	}
}