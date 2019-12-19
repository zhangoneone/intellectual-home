//PB13 -> ADDO 这个产生转换完成的中断信号(输入模式)数据线
//PB14 -> ADSK 这个拉高开始读取数据(输出模式)时钟线


#include "stm32f10x.h"
#include "hx711drv.h"
static int kg;

int get_kg()//获得压力大小
{
		unsigned long adc;
		adc=ReadCount();
		adc/=100000;
		kg=adc-78;
		return kg;
}
void sendchange()//发送压力改变警报
{
			unsigned long adc;
	//		char dis[30];
			adc=ReadCount();
	//			sprintf(dis,"%ld\r\n",adc);
	 //   Lora_Printf(dis);
	//	if(adc>9000000||adc<8500000)
			adc/=100000;
		if(78+kg!=adc&&78-kg!=adc)
		{
		Lora_Printf("pressed have changed!\r\n");
			printf("当前adc值:%d\r\n",adc);
			printf("当前kg:%d\r\n",kg);
	   	kg=adc-78;
			printf("下次参考的kg:%d\r\n",kg);
			
		}
}

void sendpress()//发送压力值的大约值，单位kg
{
	char dis[30];
		sprintf(dis,"pressed is about %.2f kg\r\n",get_kg());
		Lora_Printf(dis);
}

void HX711_init(void)
{
  MYGPIO_Init();
  HX711_ADDOIn();
  HX711_ADSKOut();
}

unsigned long ReadCount(void)
{
  unsigned long Count;
  unsigned char i;
  HX711_init();

  ADSK_L();//输出模式，低电平

  Count=0;
  while(READADDO()); //AD转换未结束则等待，否则开始读取
  for (i=0;i<24;i++)
  {
    ADSK_H();
    Count=Count<<1; //下降沿来时变量Count左移一位，右侧补零
    ADSK_L();
    if(READADDO()) Count++;
  }
  ADSK_H();
  Count=Count^0x800000;//第25个脉冲下降沿来时，转换数据
  ADSK_L();
  return(Count);
}


//初始化IO口
void MYGPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure1;//定义结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//GPIOE外部时钟使能，特别重要！！！

	//PB13 config
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;//设置为普通推挽输出
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;//频率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure1);//初始化
	
		//PB14 config
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;//设置为普通推挽输出
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;//频率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure1);//初始化
}

//ADDO 配置为输入状态

void HX711_ADDOIn(void)
{
		GPIO_InitTypeDef GPIO_InitStructure1;//定义结构体
	//PB14 config
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN_FLOATING;//设置为普通浮空输入
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;//频率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure1);//初始化
}

//ADDO 配置为输出状态
void HX711_ADDOOut(void)
{
	GPIO_InitTypeDef GPIO_InitStructure1;//定义结构体
	//PB14 config
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;//设置为普通输出
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;//频率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure1);//初始化
}

//ADSK配置为输入模式
void HX711_ADSKIn(void)
{
	GPIO_InitTypeDef GPIO_InitStructure1;//定义结构体
	//PB14 config
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IN_FLOATING;//设置为普通浮空输入
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;//频率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure1);//初始化
}

// ADSK配置为输出状态

void HX711_ADSKOut()
{
	GPIO_InitTypeDef GPIO_InitStructure1;//定义结构体
	//PB14 config
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;//设置为普通输出
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;//频率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure1);//初始化
}