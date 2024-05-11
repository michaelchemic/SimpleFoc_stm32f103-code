#ifndef __AS5600_H_
#define __AS5600_H_

#include "sys.h"

//AS5600地址存储
#define  RAW_Angle_Hi    0x0C
#define  RAW_Angle_Lo    0x0D
#define  AS5600_Address  0x36
#define  AS5600_CPR      4096
//IO方向设置
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;} //SDA切换为输入模式 
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;} //SDA切换为输出模式 
//IO操作函数
#define IIC_SCL    PBout(6) //SCL输出
#define IIC_SDA    PBout(7) //SDA输出	 
#define READ_SDA   PBin(7)  //SDA输入

extern float full_rotations;
extern float angle_prev;
extern float vel_angle_prev;
extern float angle_prev_Velocity;
extern float vel_angle_prev_Velocity;
extern unsigned long angle_prev_ts;
extern unsigned long vel_angle_prev_ts;

void AS5600_IIC_Init(void);
void delay_s(u32 i);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(u8 ack);
u8 AS5600_ReadOneByte(u8 addr);
u16 AS5600_ReadRawAngleTwo(void);
float get_Angle_360(void);
float get_Angle_2PI(void);
float get_Angle(void);
float getVelocity(void);
float get_Speed(void);

#endif
