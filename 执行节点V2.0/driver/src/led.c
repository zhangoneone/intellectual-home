#include "led.h"
/**硬件连接：PC10->D3 PC11->D4 PC12->D5 PD2->D6 PA8->D1
  ************************************
  * @brief  Led灯IO口的初始化
  * @param  None
  * @retval None
	************************************
*/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 
	
	GPIO_DeInit(GPIOB);	 //将外设GPIOB寄存器重设为缺省值		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_9);    //将PB9引脚置高,初始状态为熄灭指示灯
}

/*************************
打开led灯
***************************/
void LED_ON(void)
{
	GPIO_ResetBits(GPIOB , GPIO_Pin_9);   //将PB9引脚置低,点亮指示灯
}
/*************************************
关闭led灯
*********************************/
void LED_OFF(void)
{

	GPIO_SetBits(GPIOB , GPIO_Pin_9);    //将PB9引脚置高,熄灭指示灯
}
