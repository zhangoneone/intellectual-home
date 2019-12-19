#include "stm32f10x.h"
#include "common.h"
#include "bsp_esp8266.h"

extern u8 USART2_TX_BUF[1024]; 	//发送缓冲,最大1024字节,要发送给wifi模块的

extern struct STRUCT_USARTx_Fram Lora_Record;

void lora_init();
void USART2_Config(unsigned bound);   																			//初始化lora串口
void USART2_NVIC_Configuration();																									//配置lora串口NVIC中断
void Lora_Printf(char *send);                                                   //数据输出到lora