#include "contiki.h"
#include "usart.h"
#include "led.h"
#include "stdio.h"
#include "hx711drv.h"
#include "6050.h"
#include "sys.h"
#include "rtc_time.h"
#include "bluetooth.h"
#include "sht.h"

PROCESS(tcp_keep, "tcp心跳检测,定时发送");//60s一次

PROCESS(mpu6050, "触动检测线程,后台长开");//进程声明

PROCESS(hx711, "压力检测线程，后台长开");//进程声明

PROCESS(sht_send, "温湿度定时更新线程");//30s一次

AUTOSTART_PROCESSES(&tcp_keep,&mpu6050,&hx711,&sht_send);


PROCESS_THREAD(tcp_keep, ev, data)
{
	  static struct etimer et;
    PROCESS_BEGIN();
	while(1)
	{
		etimer_set(&et, CLOCK_SECOND*60);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		Lora_Printf("hello!\r\n");
	}
   PROCESS_END();
}

PROCESS_THREAD(mpu6050, ev, data)
{
	   static struct etimer et;
		PROCESS_BEGIN();
while(1)
{
		etimer_set(&et, CLOCK_SECOND/200);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		getdata();
}
   PROCESS_END();
}

PROCESS_THREAD(hx711, ev, data)
{
	static struct etimer et;
  PROCESS_BEGIN();
		while(1)
		{
		sendchange();//如压力改变，则发送警报
		etimer_set(&et, CLOCK_SECOND/5);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		}
   PROCESS_END();
}

PROCESS_THREAD(sht_send, ev, data)
{
	static struct etimer et;
  PROCESS_BEGIN();
		while(1)
		{
		sendsht();//提交温湿度信息
		etimer_set(&et, CLOCK_SECOND*30);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		}
   PROCESS_END();
}


