#include "stm32f10x.h"
#include "usart.h"

#define RX_BUF_MAX_LEN     1024                                     //最大接收缓存字节数

#pragma anon_unions
extern struct  STRUCT_USARTx_Fram                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	
  union {                                   //这里把union改成了struct
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
 }; 
	
} strEsp8266_Fram_Record;


extern u8 USART3_TX_BUF[1024]; 	//发送缓冲,最大1024字节,要发送给wifi模块的

extern struct STRUCT_USARTx_Fram Lora_Record;

void lora_init();
void USART3_Config(unsigned bound);   																			//初始化lora串口
void USART3_NVIC_Configuration();																									//配置lora串口NVIC中断
void Lora_Printf(char *send);                                                   //数据输出到lora
void handle_lora();
char* lora_get();
void lora_ctr(char num);