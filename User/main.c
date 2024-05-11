#include "sys.h"

/*******************
基础变量定义
*******************/
#define RX_BUFFER_SIZE 5

// 定义串口接收缓冲区和指针
extern uint8_t rx_buffer[RX_BUFFER_SIZE];
extern uint8_t rx_index;

extern uint8_t function_code;
extern float data_value;

// extern uint8_t USAR2_RxData;

float voltage_limit        = 12; // 用户设置：电压限幅12V，设置不超过供电电压
float voltage_power_supply = 12; // 用户设置：供电电压12V
float shaft_angle = 0, open_loop_timestamp = 0;
float zero_electric_angle = 0, Ualpha, Ubeta = 0, Ua = 0, Ub = 0, Uc = 0, dc_a = 0, dc_b = 0, dc_c = 0;

float Sensor_Angle; // 无刷电机实际角度
// float Angle_target = 1.00;	//用户设置：无刷电机目标角度 默认设置1度

float Sensor_Speed; // 无刷电机实际速度
// float Speed_target = 10.0;	//用户设置：无刷电机目标速度

uint8_t Total_Control_Flag; // 控制频率(200HZ)

int main(void)
{
    uart2_init(115200); // 串口2初始化
    uart3_init(115200); // 串口3初始化

    USART_Send_String(USART2, "USART Send OK!");
    FOC_Init(12);          // 默认12V供电
    FOC_AS5600_Init(7, 1); // 7极对数 正补偿方向

    Systick_CountMode(); // Systick计时初始化(同时禁用延时函数)

    while (1) {
        switch (function_code) {
            case 0x01:
                // FOC角度控制
                Set_Angle(data_value); //(解除注释即可使用)
                break;
            case 0x02:
                // FOC速度控制，用户设置速度通过帕克逆变换和克拉克逆setPwm
                Set_Speed(data_value); //(解除注释即可使用)
                break;
            case 0x03:
                // FOC棘轮控制
                Ratchet_Wheel(data_value); //(解除注释即可使用)
                break;
            case 0x04:
                // FOC边界力矩控制
                Boundary_Moment_limitation(data_value);
                break;
        }
    }
}
void Motor_Function(void)
{

    function_code       = rx_buffer[1];
    uint8_t data_value1 = rx_buffer[2];
    short data_value0   = data_value1 * 100;
    uint8_t data_value2 = rx_buffer[3];
    data_value          = data_value0 + data_value2;
}

// debug 用
// u8 Value;
//		// if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
//         Value = USART_ReceiveData(USART2);
//		switch(Value){
//			case '1':
//		//FOC角度控制
//		Set_Angle(Angle_target);//(解除注释即可使用)
//			break;
//			case '2':
//		//FOC速度控制，用户设置速度通过帕克逆变换和克拉克逆setPwm
//		Set_Speed(Speed_target);//(解除注释即可使用)
//			break;
//            case '3':
//       //FOC棘轮控制
//         Ratchet_Wheel(); //(解除注释即可使用)
//         break;
//            case '4':
//         //FOC边界力矩控制
//         Boundary_Moment_limitation();
//         break;
//
//
//       // }

//}

//////////////////////////////////////////////////////////////////解包示例1////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////解包示例2////////////////////////////////////////////////////////////////
// void DGUS_ISRHandler(uint8_t Data)
//{
//  static uint8_t sDgus_RxNum = 0;                //数量
//  static uint8_t sDgus_RxBuf[DGUS_PACKAGE_LEN];
//  static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

//  sDgus_RxBuf[gDGUS_RxCnt] = Data;
//  gDGUS_RxCnt++;

//  //判断帧头
//  if(sDgus_RxBuf[0] != DGUS_FRAME_HEAD1)       //接收到帧头1
//  {
//    gDGUS_RxCnt = 0;
//    return;
//  }
//  if((2 == gDGUS_RxCnt) && (sDgus_RxBuf[1] != DGUS_FRAME_HEAD2))
//  {
//    gDGUS_RxCnt = 0;
//    return;
//  }

//  //确定一帧数据长度
//  if(gDGUS_RxCnt == 3)
//  {
//    sDgus_RxNum = sDgus_RxBuf[2] + 3;
//  }

//  //接收完一帧数据
//  if((6 <= gDGUS_RxCnt) && (sDgus_RxNum <= gDGUS_RxCnt))
//  {
//    gDGUS_RxCnt = 0;

//    if(xDGUSRcvQueue != NULL)                    //解析成功, 加入队列
//    {
//      xQueueSendFromISR(xDGUSRcvQueue, &sDgus_RxBuf[0], &xHigherPriorityTaskWoken);
//      portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
//    }
//  }
//}

//////////////////////////////////////////////////////////////////超时检测示例////////////////////////////////////////////////////////////////

// static void DGUS_TimingAndUpdate(uint16_t Nms)
//{
//   sDGUSTiming_Nms_Num = Nms;
//   TIM_SetCounter(DGUS_TIM, 0);                   //设置计数值为0
//   TIM_Cmd(DGUS_TIM, ENABLE);                     //启动定时器
// }

// void DGUS_COM_IRQHandler(void)
//{
//   if((DGUS_COM->SR & USART_FLAG_RXNE) == USART_FLAG_RXNE)
//   {
//     DGUS_TimingAndUpdate(5);                     //更新定时(防止超时)
//     DGUS_ISRHandler((uint8_t)USART_ReceiveData(DGUS_COM));
//   }
// }
