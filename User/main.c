#include "sys.h"

/*******************
������������
*******************/
#define RX_BUFFER_SIZE 5

// ���崮�ڽ��ջ�������ָ��
extern uint8_t rx_buffer[RX_BUFFER_SIZE];
extern uint8_t rx_index;

extern uint8_t function_code;
extern float data_value;

// extern uint8_t USAR2_RxData;

float voltage_limit        = 12; // �û����ã���ѹ�޷�12V�����ò����������ѹ
float voltage_power_supply = 12; // �û����ã������ѹ12V
float shaft_angle = 0, open_loop_timestamp = 0;
float zero_electric_angle = 0, Ualpha, Ubeta = 0, Ua = 0, Ub = 0, Uc = 0, dc_a = 0, dc_b = 0, dc_c = 0;

float Sensor_Angle; // ��ˢ���ʵ�ʽǶ�
// float Angle_target = 1.00;	//�û����ã���ˢ���Ŀ��Ƕ� Ĭ������1��

float Sensor_Speed; // ��ˢ���ʵ���ٶ�
// float Speed_target = 10.0;	//�û����ã���ˢ���Ŀ���ٶ�

uint8_t Total_Control_Flag; // ����Ƶ��(200HZ)

int main(void)
{
    uart2_init(115200); // ����2��ʼ��
    uart3_init(115200); // ����3��ʼ��

    USART_Send_String(USART2, "USART Send OK!");
    FOC_Init(12);          // Ĭ��12V����
    FOC_AS5600_Init(7, 1); // 7������ ����������

    Systick_CountMode(); // Systick��ʱ��ʼ��(ͬʱ������ʱ����)

    while (1) {
        switch (function_code) {
            case 0x01:
                // FOC�Ƕȿ���
                Set_Angle(data_value); //(���ע�ͼ���ʹ��)
                break;
            case 0x02:
                // FOC�ٶȿ��ƣ��û������ٶ�ͨ��������任�Ϳ�������setPwm
                Set_Speed(data_value); //(���ע�ͼ���ʹ��)
                break;
            case 0x03:
                // FOC���ֿ���
                Ratchet_Wheel(data_value); //(���ע�ͼ���ʹ��)
                break;
            case 0x04:
                // FOC�߽����ؿ���
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

// debug ��
// u8 Value;
//		// if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
//         Value = USART_ReceiveData(USART2);
//		switch(Value){
//			case '1':
//		//FOC�Ƕȿ���
//		Set_Angle(Angle_target);//(���ע�ͼ���ʹ��)
//			break;
//			case '2':
//		//FOC�ٶȿ��ƣ��û������ٶ�ͨ��������任�Ϳ�������setPwm
//		Set_Speed(Speed_target);//(���ע�ͼ���ʹ��)
//			break;
//            case '3':
//       //FOC���ֿ���
//         Ratchet_Wheel(); //(���ע�ͼ���ʹ��)
//         break;
//            case '4':
//         //FOC�߽����ؿ���
//         Boundary_Moment_limitation();
//         break;
//
//
//       // }

//}

//////////////////////////////////////////////////////////////////���ʾ��1////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////���ʾ��2////////////////////////////////////////////////////////////////
// void DGUS_ISRHandler(uint8_t Data)
//{
//  static uint8_t sDgus_RxNum = 0;                //����
//  static uint8_t sDgus_RxBuf[DGUS_PACKAGE_LEN];
//  static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

//  sDgus_RxBuf[gDGUS_RxCnt] = Data;
//  gDGUS_RxCnt++;

//  //�ж�֡ͷ
//  if(sDgus_RxBuf[0] != DGUS_FRAME_HEAD1)       //���յ�֡ͷ1
//  {
//    gDGUS_RxCnt = 0;
//    return;
//  }
//  if((2 == gDGUS_RxCnt) && (sDgus_RxBuf[1] != DGUS_FRAME_HEAD2))
//  {
//    gDGUS_RxCnt = 0;
//    return;
//  }

//  //ȷ��һ֡���ݳ���
//  if(gDGUS_RxCnt == 3)
//  {
//    sDgus_RxNum = sDgus_RxBuf[2] + 3;
//  }

//  //������һ֡����
//  if((6 <= gDGUS_RxCnt) && (sDgus_RxNum <= gDGUS_RxCnt))
//  {
//    gDGUS_RxCnt = 0;

//    if(xDGUSRcvQueue != NULL)                    //�����ɹ�, �������
//    {
//      xQueueSendFromISR(xDGUSRcvQueue, &sDgus_RxBuf[0], &xHigherPriorityTaskWoken);
//      portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
//    }
//  }
//}

//////////////////////////////////////////////////////////////////��ʱ���ʾ��////////////////////////////////////////////////////////////////

// static void DGUS_TimingAndUpdate(uint16_t Nms)
//{
//   sDGUSTiming_Nms_Num = Nms;
//   TIM_SetCounter(DGUS_TIM, 0);                   //���ü���ֵΪ0
//   TIM_Cmd(DGUS_TIM, ENABLE);                     //������ʱ��
// }

// void DGUS_COM_IRQHandler(void)
//{
//   if((DGUS_COM->SR & USART_FLAG_RXNE) == USART_FLAG_RXNE)
//   {
//     DGUS_TimingAndUpdate(5);                     //���¶�ʱ(��ֹ��ʱ)
//     DGUS_ISRHandler((uint8_t)USART_ReceiveData(DGUS_COM));
//   }
// }
