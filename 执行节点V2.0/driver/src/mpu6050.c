
#include "mpuiic.h"
#include "mpu6050.h"
#include "mpu6050reg.h"

/**
  **********************************
  * @brief  MPU6050初始化
	* @param  None
  * @retval 0:成功初始化
  **********************************
*/
unsigned char MPU6050_Init(void)
{
	unsigned char res, d;
	unsigned char aaa;
	MPU_IIC_Init();
	aaa=MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80);	//复位MPU6050
	printf("one:%d\n",aaa);
	delay_ms(100);
	aaa=MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00);	//唤醒MPU6050
	printf("two:%d\n",aaa);
	
	aaa=MPU_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
	printf("three:%d\n",aaa);
	
	aaa=MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
	printf("four:%d\n",aaa);
	
	aaa=MPU_Set_Rate(50);						//设置采样率50Hz
	printf("five:%d\n",aaa);
	aaa=MPU_Write_Byte(MPU_INT_EN_REG, 0X00);	//关闭所有中断
	printf("six:%d\n",aaa);
	aaa=MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00);	//I2C主模式关闭
	printf("seven:%d\n",aaa);
	aaa=MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);	//关闭FIFO
	printf("eight:%d\n",aaa);
	aaa=MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80);	//INT引脚低电平有效
	printf("nine:%d\n",aaa);
	res = MPU_Read_Byte(MPU_DEVICE_ID_REG);
	printf("res:%d\n",aaa);
	aaa=MPU_Set_LPF(50);
	printf("eleven:%d\n",aaa);
//	if (res == MPU_ADDR && d == 0)//器件ID正确  //此处出错
	if (res == MPU_ADDR)//MPU_ADDR=0x68
	{
		aaa=MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01);	//设置CLKSEL,PLL X轴为参考
		printf("twele:%d\n",aaa);
		aaa=MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00);	//加速度与陀螺仪都工作
		printf("thirty:%d\n",aaa);
		aaa=MPU_Set_Rate(50);						//设置采样率为50Hz
		printf("fourty:%d\n",aaa);
	}
	else
		return 1;
	return 0;
}

/**
  **********************************
  * @brief  设置MPU6050陀螺仪满量程范围
	* @param  fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
  * @retval 0：成功
  **********************************
*/
unsigned char MPU_Set_Gyro_Fsr(unsigned char fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}

/**
  **********************************
  * @brief  设置MPU6050加速度计满量程范围
	* @param  fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
  * @retval 0：成功
  **********************************
*/
unsigned char MPU_Set_Accel_Fsr(unsigned char fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}

/**
  **********************************
  * @brief  设置MPU6050数字低通滤波器
	* @param  lpf:数字低通滤波频率(Hz)
  * @retval 0：成功
  **********************************
*/
unsigned char MPU_Set_LPF(uint16_t lpf)
{
	unsigned char data = 0;
	if (lpf >= 188)data = 1;
	else if (lpf >= 98)data = 2;
	else if (lpf >= 42)data = 3;
	else if (lpf >= 20)data = 4;
	else if (lpf >= 10)data = 5;
	else data = 6;
	return MPU_Write_Byte(MPU_CFG_REG, data);//设置数字低通滤波器  
}

/**
  **********************************
  * @brief  设置MPU6050采样频率
	* @param  rate:4~1000(Hz)
  * @retval 0：成功
  **********************************
*/
unsigned char MPU_Set_Rate(uint16_t rate)
{
	unsigned char data;
	if (rate>1000)rate = 1000;
	if (rate<4)rate = 4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data);	//设置数字低通滤波器
	return MPU_Set_LPF(rate / 2);	//自动设置LPF为采样率的一半
}

/**
  **********************************
  * @brief  MPU6050采集温度
	* @param  None
  * @retval 扩大100倍
  **********************************
*/
short MPU_Get_Temperature(void)
{
	unsigned char buf[2];
	short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
	raw = ((u16)buf[0] << 8) | buf[1];
	temp = 36.53 + ((double)raw) / 340;
	return temp * 100;;
}

/**
  **********************************
  * @brief  采集MPU6050陀螺仪原始数据
	* @param  gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
  * @retval 0：成功
  **********************************
*/
unsigned char MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
	unsigned char buf[6], res;
	res = MPU_Read_Len(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
	if (res == 0)
	{
		*gx = ((u16)buf[0] << 8) | buf[1];
		*gy = ((u16)buf[2] << 8) | buf[3];
		*gz = ((u16)buf[4] << 8) | buf[5];
	}
	return res;
}

/**
  **********************************
  * @brief  采集MPU6050加速度计原始数据
	* @param  gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
  * @retval 0：成功
  **********************************
*/
unsigned char MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
	unsigned char buf[6], res;
	res = MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
	if (res == 0)
	{
		*ax = ((u16)buf[0] << 8) | buf[1];
		*ay = ((u16)buf[2] << 8) | buf[3];
		*az = ((u16)buf[4] << 8) | buf[5];
	}
	return res;
}

/**
  **********************************
  * @brief  IIC连续写数据
	* @param  addr:器件地址	reg:寄存器地址 len:写入长度 buf:数据区
  * @retval 0：成功
  **********************************
*/
unsigned char MPU_Write_Len(unsigned char addr, unsigned char reg, unsigned char len, unsigned char *buf)
{
	unsigned char i;
	MPU_IIC_Start();
	MPU_IIC_Send_Byte((addr << 1) | 0);//发送器件地址+写命令	
	if (MPU_IIC_Wait_Ack())	//等待应答
	{
		MPU_IIC_Stop();
		return 1;
	}
	MPU_IIC_Send_Byte(reg);	//写寄存器地址
	MPU_IIC_Wait_Ack();		//等待应答
	for (i = 0;i<len;i++)
	{
		MPU_IIC_Send_Byte(buf[i]);	//发送数据
		if (MPU_IIC_Wait_Ack())		//等待ACK 这里进去了
		{
			MPU_IIC_Stop();
			return 1;
		}
	}
	MPU_IIC_Stop();
	return 0;
}

/**
  **********************************
  * @brief  IIC连续读数据
	* @param  addr:器件地址	reg:寄存器地址 len:写入长度 buf:数据区
  * @retval 0：成功
  **********************************
*/
unsigned char MPU_Read_Len(unsigned char addr, unsigned char reg, unsigned char len, unsigned char *buf)
{
	MPU_IIC_Start();
	MPU_IIC_Send_Byte((addr << 1) | 0);//发送器件地址+写命令	
	if (MPU_IIC_Wait_Ack())	//等待应答
	{
		MPU_IIC_Stop();
		return 1;
	}
	MPU_IIC_Send_Byte(reg);	//写寄存器地址
	MPU_IIC_Wait_Ack();		//等待应答
	MPU_IIC_Start();
	MPU_IIC_Send_Byte((addr << 1) | 1);//发送器件地址+读命令	
	MPU_IIC_Wait_Ack();		//等待应答 
	while (len)
	{
		if (len == 1)*buf = MPU_IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf = MPU_IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++;
	}
	MPU_IIC_Stop();	//产生一个停止条件 
	return 0;
}

/**
  **********************************
  * @brief  MPU6050写一个字节
	* @param  reg:寄存器地址 data:数据
  * @retval None
  **********************************
*/
unsigned char MPU_Write_Byte(unsigned char reg, unsigned char data)
{
	MPU_IIC_Start();
	MPU_IIC_Send_Byte((MPU_ADDR << 1) | 0);//发送器件地址+写命令 0x68<<1=0xd0
	if (MPU_IIC_Wait_Ack())	//等待应答
	{
		MPU_IIC_Stop();
		return 1;
	}
	MPU_IIC_Send_Byte(reg);	//写寄存器地址
	MPU_IIC_Wait_Ack();		//等待应答 
	MPU_IIC_Send_Byte(data);//发送数据
	if (MPU_IIC_Wait_Ack())	//等待ACK
	{
		MPU_IIC_Stop();
		return 1;
	}
	MPU_IIC_Stop();
	return 0;
}

/**
  **********************************
  * @brief  MPU6050读一个字节
	* @param  reg:寄存器地址
  * @retval None
  **********************************
*/
unsigned char MPU_Read_Byte(unsigned char reg)
{
	unsigned char res;
	MPU_IIC_Start();
	MPU_IIC_Send_Byte((MPU_ADDR << 1) | 0);//发送器件地址+写命令	
	MPU_IIC_Wait_Ack();		//等待应答 
	MPU_IIC_Send_Byte(reg);	//写寄存器地址
	MPU_IIC_Wait_Ack();		//等待应答
	MPU_IIC_Start();
	MPU_IIC_Send_Byte((MPU_ADDR << 1) | 1);//发送器件地址+读命令	
	MPU_IIC_Wait_Ack();		//等待应答 
	res = MPU_IIC_Read_Byte(0);//读取数据,发送nACK 
	MPU_IIC_Stop();			//产生一个停止条件 
	return res;
}
