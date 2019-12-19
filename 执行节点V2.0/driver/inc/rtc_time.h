#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_
#include "stm32f10x.h"


typedef struct _rtc_time_t
{
    uint32_t year;
    uint8_t month;
    uint8_t day;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} rtc_time_t ;

void  rtc_init(void);
void set_rtc_time(rtc_time_t* time);
rtc_time_t get_time(void);
#endif //_RTC_TIME_H_