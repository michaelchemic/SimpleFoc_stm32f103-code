#include "AS5600.h"

/************************************************
��ȡAS5600�Ƕ�(ģ��I2C)
Ӳ������:
PB6--SCL
PB7--SDA
************************************************/

float full_rotations;    				//��ǰ��תȦ��
float angle_prev;		 				//��ǰ�Ƕ�(����λ�û�)
float vel_angle_prev;					//�ϴνǶ�(����λ�û�)
float angle_prev_Velocity;				//��ǰ�Ƕ�(�����ٶȻ�)
float vel_angle_prev_Velocity;			//�ϴνǶ�(�����ٶȻ�)
unsigned long angle_prev_ts;			//��ǰ������ʱ��
unsigned long vel_angle_prev_ts;		//�ϴε�����ʱ��

/***********************************************
ģ��I2C���ų�ʼ��:
***********************************************/
void AS5600_IIC_Init(void)
{			
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��PB�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;   //�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //50M
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   //�����趨������ʼ��GPIOB 
}
/***********************************************
I2C�����ʱ:
***********************************************/
void delay_s(u32 i)
{
	while(i--);
}
/***********************************************
I2C��ʼ�ź�:
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
I2Cֹͣ�ź�:
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
I2C�ȴ�Ӧ��:
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
I2CӦ��λ:
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
I2C��Ӧ��λ:
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
I2C��������:
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
I2C��ȡ����:
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
��ȡAS5600������8λԭʼ����:
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
��ȡAS5600������12λԭʼ����:
��ע��
	1.ԭʼ����Ϊ12λ(0~4095)
	2.�õ�0~360�ȽǶ�������/4096*360
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
��ȡ�ű�����360��ֵ:(0��-360��)
***********************************************/
float get_Angle_360(void)
{
	return AS5600_ReadRawAngleTwo() * 0.08789;
}

/***********************************************
��ȡ�ű�������һ������ֵ:(0-6.28)
***********************************************/
float get_Angle_2PI(void)
{
	return AS5600_ReadRawAngleTwo() * 0.08789 / 57.32484;
}
/***********************************************
�ű����������ƽǶ��ۼƼ���:(0-��)
***********************************************/
float get_Angle(void)
{
	float val = get_Angle_2PI();
	float d_angle = val - angle_prev;
	//������ת��Ȧ��
	//ͨ���жϽǶȱ仯�Ƿ����80%��һȦ(0.8f*6.28318530718f)���ж��Ƿ��������
	//������������,��full_rotations����1(���d_angleС��0)�����1(���d_angle����0)
	if( fabs(d_angle) > (0.8f*6.28318530718f) )
		full_rotations += (d_angle > 0) ? -1 : 1;
	angle_prev = val;
	return (float)full_rotations * 6.28318530718f + angle_prev;
}
/***********************************************
�ű������ٶȼ���:(0-��)
***********************************************/
float getVelocity(void)
{
	float Ts, vel=0.0;

	//�������ʱ��
	angle_prev_ts = SysTick->VAL;
	if(angle_prev_ts<vel_angle_prev_ts)Ts = (float)(vel_angle_prev_ts - angle_prev_ts)/9*1e-6;
	else
		Ts = (float)(0xFFFFFF - angle_prev_ts + vel_angle_prev_ts)/9*1e-6;
	//�����޸�΢С���
	if(Ts == 0 || Ts > 0.5) Ts = 1e-3f;
	//��¼��ǰ�Ƕ�
	angle_prev_Velocity = get_Angle();
	//�����洢
	vel = (angle_prev_Velocity - vel_angle_prev_Velocity) / Ts;
	vel_angle_prev_Velocity = angle_prev_Velocity;
	vel_angle_prev_ts = angle_prev_ts;

	return vel;
}
/***********************************************
�ű������ٶȵ�ͨ�˲�����:(0-��)
***********************************************/
float get_Speed(void)
{
	//�ٶ�ԭʼ���ݲɼ�
	float vel_M0_ori=getVelocity();
	//ԭʼ���ݵ�ͨ�˲�
	float vel_M0_flit=LowPass_Filter(DIR*vel_M0_ori);
	return vel_M0_flit;
}
