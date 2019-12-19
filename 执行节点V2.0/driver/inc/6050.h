
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "led.h"
#include "delay.h"
#include "usart.h"

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
//#include "uart_1.h"
//#include "dma.h"
//#include "RS485.h"
//#include "74hc595.h"
#include "string.h"
#include "rtc_time.h"

/**************************
模板配置信息：
选用芯片是 stm32f103vc
启动信息：stm32f10x_hd.s
固件库：stm32固件库v3.5
***************************/

#define led_PB12   PBout(12)	   //位绑定，位定义PB12口作为LED输出。这样速度最快
extern float a[3],w[3],angle[3],T;
extern unsigned char Re_buf[11],temp_buf[11],counter;
extern unsigned char sign;
void mpu6050_init();
void mpu6050_close();
void mpu6050_open();
int getdata(void);
int monitor(float *ax,float *a);//监测算法
void mymemcpy(unsigned char * Re_buf,unsigned char * Temp,int count);