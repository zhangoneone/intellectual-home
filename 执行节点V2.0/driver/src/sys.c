/**
  ************************************
  * @file    sys.c
  * @author  wszdxmh
  * @version V3.5.0
  * @date    2015.08.08
  * @brief   中断分组函数
	************************************
*/
#include "sys.h"

void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//配置系统时钟
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;                    //定义外部高速晶体启动状态枚举变量

	RCC_DeInit();                                    //复位RCC外部设备寄存器到默认值
	RCC_HSEConfig(RCC_HSE_ON);                       //打开外部高速晶振
	HSEStartUpStatus = RCC_WaitForHSEStartUp();      //等待外部高速时钟准备好
	if(HSEStartUpStatus == SUCCESS)                  //外部高速时钟已经准别好
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //开启FLASH预读缓冲功能，加速FLASH的读取。所有程序中必须的用法.位置：RCC初始化子函数里面，时钟起振之后
		FLASH_SetLatency(FLASH_Latency_2);                    //flash操作的延时
			
		RCC_HCLKConfig(RCC_SYSCLK_Div1);               //配置AHB(HCLK)时钟等于==SYSCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);                //配置APB2(PCLK2)钟==AHB时钟
		RCC_PCLK1Config(RCC_HCLK_Div2);                //配置APB1(PCLK1)钟==AHB1/2时钟
			 
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //配置PLL时钟 == 外部高速晶体时钟 * 9 = 72MHz
		RCC_PLLCmd(ENABLE);                                   //使能PLL时钟
   
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)    //等待PLL时钟就绪
		{
		}
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);            //配置系统时钟 = PLL时钟
		while(RCC_GetSYSCLKSource() != 0x08)                  //检查PLL时钟是否作为系统时钟
		{
		}
	}
}

//IO口配置
//用法：用哪个打开那个
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;   //定义名为GPIO_InitStructure的结构体类型变量

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //打开GPIOB的时钟，并使能。
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //打开GPIOC的时钟，并使能。
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);  //打开GPIOD的时钟，并使能。
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //打开GPIOA的时钟，并使能。
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);  //打开GPIOB和GPIOD的时钟，并使能。
	//配置PD10口：
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;              //选定哪个IO口 现选定PD9口和PD10口
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;                       //设定IO口的模式为上拉输入
//	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	                 //如果IO口的模式是输入，GPIO_Speed可以不赋值
//	GPIO_Init(GPIOD, &GPIO_InitStructure);	/* PC6 */                    //初始化GPIOD

	//配置PA口：
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;              //选定哪个IO口 现选定PA0口和PA1口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		             //设定IO口的模式为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	                 //设定IO口的输出速度为2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	/* PC6 */                    //初始化GPIOD
	
	//配置PD口：
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;              //选定哪个IO口 现选定PD0123口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		             //设定IO口的模式为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	                 //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure);	                             //初始化GPIOD

	//配置PD口：
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;              //选定哪个IO口 现选定PD2口
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		             //设定IO口的模式为推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	                 //设定IO口的输出速度为2MHz
//	GPIO_Init(GPIOD, &GPIO_InitStructure);	/* PC6 */                    //初始化GPIOD

	//配置3个LED灯：PB12、PB13、PB14：
	GPIO_SetBits(GPIOB,  GPIO_Pin_0 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);	         //关闭3个LED灯：PB12、PB13、PB14，输出高电平
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;	             //选定PB12、PB13、PB14
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		             //设定IO口的模式为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		             //设定IO口的输出速度为2MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					             //初始化GPIOB
}

//中断IO口配置
//用法：用哪个打开那个
void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;						   //定义一个结构体变量

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 		   //打开APB2总线复用时钟
	
	//外部中断线10配置：
	EXTI_ClearITPendingBit(EXTI_Line10);						   //清除中断挂起位 外部中断线10
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		       //选择是中断触发还是事件触发，现在选择的是中断触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	       //触发方式：现在选择的是下降沿触发
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;				       //选择哪个IO口作为中断输入，现在选择的是PD10口
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					   //使能中断
	EXTI_Init(&EXTI_InitStructure);							       //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource10);   //选择GPIO管脚作为外部中断线路
	//外部中断线9配置：
	EXTI_ClearITPendingBit(EXTI_Line9);						       //清除中断挂起位 外部中断线9
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		       //选择是中断触发还是事件触发，现在选择的是中断触发
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	       //触发方式：现在选择的是下降沿触发
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;				       //选择哪个IO口作为中断输入，现在选择的是PD9口
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					   //使能中断
	EXTI_Init(&EXTI_InitStructure);							       //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource9);    //选择GPIO管脚作为外部中断线路
}

//中断优先级配置
//用法：用哪个打开那个
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure; 					   //定义用于配置中断的结构体变量

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                //中断优先级分组  抢占式优先级别设置为2位；响应优先级占2位
	
	//外部中断线10优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;           //指定中断源
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	//外部中断线9优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;             //指定中断源  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	   //抢占优先级 暂时没有
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             //指定响应优先级别2
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	//串口1中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;              //通道设置为串口1中断  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	//串口1用DMA接收中断优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;       //通道设置为串口DMA通道4中断  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
//	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	//串口2中断优先级设置(RS485)
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			   //指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	//串口3中断优先级设置(RS485)
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			   //指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	   //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             //指定响应优先级别2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

//	//串口1用DMA接收优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;       //DMA1通道5中断 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
//	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

//	//串口4中断优先级设置(RS485)
//	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;			   //指定中断源
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
//	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

//	//串口5中断优先级设置(RS485)
//	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;			   //指定中断源
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
//	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	
	//通用定时器2中断优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器	
	
	//通用定时器3中断优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
	//通用定时器4中断优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器	
	
	//通用定时器5中断优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM5中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器	
	
	//通用定时器6中断优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM6中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器	
	
	//通用定时器7中断优先级设置
//	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //TIM7中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //抢占优先级1级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级3级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器					 
}
