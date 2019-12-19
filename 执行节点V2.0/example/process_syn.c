#include "contiki.h"
#include "usart.h"

static process_event_t event_data_ready;

 /*声明两个进程*/
 PROCESS(print_hello_process, "Hello");
 PROCESS(print_world_process, "world");
AUTOSTART_PROCESSES(&print_hello_process, &print_world_process); //让该两进程自启动
 /*定义进程 print_hello_process*/
 PROCESS_THREAD(print_hello_process, ev, data)
 {
	 static struct etimer timer;
	 	 	static int testnum=0;
 PROCESS_BEGIN();
 etimer_set(&timer, CLOCK_CONF_SECOND); //#define CLOCK_CONF_SECOND 10 将 timer 的 interval 设为 1 详情见

 printf("***print hello process start***\n");

 event_data_ready = process_alloc_event(); //return lastevent++; 新建一个事件，事实上是用一组 unsigned char值来标识不同事件

 while (1)
 {
 PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER); //详情见 4.2，即 etimer 到期继续执行下面内容

// printf("Hello\n");

	 testnum++;
 process_post(&print_world_process, event_data_ready, &testnum); //传递异步事件给 print_world_process，直到内核调度该进程才处理该事件。详情见 4.3

 etimer_reset(&timer); //重置定时器，详情见 4.4
 }

 PROCESS_END();
 }
 /*定义进程 print_world_process*/

 PROCESS_THREAD(print_world_process, ev, data)
 {
 PROCESS_BEGIN();

 printf("***print world process start***\n");

 while (1)
 {
 PROCESS_WAIT_EVENT_UNTIL(ev == event_data_ready);
 //printf("world\n");
	 printf("%d\n",(*(int *)data));
 }

 PROCESS_END();
}