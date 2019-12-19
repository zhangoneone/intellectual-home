/****************************************Copyright (c)****************************************************
** 执行节点usart3（lora）-->汇聚节点usart2（lora）                                     
** 汇聚节点usart3（wifi）发给路由器                                
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			
** Last modified Date:          
** Last Version:		   
** Descriptions:							
**--------------------------------------------------------------------------------------------------------
** Created by:			FiYu
** Created date:		2015-12-15
** Version:			    1.0
** Descriptions:		ESP8266 WiFi无线通信实验				
**--------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:		
** Version:				
** Descriptions:		
** Rechecked by:			
**********************************************************************************************************/
 
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "lora.h"
 
 /**************************************************************************************
 * 描  述 : GPIO_LED初始化
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void LED_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 						 
		
  GPIO_DeInit(GPIOB);	 //将外设GPIOB寄存器重设为缺省值
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB , GPIO_Pin_9);   //初始状态，熄灭指示灯
}

/**************************************************************************************
 * 描  述 : MAIN函数
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
int main ( void )
{
	LED_Configuration ();
  USARTx_Config ();                                        //初始化串口1
	USART1_Init();
	SysTick_Init ();                                         //配置 SysTick 为 1ms 中断一次 
	ESP8266_Init ();                                         //初始化WiFi模块使用的接口和外设
	lora_init();																						//初始化lora串口 
  printf("sink node\r\n");
  ESP8266_StaTcpClient_UnvarnishTest ();
  while (1)
	{
		;
	}
	
	
}


/*********************************************END OF FILE**********************/
