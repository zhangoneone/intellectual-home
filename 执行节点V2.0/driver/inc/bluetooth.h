#include "stm32f10x.h"
#include "6050.h"

void bluetooth_init();
char* blrec();
void blsend(u8 *send);
int blcontrol(char num);
//void sendtime();
void testblue(void);

#define USART3_REC_LEN  200  	//定义最大接收字节数 200
extern u8  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节
extern u16 USART3_RX_STA;         		//接收状态标记	 0-14位为接收字节数，15位接收完成标志位