#ifndef __PID_H_
#define __PID_H_

#include "sys.h"

typedef struct//定义pid结构体
{
	float Set;//定义目标值
	float Actual;//定义真实值
	float err;//定义偏差值
	float err_last;//定义上一个偏差值
	float Kp,Ki,Kd;//定义比例，积分，微分
	float voltage;//定义电压值
	float integral;//定义积分值
}pid;

extern pid pid_angle;//定义速度环pid
extern pid pid_speed;//定义距离环pid

void PID_init(void);//PID初始化

#endif
