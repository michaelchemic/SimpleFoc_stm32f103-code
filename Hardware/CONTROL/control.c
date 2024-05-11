#include "control.h"

float Angle_Out;//�Ƕ�PWM���
float Speed_Out;//�ٶ�PWM���

/**********************
��ʱ��3�жϺ���(200HZ)
**********************/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)//��������жϱ�־λ==SET
	{
		Total_Control_Flag = 1;
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//�����־λ
	}
}

/**********************
�ǶȻ���pid����

���룺�Ƕ����
������ǶȻ����(��������)
**********************/
float Angle_Control(float Angle_Err)
{
	int PWM_Out;

	pid_angle.err=Angle_Err;
	
	pid_angle.integral+=pid_angle.err;
	
	PWM_Out=pid_angle.Kp * pid_angle.err + pid_angle.Ki * pid_angle.integral + pid_angle.Kd * (pid_angle.err-pid_angle.err_last);

	pid_angle.integral=pid_angle.integral>2000?2000:(pid_angle.integral<(-2000)?(-2000):pid_angle.integral);//�����޷�
	
	pid_angle.err_last=pid_angle.err;
	
	return PWM_Out;
}
/**********************
�ٶȻ���pid����

���룺�ٶ����
������ٶȻ����(��������)
**********************/
float Speed_Control(float Speed_Err)
{
	int PWM_Out;

	pid_speed.err=Speed_Err;
	
	pid_speed.integral+=pid_speed.err;
	
	PWM_Out=pid_speed.Kp * pid_speed.err + pid_speed.Ki * pid_speed.integral + pid_speed.Kd * (pid_speed.err-pid_speed.err_last);

	pid_speed.integral=pid_speed.integral>3000?3000:(pid_speed.integral<(-3000)?(-3000):pid_speed.integral);//�����޷�
	
	pid_speed.err_last=pid_speed.err;
	
	return PWM_Out;
}
/**********************
�޷�����:(��������)
**********************/
void Moment_limiting(void)
{
	if(Angle_Out>6)Angle_Out=6;//�ǶȻ���������޷�
	if(Angle_Out<-6)Angle_Out=-6;
	if(Speed_Out>6)Speed_Out=6;//�ٶȻ���������޷�
	if(Speed_Out<-6)Speed_Out=-6;
}
