#include "sys.h" 

/*
  ============================================================================================================================
    NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  | Description
  ============================================================================================================================
   NVIC_PriorityGroup_0  |                0                  |            0-15             |   0 bits for pre-emption priority
                         |                                   |                             |   4 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------
   NVIC_PriorityGroup_1  |                0-1                |            0-7              |   1 bits for pre-emption priority
                         |                                   |                             |   3 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------    
   NVIC_PriorityGroup_2  |                0-3                |            0-3              |   2 bits for pre-emption priority
                         |                                   |                             |   2 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------    
   NVIC_PriorityGroup_3  |                0-7                |            0-1              |   3 bits for pre-emption priority
                         |                                   |                             |   1 bits for subpriority
  ----------------------------------------------------------------------------------------------------------------------------    
   NVIC_PriorityGroup_4  |                0-15               |            0                |   4 bits for pre-emption priority
                         |                                   |                             |   0 bits for subpriority                       
  ============================================================================================================================
*/


/**********************
中断优先级分配
**********************/
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//4级抢占,4级响应。
	
//	//定时器中断
//	NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
//	NVIC_Init(&NVIC_InitStruct);
    
     //串口2 接HMI BOARD
     NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;  
     NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;             
     NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; 
     NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;       
     NVIC_Init(&NVIC_InitStruct);   
	
			//串口3
			NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;
			NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
			NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
			NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
			NVIC_Init(&NVIC_InitStruct);	
    
                     
    
    
}
