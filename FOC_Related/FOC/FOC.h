#ifndef __FOC_H
#define __FOC_H

#include "sys.h"

#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
//���amtС��low����low�����amt����high�򷵻�high�����򷵻�amt����������low��high��Χ�ڡ�
extern int PP,DIR;

void Systick_CountMode(void);
float _electricalAngle(void);
float _normalizeAngle(float angle);
void setPWM(float Ua,float Ub,float Uc);
void setTorque(float Uq,float angle_el);
void FOC_Init(float power_supply);
void FOC_AS5600_Init(int _PP,int _DIR);
void Set_Angle(float Angle);
void Set_Speed(float Speed);
void Ratchet_Wheel(float Angle);
void Boundary_Moment_limitation(float angle_range);

#endif
