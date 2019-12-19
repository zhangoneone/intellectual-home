//PB12>beep
#include "buzzer.h"
 /************************************
  * @brief  蜂鸣器灯IO口的初始化
  * @param  None
  * @retval None
	*************************************/
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure1;//定义结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//外部时钟使能

	//PB12 config
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_Out_PP;//设置为普通推挽输出
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;//频率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure1);//初始化
}

void BEEP_ON()
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//置1
}

void BEEP_OFF()
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);//置0
}