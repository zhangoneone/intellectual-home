#include "6050.h"
/*************************************************
函数: int getdata(void)
功能: 获取传感器数据
参数: 无
返回: 无
**************************************************/
float a[3],w[3],angle[3],T;
unsigned char Re_buf[11],temp_buf[11],counter=0;
unsigned char sign,t,he;
float ax[3];//作为系统静止时的加速度参照
float x=0.1;//阈值
int static ct=0;//统计阈值外的次数
int start=0;
int end =0;//起止时间
static unsigned char Temp[11];
char tmp[40];
static int runflag=0;//运动标志位
static int berunflag=0;
static int quitflag=0;
static int bequitflag=0;
static int quitnum=0;//静止计数器
int quitsec=2;//静止了2秒

void mpu6050_init()
{
	USART2_Config();
  NVIC_Configuration();	
}

void mpu6050_close()
{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // 失能指定的USART2接收中断
}

void mpu6050_open()
{
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能指定的USART2接收中断
}

void USART2_IRQHandler(void)		   //串口2全局中断服务函数
{
   if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
     {
      Temp[counter] = USART_ReceiveData(USART2);   //接收数据
      //网购给的程序
      //if(counter == 0 && Re_buf[0] != 0x55) return;      //第 0 号数据不是帧头，跳过
	  if(counter == 0 && Temp[0] != 0x55) return;      //第 0 号数据不是帧头，跳过
      counter++; 
      if(counter==11) //接收到 11 个数据
      { 
         mymemcpy(Re_buf,Temp,11);
         counter=0; //重新赋值，准备下一帧数据的接收
         sign=1;
      }			
   }
}

int getdata(void)
{
   unsigned char Temp[11];
	
      if(sign)
      {  
         memcpy(Temp,Re_buf,11);
         sign=0;
         if(Re_buf[0]==0x55)       //检查帧头
         {  
            switch(Re_buf[1])
            {
               case 0x51: //标识这个包是加速度包
                  a[0] = ((short)(Temp[3]<<8 | Temp[2]))/32768.0*16;      //X轴加速度
                  a[1] = ((short)(Temp[5]<<8 | Temp[4]))/32768.0*16;      //Y轴加速度
                  a[2] = ((short)(Temp[7]<<8 | Temp[6]))/32768.0*16;      //Z轴加速度
                  T    = ((short)(Temp[9]<<8 | Temp[8]))/340.0+36.25;      //温度
                  break;
               case 0x52: //标识这个包是角速度包
                  w[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*2000;      //X轴角速度
                  w[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*2000;      //Y轴角速度
                  w[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*2000;      //Z轴角速度
                  T    = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;      //温度
                  break;
               case 0x53: //标识这个包是角度包
                  angle[0] = ((short)(Temp[3]<<8| Temp[2]))/32768.0*180;   //X轴滚转角（x 轴）
                  angle[1] = ((short)(Temp[5]<<8| Temp[4]))/32768.0*180;   //Y轴俯仰角（y 轴）
                  angle[2] = ((short)(Temp[7]<<8| Temp[6]))/32768.0*180;   //Z轴偏航角（z 轴）
                  T        = ((short)(Temp[9]<<8| Temp[8]))/340.0+36.25;   //温度

                  //printf("X轴角度：%.2f   Y轴角度：%.2f   Z轴角度：%.2f\r\n",angle[0],angle[1],angle[2]);
                  break;
               default:  break;
            }
			//			printf("X角度：%.2f  Y角度：%.2f  Z角度：%.2f  X速度：%.2f  Y速度：%.2f  Z速度：%.2f\r\n",angle[0],angle[1],angle[2],w[0],w[1],w[2]);
				//		printf("x:%.2f   y:%.2f   z:%.2f\r\n",a[0],a[1],a[2]);
          //执行检测算法
//						if(monitor(ax,a)==1)//运动状态
//					 {
//						 if(runflag==0)
//						 {
//								blsend("running!\r\n");
//								runflag=1;//置1,说明发送过run信息
//							  quitflag=0;//说明现在是run状态，可以发送quit状态了
//							  bequitflag=0;
//							  berunflag=1;
//						 }
//					 }
//					 else if(monitor(ax,a)==0)//静止状态
//					 {
//						 if(quitflag==0)
//						 {
//							 blsend("quit!\r\n");
//							 quitflag=1;//quit位置1，说明发送过quit信息
//							 runflag=0;//运动位置0，说明现在是quit状态，可以发送run状态了
//							 bequitflag=1;
//							 berunflag=0;
//						 }
//					 }
//					 else if(monitor(ax,a)==-1)//待定静止状态
//					{
//						if(bequitflag==0)
//						{
//							 blsend("will quit!\r\n");
//						 	 bequitflag=1;
//							 quitflag=0;
//							 runflag=0;
//							 berunflag=0;
//						}
//					 }
//					else //待定运动状态
//					{
//						if(berunflag==0)
//						{
//							blsend("will running!\r\n");
//							 berunflag=0;
//						 	bequitflag=0;
//							 quitflag=1;
//							 runflag=0;
//						}
//					}
//      }
				if(monitorx(ax,a)==1)
				{
					 if(runflag==0)
					 {
							Lora_Printf("running!\r\n");
							runflag=1;//置1,说明发送过run信息
							quitflag=0;//说明现在是run状态，可以发送quit状态了
					 }
				}
				else
				{
					if(quitflag==0)
					{
				    Lora_Printf("quit!\r\n");
						quitflag=1;//quit位置1，说明发送过quit信息
					  runflag=0;//运动位置0，说明现在是quit状态，可以发送run状态了
					}
				}
			}
   }
	 return 0;
}

//监测算法，返回1代表运动，返回0代表静止 返回-1待定静止，返回-2待定运动
//参数1 参照数组，参数2 实时数据
//如果模块的三轴加速度在阈值范围内波动,判定静止。2秒内在阈值范围外波动，超过2次，判定运动
int monitor(float ax[],float a[])
{
	int i;
	//首次调用，开始赋值
	if(ax[0]==0&&ax[1]==0&&ax[2]==0)
	{
		for(i=0;i<3;i++)
		{
			ax[i]=a[i];
		}
	}
	//下面开始检测
	if( ax[0]-a[0]>x||ax[0]-a[0]<(-x) || ax[1]-a[1]>x||ax[1]-a[1]<(-x)  || ax[2]-a[2]>x||ax[2]-a[2]<(-x))//当加速度变化超过阈值
	{
			ct++;
			if(ct==1)//ct为1说明首次出现加速度波动
			{
				start=get_time().second;
			}
			else
			{
				end=get_time().second;
			  if(end-start<0)//时间跨越了一分钟
			  {
				  end+=60;
			  }
			}
			if(end-start>=0)	
			{	
					if(ct>0)//加速度变化了1次
					{
					  ct=0;
						start=0;
						end=0;
					for(i=0;i<3;i++){ax[i]=0;}//清零
					quitnum=0;//静止计数器清0
					return 1;
					}
					else
					{
							//不满足要求，开启下一个检测循环
						ct=0;
						start=0;
						end=0;
					for(i=0;i<3;i++){ax[i]=0;}//清零
					quitnum=0;//静止计数器清0
						return -2;
					}
			}
}
	//加速度变化没有超过阈值
	else
	{
		quitnum++;
		if(quitnum>quitsec*200)//大约是静止了3秒了
		{
			quitnum=0;
			return 0;
		}
		
	}
	return -1;
}


int monitorx(float ax[],float a[])
{
		int i;
	//首次调用，开始赋值
	if(ax[0]==0&&ax[1]==0&&ax[2]==0)
	{
		for(i=0;i<3;i++)
		{
			ax[i]=a[i];
		}
	}
	//下面开始检测
	if( ax[0]-a[0]>x||ax[0]-a[0]<(-x) || ax[1]-a[1]>x||ax[1]-a[1]<(-x)  || ax[2]-a[2]>x||ax[2]-a[2]<(-x))//当加速度变化超过阈值
	{
			ct++;
			if(ct==1)//ct为1说明首次出现加速度波动
			{
				start=get_time().second;
			}
			else
			{
				end=get_time().second;
			  if(end-start<0)//时间跨越了一分钟
			  {
				  end+=60;
			  }
			}
			if(end-start>=0)	
			{	
					if(ct>0)//加速度变化了1次
					{
					  ct=0;
						start=0;
						end=0;
					for(i=0;i<3;i++){ax[i]=0;}//清零
					quitnum=0;//静止计数器清0
					return 1;
					}
					else
					{
							//不满足要求，开启下一个检测循环
						ct=0;
						start=0;
						end=0;
					for(i=0;i<3;i++){ax[i]=0;}//清零
					quitnum=0;//静止计数器清0
						return 1;
					}
				}
			return 1;
			}
	//加速度变化没有超过阈值
	else
	{
		quitnum++;
		if(quitnum>quitsec*200)//大约是静止了3秒了
		{
			return 0;
		}
		return 1;
	}
	return 1;
}


void mymemcpy(unsigned char * Re_buf,unsigned char * Temp,int count)
{
	int i=0;
	for(;i<count;i++)
	{
		Re_buf[i]=Temp[i];
	}
	
}

