#include "makefile.h"

const char *gettime="get time";
const char *openled="open led";
const char *closeled="close led";
const char *settime="set time";
void make(char bufrec[])
{
	if(com_str(settime,bufrec))
	{
  	cur_time.year=(bufrec[9]-48)*1000+(bufrec[10]-48)*100+(bufrec[11]-48)*10+bufrec[12]-48;
		cur_time.month=(bufrec[14]-48)*10+bufrec[15]-48;
		cur_time.day=(bufrec[17]-48)*10+bufrec[18]-48;
		cur_time.date=bufrec[20]-48;
		cur_time.hour=(bufrec[22]-48)*10+bufrec[23]-48;
		cur_time.minute=(bufrec[25]-48)*10+bufrec[26]-48;
		cur_time.second=(bufrec[28]-48)*10+bufrec[29]-48;
		set_rtc_time(&cur_time);
		sendtime();
	//	chtime(cur_time);
	}
	if(com_str(gettime,bufrec))
	{
		static rtc_time_t Real_Time;
		Real_Time=get_time();
		printf("%d.%d.%d  %d:%d:%d  \r\nroot#",Real_Time.year,Real_Time.month,Real_Time.day,Real_Time.hour,Real_Time.minute,Real_Time.second);
	}
	if(com_str(openled,bufrec))
	{
		LED_ON();
	}
	if(com_str(closeled,bufrec))
	{
		LED_OFF();
	}
}

int com_str(char *str,char bufrec[])//1相等
{
	int i=0;
	if((*str)=='\0'||bufrec[0]=='\0')
		return 0;
		while((*str)!='\0')
		{
			if((*str)!=bufrec[i])
				return 0;
			else
			{
				i++;
				str++;
				continue;
			}
		}
			return 1;
}

void chtime(rtc_time_t time)//设置系统时间
{
	  static rtc_time_t cur_time;
		cur_time.year=time.year;
		cur_time.month=time.month;
		cur_time.day=time.day;
		cur_time.hour=time.hour;
		cur_time.minute=time.minute;
		cur_time.second=time.second;
		set_rtc_time(&cur_time);
}
void sendtime()
{
	char dis[60];
	sprintf(dis,"current time:%d.%d.%d  %d:%d:%d\r\n",get_time().year,get_time().month,get_time().day,get_time().hour,get_time().minute,get_time().second);
	Lora_Printf(dis);
}
