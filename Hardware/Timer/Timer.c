#include "Timer.h"

void TIM3_Init(void)
{
	//配置定时器部分(TIM3)
	TIM_InternalClockConfig(TIM3);//TIM3使用内部时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//APB1总线中的TIM3设为使能
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//设置时钟分频为1分频->影响滤波器
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//设置为向上计数	
	TIM_TimeBaseInitStructure.TIM_Period=7200-1;//自动重装ARR值 0<ARR<65535		/*每1ms触发一次ADC数据转化*/
	TIM_TimeBaseInitStructure.TIM_Prescaler=50-1;//预分频PSC值 0<PSC<65535
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);//初始化时基单元
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//高级定时器拥有的重复计数器，不用则给0
	
	//配置定时器中断部分(TIM3)
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);//避免刚初始化完就进入中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //使能指定的TIM3中断,允许更新中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置为PriorityGroup_2：先占优先级：2个  从占优先级：2个
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级2级
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3外设						 
}
