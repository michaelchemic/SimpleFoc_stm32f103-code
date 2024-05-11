#ifndef __LOWPASS_FILTER_H
#define __LOWPASS_FILTER_H

#include "sys.h"

extern float y_prev;

float LowPass_Filter(float x);
	
#endif
