/**
  ************************************
  * @file    delay.c
  * @author  wszdxmh
  * @version V3.5.0
  * @date    2015.08.08
  * @brief   Systick时钟延时函数
	************************************
*/
#include "delay.h"
#include "sys.h"

static uint8_t  fac_us=0;//us延时倍乘数
static uint16_t fac_ms=0;//ms延时倍乘数


//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
/*void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8  //1MHZ???
	fac_us=SystemCoreClock/8000000;	//为系统时钟的1/8  	 
	fac_ms=(u16)fac_us*1000;//非ucos下,代表每个ms需要的systick时钟数   
}	*/							    
void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8  //1MHZ???
	fac_us=SystemCoreClock/8000000;	//为系统时钟的1/8  	 
	fac_ms=(u16)fac_us*1000;//非ucos下,代表每个ms需要的systick时钟数   
}	
/**
  * @brief  延时 n us
  * @param  us: specifies the delay time length.
  * @retval None
  */
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
}
void delay_initx(u8 SYSCLK)
{
    SysTick->CTRL&=0xfffffffb;//bit2清空,选择外部时钟  HCLK/8
    fac_us=SYSCLK/8;    	    
    fac_ms=(u16)fac_us*1000;
}  

/**
  * @brief  延时 n ms
  * @param  ms: specifies the delay time length.
  * @retval None
  */
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
}
