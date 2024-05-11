#include "AS5600.h"

/************************************************
读取AS5600角度(模拟I2C)
硬件连接:
PB6--SCL
PB7--SDA
************************************************/

float full_rotations;    				//当前旋转圈数
float angle_prev;		 				//当前角度(用于位置环)
float vel_angle_prev;					//上次角度(用于位置环)
float angle_prev_Velocity;				//当前角度(用于速度环)
float vel_angle_prev_Velocity;			//上次角度(用于速度环)
unsigned long angle_prev_ts;			//当前的运行时间
unsigned long vel_angle_prev_ts;		//上次的运行时间

/***********************************************
模拟I2C引脚初始化:
***********************************************/
void AS5600_IIC_Init(void)
{			
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能PB端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;   //端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //50M
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   //根据设定参数初始化GPIOB 
}
/***********************************************
I2C软件延时:
***********************************************/
void delay_s(u32 i)
{
	while(i--);
}
/***********************************************
I2C起始信号:
***********************************************/
void IIC_Start(void)
{
	IIC_SDA=1;
	IIC_SCL=1;
	delay_s(20);
	IIC_SDA=0;
	delay_s(20);
	IIC_SCL=0;
}
/***********************************************
I2C停止信号:
***********************************************/
void IIC_Stop(void)
{
	IIC_SCL=0;
	IIC_SDA=0;
	delay_s(20);
	IIC_SCL=1;
	IIC_SDA=1;
	delay_s(20);
}
/***********************************************
I2C等待应答:
***********************************************/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	SDA_IN();
	IIC_SDA=1;
	IIC_SCL=1;
	delay_s(10);
	while(READ_SDA)
	{
		if(++ucErrTime>250)
		{
			SDA_OUT();
			IIC_Stop();
			return 1;
		}
	}
	SDA_OUT();
	IIC_SCL=0;
	return 0; 
}
/***********************************************
I2C应答位:
***********************************************/
void IIC_Ack(void)
{
	IIC_SCL=0;
	IIC_SDA=0;
	delay_s(20);
	IIC_SCL=1;
	delay_s(20);
	IIC_SCL=0;
}
/***********************************************
I2C否应答位:
***********************************************/
void IIC_NAck(void)
{
	IIC_SCL=0;
	IIC_SDA=1;
	delay_s(20);
	IIC_SCL=1;
	delay_s(20);
	IIC_SCL=0;
}
/***********************************************
I2C发送数据:
***********************************************/
void IIC_Send_Byte(u8 txd)
{
	u32 i;
	
	IIC_SCL=0;
	for(i=0;i<8;i++)
	{
		if((txd&0x80)!=0)IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1;
		delay_s(20);
		IIC_SCL=1;
		delay_s(20);
		IIC_SCL=0;
		delay_s(20);
	}
}
/***********************************************
I2C读取数据:
***********************************************/
u8 IIC_Read_Byte(u8 ack)
{
	u8 i,rcv=0;
	
	SDA_IN();
	for(i=0;i<8;i++)
	{
		IIC_SCL=0; 
		delay_s(20);
		IIC_SCL=1;
		rcv<<=1;
		if(READ_SDA)rcv++;
		delay_s(10);
	}
	SDA_OUT();
	if(!ack)IIC_NAck();
	else
		IIC_Ack();
	return rcv;
}
/***********************************************
读取AS5600编码器8位原始数据:
***********************************************/
u8 AS5600_ReadOneByte(u8 addr)
{
	u8 temp;		  	    																 
	
	IIC_Start();
	IIC_Send_Byte(AS5600_Address<<1);
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte((AS5600_Address<<1)+1);
	IIC_Wait_Ack();	 
	temp=IIC_Read_Byte(0);		   
	IIC_Stop();
	
	return temp;
}
/***********************************************
读取AS5600编码器12位原始数据:
备注：
	1.原始数据为12位(0~4095)
	2.得到0~360度角度数据需/4096*360
***********************************************/
u16 AS5600_ReadRawAngleTwo(void)
{
	u8 dh,dl;		  	    																 
	
	IIC_Start();
	IIC_Send_Byte(AS5600_Address<<1);
	IIC_Wait_Ack();
	IIC_Send_Byte(RAW_Angle_Hi);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte((AS5600_Address<<1)+1);
	IIC_Wait_Ack();
	dh=IIC_Read_Byte(1);
	dl=IIC_Read_Byte(0);
	IIC_Stop();
	
	return ((dh<<8)+dl);
}
/***********************************************
读取磁编码器360度值:(0°-360°)
***********************************************/
float get_Angle_360(void)
{
	return AS5600_ReadRawAngleTwo() * 0.08789;
}

/***********************************************
读取磁编码器归一化弧度值:(0-6.28)
***********************************************/
float get_Angle_2PI(void)
{
	return AS5600_ReadRawAngleTwo() * 0.08789 / 57.32484;
}
/***********************************************
磁编码器弧度制角度累计计算:(0-∞)
***********************************************/
float get_Angle(void)
{
	float val = get_Angle_2PI();
	float d_angle = val - angle_prev;
	//计算旋转的圈数
	//通过判断角度变化是否大于80%的一圈(0.8f*6.28318530718f)来判断是否发生了溢出
	//如果发生了溢出,则将full_rotations增加1(如果d_angle小于0)或减少1(如果d_angle大于0)
	if( fabs(d_angle) > (0.8f*6.28318530718f) )
		full_rotations += (d_angle > 0) ? -1 : 1;
	angle_prev = val;
	return (float)full_rotations * 6.28318530718f + angle_prev;
}
/***********************************************
磁编码器速度计算:(0-∞)
***********************************************/
float getVelocity(void)
{
	float Ts, vel=0.0;

	//计算采样时间
	angle_prev_ts = SysTick->VAL;
	if(angle_prev_ts<vel_angle_prev_ts)Ts = (float)(vel_angle_prev_ts - angle_prev_ts)/9*1e-6;
	else
		Ts = (float)(0xFFFFFF - angle_prev_ts + vel_angle_prev_ts)/9*1e-6;
	//快速修复微小溢出
	if(Ts == 0 || Ts > 0.5) Ts = 1e-3f;
	//记录当前角度
	angle_prev_Velocity = get_Angle();
	//变量存储
	vel = (angle_prev_Velocity - vel_angle_prev_Velocity) / Ts;
	vel_angle_prev_Velocity = angle_prev_Velocity;
	vel_angle_prev_ts = angle_prev_ts;

	return vel;
}
/***********************************************
磁编码器速度低通滤波计算:(0-∞)
***********************************************/
float get_Speed(void)
{
	//速度原始数据采集
	float vel_M0_ori=getVelocity();
	//原始数据低通滤波
	float vel_M0_flit=LowPass_Filter(DIR*vel_M0_ori);
	return vel_M0_flit;
}
