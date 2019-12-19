#ifndef __SGP18T_ILI9163B_BASE_H__
#define __SGP18T_ILI9163B_BASE_H__

#include "stm32f10x_conf.h"
#include "ili9163b-delay.h"

/*
 *  PD10 --> TFT_BL_ON/OFF	
 *  PD9 --> TFT_ON/OFF
 *  PD8 --> TFT_RST
 *  PB15 --> TFT_AO
 *  PB14 --> TFT_SI
 *  PB13 --> TFT_SCL
 *  PB12 --> TFT_CS
 */

#define		ON		1
#define		OFF		0
     
#define         CMD_RAMWR               0x2C
#define         CMD_CASET               0x2A
#define         CMD_RASET               0x2B

#define         CMD_MADCTL              0x36
     
#define         CMD_READID1             0xDA
#define         CMD_READID2             0xDB
#define         CMD_READID3             0xDC

/*******************??????******************************/


#define	        ILI9163_RESET_SET()		{GPIO_SetBits(GPIOD, GPIO_Pin_8);}
#define	        ILI9163_RESET_CLR()		{GPIO_ResetBits(GPIOD, GPIO_Pin_8);}

#define	        ILI9163_BL_ON()		        {GPIO_SetBits(GPIOD, GPIO_Pin_10);}
#define	        ILI9163_BL_OFF()		{GPIO_ResetBits(GPIOD, GPIO_Pin_10);}

#define	        ILI9163_POWER_ON()		{GPIO_SetBits(GPIOD, GPIO_Pin_9);}
#define	        ILI9163_POWER_OFF()		{GPIO_ResetBits(GPIOD, GPIO_Pin_9);}

#define	        ILI9163_A0_SET()		{GPIO_SetBits(GPIOB, GPIO_Pin_15);}
#define	        ILI9163_A0_CLR()		{GPIO_ResetBits(GPIOB, GPIO_Pin_15);}

#define	        ILI9163_CS_SET()		{GPIO_SetBits(GPIOB, GPIO_Pin_12);}
#define	        ILI9163_CS_CLR()		{GPIO_ResetBits(GPIOB, GPIO_Pin_12);}

#define         ILI9163_SCL_SET()		{GPIO_SetBits(GPIOB, GPIO_Pin_13);}
#define	        ILI9163_SCL_CLR()		{GPIO_ResetBits(GPIOB, GPIO_Pin_13);}

#define	        ILI9163_SDA_SET()		{GPIO_SetBits(GPIOB, GPIO_Pin_14);}
#define	        ILI9163_SDA_CLR()		{GPIO_ResetBits(GPIOB, GPIO_Pin_14);}

#define	        ILI9163_SDA_ReadInput()		 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)

#define         ILI9163_ID              0x548066
#define         ST7735_ID               0x7c88f0

/*****************私有函数声名*********************************/

void ILI9163B_init(void);					//LCD初始化
uint32_t ILI9163B_get_lcd_id(void);
void ILI9163B_write_command(uint8_t cmd);			//发送控制字
void ILI9163B_write_para8(uint8_t dat);				//发送数据参数
void ILI9163B_write_para16(uint16_t dat);			//发送像素显示参数
void ILI9163B_address_rst(void);				//DDRAM地址重
void ILI9163B_SetPos(uint8_t xs,uint8_t ys,uint8_t xe,uint8_t ye);	//定位显示像素位置

#endif /*SGP18T_ILI9163B.h*/
