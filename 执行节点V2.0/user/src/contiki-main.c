/**
contiki移植
chen li  
**/
#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>
#include "clock.h"
#include "process.h"
#include "procinit.h"
#include "etimer.h"
#include "autostart.h"
#include "usart.h"
#include "led.h"
#include "delay.h"
#include "rtc_time.h"
#include "hx711drv.h"
#include "lora.h"

unsigned int idle_count = 0;

static void platform_init(void)
{
	LED_Init();
	delay_init();
	NVIC_Configuration();//串口中断
	USART1_Config();
	rtc_configuration();
	nvic_init();//时钟中断
	rtc_init();
	rtc_to_time();
	Beep_Init();
	HX711_init();
	lora_init();
//	bluetooth_init();
	mpu6050_init();
}

int main()
{
	platform_init();
	printf("contiki version 2.5 by zq\r\n");
  printf("Loading...\r\n");
  clock_init();
  process_init();
	printf("Loading Successful...\r\n");
	Lora_Printf("system start ready...\r\n");
  process_start(&etimer_process, NULL);
  autostart_start(autostart_processes);
  while(1) {
    do 
    {
    } 
    while(process_run() > 0);
    idle_count++;
    /* Idle! */
    /* Stop processor clock */
    /* asm("wfi"::); */ 
  }
  return 0;
}
