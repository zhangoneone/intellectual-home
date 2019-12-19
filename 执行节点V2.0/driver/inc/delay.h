/**
  ************************************
  * @file    delay.h
  * @author  Á—ª‘
  * @version V3.5.0
  * @date    2015.08.08
  * @brief   Systick ±÷”—” ±∫Ø ˝
	************************************
*/
#ifndef __DELAY_H
#define __DELAY_H

#include "sys.h"


void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif
