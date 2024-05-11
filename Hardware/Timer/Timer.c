#include "Timer.h"

void TIM3_Init(void)
{
	//���ö�ʱ������(TIM3)
	TIM_InternalClockConfig(TIM3);//TIM3ʹ���ڲ�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//APB1�����е�TIM3��Ϊʹ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//����ʱ�ӷ�ƵΪ1��Ƶ->Ӱ���˲���
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//����Ϊ���ϼ���	
	TIM_TimeBaseInitStructure.TIM_Period=7200-1;//�Զ���װARRֵ 0<ARR<65535		/*ÿ1ms����һ��ADC����ת��*/
	TIM_TimeBaseInitStructure.TIM_Prescaler=50-1;//Ԥ��ƵPSCֵ 0<PSC<65535
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);//��ʼ��ʱ����Ԫ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//�߼���ʱ��ӵ�е��ظ����������������0
	
	//���ö�ʱ���жϲ���(TIM3)
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);//����ճ�ʼ����ͽ����ж�
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ΪPriorityGroup_2����ռ���ȼ���2��  ��ռ���ȼ���2��
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�2��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3����						 
}
