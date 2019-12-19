/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "usart.h"
#include "sys.h"  
#include "stm32f10x_usart.h"
#include "string.h"
#include "delay.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 



//需要设置串口接收中断和定时器3中断，中断时间为1ms
//------------------------------------------------------------------
//函数名：void USART1_IRQHandler(void)
//输入参数：null
//返回参数：null
//说明：串口接收中断服务
//------------------------------------------------------------------
//void USART1_IRQHandler(void)
//{
//   u8 rx_data;
//  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)          //判断读寄存器是否非空
//  {      
////    GPIO_SetBits(GPIOB,GPIO_Pin_6);
//    rx_data[RbufCounter++]=USART_ReceiveData(USART1);    //接收字节到接收缓冲区
//    if(USART_Rsv_Status==0)
//    {
//      if(RbufCounter>1)
//      {
//        if(rx_data[0]==0xA5&&rx_data[1]==0x5A)    //当接收到的数据帧头两个字节同时为0xA5和0x5A时
//        {
//          USART_Rsv_Status=1;
////           USART_SendData(USART1, rx_data[0]);
//        }
//        else
//        {
//          rx_data[0]=rx_data[1];
//          RbufCounter=1;
//         
//        }
//      }
//    }
//    else
//    {
//      USART_1ms_Cnt=0;
//    }               
//  }         
//}

//可以发送和接收一次错误数据
//void USART1_IRQHandler(u8 GetData)
//{ 
//u8 BackData;
//if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //中断产生
//{  
//			USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志.
//			GetData=USART1->DR;  	
//		//	GetData = UART1_GetByte(BackData);   //也行GetData=USART1->DR;   
//			printf("%d",GetData);
//	//	USART1_SendByte(GetData);      //发送数据
//		
//			LED_ON();
//			delay_ms(1000);
//			LED_OFF();
//} 
//}

//可以发送和接收一次正确数据
//void USART1_IRQHandler() 
// { 
//		u8 RX_dat;                                //定义字符变量
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //判断发生接收中断
//		{
//   RX_dat=(USART_ReceiveData(USART1) & 0x7F);      //接收数据，整理除去前两位                                     
//   USART_ClearITPendingBit(USART1,  USART_IT_RXNE);          //清除中断标志
//		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET){}//等待接收结束
////   USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);          //关中断
//       USART_SendData(USART1,RX_dat);             //发送数据
//		}
//			LED_ON();      //LED M
//			delay_ms(1000);
//			LED_OFF();
//}


//------------------------------------------------------------------
//函数名：void TIM2_IRQHandler(void)
//输入参数：null
//返回参数：null
//说明：定时器2中断服务
//------------------------------------------------------------------
void TIM2_IRQHandler(void)
{
 
}
//------------------------------------------------------------------
//函数名：void TIM3_IRQHandler(void)
//输入参数：null
//返回参数：null
//说明：定时器3中断服务
//------------------------------------------------------------------
//void TIM3_IRQHandler(void)
//{ 
//  if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)        //判断是否为定时器3溢出中断
//  {
//    
//    GPIO_SetBits(GPIOB,GPIO_Pin_6);
//    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清中断标记
//    if(USART_Rsv_Status==1)
//    USART_1ms_Cnt++;
//    if(USART_1ms_Cnt>5)     
//    {
////      USART_SendData(USART1,0xAA);
//      USART_Rsv_Status=0;     //连续计数超过5次对USART_Rsv_Status置0，继续等待接收
//      USART_1ms_Cnt=0;         //当USART_1ms_Cnt>5时对USART_1ms_Cnt重新清零 
//      if(RbufCounter==(u16)rx_data[4]+7)              //检验数据的完整性
//      {
//         int i;     //定义循环变量
//        int j;
//        data_length=rx_data[4];
//        for(i=0;i
//        {
//          data[i]=rx_data[i];
//        } 
//        CRC_data_Hi=rx_data[RbufCounter-1];
//        CRC_data_Lo=rx_data[RbufCounter-2];
//        CRC_data=CRC16((unsigned char*)data,data_length+5);
//        CRC_data_Hi1=CRC_data>>8;
//        CRC_data_Lo1=CRC_data&0x00ff;
//         if(CRC_data_Hi==(u8)CRC_data_Hi1 && CRC_data_Lo==CRC_data_Lo1)
//         {
//           for(j=0;rx_data[j]!='\0';j++)   //循环逐字输出，到结束字'\0'
//           {       
//             USART_SendData(USART1, rx_data[j]);     //发送字符
//             while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
//             {
//             } //等待字符发送完毕
//           }
//        }
//      }
//      RbufCounter=0;
//    }     
//  }
//}

//void USART3_IRQHandler(void)		   //串口3全局中断服务函数
//{
//   if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
//     {
//      Temp[counter] = USART_ReceiveData(USART3);   //接收数据
//      //网购给的程序
//      //if(counter == 0 && Re_buf[0] != 0x55) return;      //第 0 号数据不是帧头，跳过
//	  if(counter == 0 && Temp[0] != 0x55) return;      //第 0 号数据不是帧头，跳过
//      counter++; 
//      if(counter==11) //接收到 11 个数据
//      { 
//         mymemcpy(Re_buf,Temp,11);
//         counter=0; //重新赋值，准备下一帧数据的接收
//         sign=1;
//      }			
//   }
//}
//void USART3_IRQHandler(void)		   //串口3全局中断服务函数
//{
//   if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
//     {
//      Temp[counter] = USART_ReceiveData(USART3);   //接收数据
//			 printf("%d",Temp[counter]);
//     }			
//}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
