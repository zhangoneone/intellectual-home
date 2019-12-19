#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>
//#include "clock.h"
//#include "procinit.h"
//#include "etimer.h"
//#include "autostart.h"
#include "usart.h"
#include "led.h"
#include "delay.h"
#include "rtc_time.h"
#include "sys.h"
#include "lora.h"

void make(char bufrec[]);
void chtime(rtc_time_t cur_time);//设置系统时间
void sendtime();