#include "PWM.h"

/**********************
PWM配置函数：
备注：TIM1为PWM波输出
**********************/
void PWMABC_Init(void)
{
	//PWM波引脚初始化(PA8,9,10)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使用RCC开启GPIO时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//50MHZ速度
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//配置定时器部分
	TIM_InternalClockConfig(TIM1);//TIM1使用内部时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//APB2总线中的TIM1设为使能
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//设置时钟分频为1分频->影响滤波器
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//设置为向上计数
	TIM_TimeBaseInitStructure.TIM_Period=2400-1;//自动重装【ARR】值 0<ARR<65535 【**】
	TIM_TimeBaseInitStructure.TIM_Prescaler=1-1;//预分频【PSC】值 0<PSC<65535【**】
	/*此处为频率8K*/
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);//初始化时基单元
	
	
	//配置PWM波输出部分
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//设置输出比较模式为PWM1模式
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//REF有效时，输出高电平(有效电平为高电平)
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//设置为使能
	TIM_OCInitStructure.TIM_Pulse=0;//设置【CCR】捕获比较寄存器值【**】
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);//意为在TIM1的OC1通道-PA8上输出PWM波【*】
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);//意为在TIM1的OC2通道-PA9上输出PWM波【*】
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);//意为在TIM1的OC3通道-PA10上输出PWM波【*】
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//高级定时器专属--MOE主输出使能
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);//OC1预装载寄存器使能
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);//OC2预装载寄存器使能
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);//OC3预装载寄存器使能
	TIM_ARRPreloadConfig(TIM1,ENABLE);//TIM1在ARR上预装载寄存器使能
	
	TIM_Cmd(TIM1,ENABLE);//启动定时器1	
}

void PWM_SetCompare1(uint16_t Compare)//写入TIM1通道1的CCR(占空比)
{
	TIM_SetCompare1(TIM1,Compare);
}
void PWM_SetCompare2(uint16_t Compare)//写入TIM1通道2的CCR(占空比)
{
	TIM_SetCompare2(TIM1,Compare);
}
void PWM_SetCompare3(uint16_t Compare)//写入TIM1通道3的CCR(占空比)
{
	TIM_SetCompare3(TIM1,Compare);
}
void PWM_SetCompare4(uint16_t Compare)//写入TIM1通道4的CCR(占空比)
{
	TIM_SetCompare4(TIM1,Compare);
}
