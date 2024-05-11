#ifndef  _CONTROL_H
#define  _CONTROL_H

#include "sys.h" 

extern float Angle_Out;
extern float Speed_Out;

void TIM3_IRQHandler(void);
float Angle_Control(float Angle_Err);
float Speed_Control(float Speed_Err);
void Moment_limiting(void);

#endif
