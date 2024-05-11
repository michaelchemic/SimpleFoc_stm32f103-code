#ifndef __PID_H_
#define __PID_H_

#include "sys.h"

typedef struct//����pid�ṹ��
{
	float Set;//����Ŀ��ֵ
	float Actual;//������ʵֵ
	float err;//����ƫ��ֵ
	float err_last;//������һ��ƫ��ֵ
	float Kp,Ki,Kd;//������������֣�΢��
	float voltage;//�����ѹֵ
	float integral;//�������ֵ
}pid;

extern pid pid_angle;//�����ٶȻ�pid
extern pid pid_speed;//������뻷pid

void PID_init(void);//PID��ʼ��

#endif
