#ifndef  __TEST_H
#define	 __TEST_H



#include "stm32f10x.h"
#include "bsp_esp8266.h"



/********************************** 用户需要设置的参数**********************************/
#define      macUser_ESP8266_BulitApSsid                  "iuni u3"         //要建立的热点的名称
#define      macUser_ESP8266_BulitApEcn                   WPA2 PSK        //要建立的热点的加密方式
#define      macUser_ESP8266_BulitApPwd                   "iuni u3"         //要建立的热点的密钥

#define      macUser_ESP8266_CwjServer_M                 "iuni u3"  //建立热点的名称
#define      macUser_ESP8266_CwjServer_N                 "12345678"  //建立热点的密码




#define      macUser_ESP8266_TcpServer_Enum               enumTCP    //协议类型
#define      macUser_ESP8266_TcpServer_IP                 "192.168.2.1"    //服务器开启的IP地址
#define      macUser_ESP8266_TcpServer_Port               "20002"             //服务器开启的端口   

#define      macUser_ESP8266_pStr               "zhangqing"             //发送内容 
#define      macUser_ESP8266_ulStrLength         8             //发送字节数



#define      macUser_ESP8266_TcpServer_OverTime           "1800"             //服务器超时时间（单位：秒）



/********************************** 测试函数声明 ***************************************/
void                     ESP8266_StaTcpClient_UnvarnishTest  ( void );



#endif

