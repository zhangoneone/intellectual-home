#ifndef __COMMON_H
#define __COMMON_H



#include "stm32f10x.h"



/******************************* 宏定义 ***************************/
#define            macNVIC_PriorityGroup_x                     NVIC_PriorityGroup_2



/********************************** 函数声明 ***************************************/
void                     USART_printf                       ( USART_TypeDef * USARTx, char * Data, ... );

void usart2_SendByte(unsigned char SendData);
void usart3_SendByte(unsigned char SendData);

#endif /* __COMMON_H */

