#include "control.h"

float Angle_Out;//角度PWM输出
float Speed_Out;//速度PWM输出

/**********************
定时器3中断函数(200HZ)
**********************/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)//如果更新中断标志位==SET
	{
		Total_Control_Flag = 1;
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除标志位
	}
}

/**********************
角度环：pid控制

输入：角度误差
输出：角度环输出(控制力矩)
**********************/
float Angle_Control(float Angle_Err)
{
	int PWM_Out;

	pid_angle.err=Angle_Err;
	
	pid_angle.integral+=pid_angle.err;
	
	PWM_Out=pid_angle.Kp * pid_angle.err + pid_angle.Ki * pid_angle.integral + pid_angle.Kd * (pid_angle.err-pid_angle.err_last);

	pid_angle.integral=pid_angle.integral>2000?2000:(pid_angle.integral<(-2000)?(-2000):pid_angle.integral);//积分限幅
	
	pid_angle.err_last=pid_angle.err;
	
	return PWM_Out;
}
/**********************
速度环：pid控制

输入：速度误差
输出：速度环输出(控制力矩)
**********************/
float Speed_Control(float Speed_Err)
{
	int PWM_Out;

	pid_speed.err=Speed_Err;
	
	pid_speed.integral+=pid_speed.err;
	
	PWM_Out=pid_speed.Kp * pid_speed.err + pid_speed.Ki * pid_speed.integral + pid_speed.Kd * (pid_speed.err-pid_speed.err_last);

	pid_speed.integral=pid_speed.integral>3000?3000:(pid_speed.integral<(-3000)?(-3000):pid_speed.integral);//积分限幅
	
	pid_speed.err_last=pid_speed.err;
	
	return PWM_Out;
}
/**********************
限幅函数:(限制力矩)
**********************/
void Moment_limiting(void)
{
	if(Angle_Out>6)Angle_Out=6;//角度环力矩输出限幅
	if(Angle_Out<-6)Angle_Out=-6;
	if(Speed_Out>6)Speed_Out=6;//速度环力矩输出限幅
	if(Speed_Out<-6)Speed_Out=-6;
}
