#include "pid.h"

pid pid_angle;//�����ٶȻ�pid
pid pid_speed;//������뻷pid

/**********************
PID���ú���:
**********************/
void PID_init()//��ʼ��pid����
{
	//λ�û�pid
	pid_angle.Set=0.0;
	pid_angle.Actual=0.0;
	pid_angle.err=0.0;
	pid_angle.err_last=0.0;
	pid_angle.voltage=0.0;
	pid_angle.integral=0.0;
	pid_angle.Kp=0.043;//������0.043
	pid_angle.Ki=0.0005;//������0.0005
	pid_angle.Kd=0.22;//������0.22
	//�ٶȻ�pid
	pid_speed.Set=0.0;
	pid_speed.Actual=0.0;
	pid_speed.err=0.0;
	pid_speed.err_last=0.0;
	pid_speed.voltage=0.0;
	pid_speed.integral=0.0;	
	pid_speed.Kp=0.11;//������0.11
	pid_speed.Ki=0.005;//������0.005
	pid_speed.Kd=0.0008;//������0.0008
}
