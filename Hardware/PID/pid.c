#include "pid.h"

pid pid_angle;//定义速度环pid
pid pid_speed;//定义距离环pid

/**********************
PID配置函数:
**********************/
void PID_init()//初始化pid变量
{
	//位置环pid
	pid_angle.Set=0.0;
	pid_angle.Actual=0.0;
	pid_angle.err=0.0;
	pid_angle.err_last=0.0;
	pid_angle.voltage=0.0;
	pid_angle.integral=0.0;
	pid_angle.Kp=0.043;//待调节0.043
	pid_angle.Ki=0.0005;//待调节0.0005
	pid_angle.Kd=0.22;//待调节0.22
	//速度环pid
	pid_speed.Set=0.0;
	pid_speed.Actual=0.0;
	pid_speed.err=0.0;
	pid_speed.err_last=0.0;
	pid_speed.voltage=0.0;
	pid_speed.integral=0.0;	
	pid_speed.Kp=0.11;//待调节0.11
	pid_speed.Ki=0.005;//待调节0.005
	pid_speed.Kd=0.0008;//待调节0.0008
}
