#include "LOWPASS_FILTER.h"

float y_prev;

/***********************************************
��ͨ�˲���:
***********************************************/
float LowPass_Filter(float x)
{
	float y = 0.9*y_prev + 0.1*x;
	
	y_prev=y;
	
	return y;
}
