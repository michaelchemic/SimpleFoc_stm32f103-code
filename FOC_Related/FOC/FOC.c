#include "FOC.h"

int PP  = 7; // pole_pairs������ �ż���/2
int DIR = 1; // ��ˢ�����ƫ��ת����

/*******************************************
SysTick��ʼ��(ģ��micros������ʼ��)��

��ע��
    1.��0xFFFFFF��0ѭ������
    2.�������޷���ʹ��delay��ʱ����
*******************************************/
void Systick_CountMode(void)
{
    SysTick->LOAD = 0xFFFFFF - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
}
/*******************************************
��Ƕ���⣺

��ע��
    1.     ��Ƕ�      =   ��е�Ƕ�  *  ������
    2._electricalAngle = shaft_angle * pole_pairs
*******************************************/
float _electricalAngle(void)
{
    return _normalizeAngle((float)(DIR * PP) * get_Angle_2PI() - zero_electric_angle);
}
/*******************************************
�Ƕȹ�һ������:
*******************************************/
float _normalizeAngle(float angle)
{
    float a = fmod(angle, 2 * PI);
    return a >= 0 ? a : (a + 2 * PI);
    // ��Ŀ���������ʽ��condition ? expr1 : expr2
    // ����,conditionʽҪ��ֵ���������ʽ,�����������,�򷵻� expr1 ��ֵ�����Խ���Ŀ���������if-else�ļ���ʽ
}
/*******************************************
PWM�������:
*******************************************/
void setPWM(float Ua, float Ub, float Uc)
{
    // �����ѹ�޷�
    Ua = _constrain(Ua, 0.0f, voltage_limit);
    Ub = _constrain(Ub, 0.0f, voltage_limit);
    Uc = _constrain(Uc, 0.0f, voltage_limit);
    // ռ�ձȼ���+����޷�
    dc_a = _constrain(Ua / voltage_power_supply, 0.0f, 1.0f);
    dc_b = _constrain(Ub / voltage_power_supply, 0.0f, 1.0f);
    dc_c = _constrain(Uc / voltage_power_supply, 0.0f, 1.0f);
    // PWMע��
    PWM_SetCompare1(dc_a * 2400);
    PWM_SetCompare2(dc_b * 2400);
    PWM_SetCompare3(dc_c * 2400);
}
/*******************************************
���ؿ���:
*******************************************/
void setTorque(float Uq, float angle_el)
{
    // �����޷�
    Uq = _constrain(Uq, -voltage_power_supply / 2, voltage_power_supply / 2);
    /*float Ud=0;*/
    angle_el = _normalizeAngle(angle_el);
    // ������任
    Ualpha = -Uq * sin(angle_el);
    Ubeta  = Uq * cos(angle_el);
    // ��������任
    Ua = Ualpha + voltage_power_supply / 2;
    Ub = (sqrt(3) * Ubeta - Ualpha) / 2 + voltage_power_supply / 2;
    Uc = (-Ualpha - sqrt(3) * Ubeta) / 2 + voltage_power_supply / 2;
    // PWM��ֵ
    setPWM(Ua, Ub, Uc);
}
/*******************************************
FOC���Ƴ�ʼ��:
*******************************************/
void FOC_Init(float power_supply)
{
    voltage_power_supply = power_supply; // ��Դ��ѹ
    PWMABC_Init();                       // 3��PMW����ʼ��(30000HZ)
    NVIC_Config();                       // �ж����ȼ�����
    PID_init();                          // ��ʼ��pid����
}
/*******************************************
FOC���������ݳ�ʼ��:
*******************************************/
void FOC_AS5600_Init(int _PP, int _DIR)
{
    AS5600_IIC_Init(); // AS5600ģ��I2C��ʼ��

    PP  = _PP;
    DIR = _DIR;
    setTorque(3, _3PI_2);

    delay_init(); // ��ʱ������ʼ��
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000); // �ȴ�����׼�����

    zero_electric_angle = _electricalAngle(); // �������Ƕ�
    setTorque(0, _3PI_2);

    Sensor_Speed = get_Speed(); // �ٶȳ�ʼֵ����
}
/***********************************************
����Ƕȿ���:(rad)
***********************************************/
void Set_Angle(float Angle)
{
    // �Ƕȶ�ȡ
    Sensor_Angle = get_Angle();
    // �Ƕȿ���
    Angle_Out = Angle_Control((Angle - DIR * Sensor_Angle) * 180 / PI);
    // ���������޷�
    Moment_limiting();
    setTorque(Angle_Out, _electricalAngle());
    // �Ƕȴ�ӡ
    // printf("%.2f,%.2f\n",Sensor_Angle,Angle_target);
}
/***********************************************
����ٶȿ���:(rad/s)
***********************************************/
void Set_Speed(float Speed)
{
    // �ٶȶ�ȡ
    Sensor_Speed = get_Speed();
    // �ٶȿ���
    Speed_Out = Speed_Control(Speed - Sensor_Speed);
    // ���������޷�
    Moment_limiting();
    // ���������ؿ���
    setTorque(Speed_Out, _electricalAngle());
    // �ٶȴ�ӡ
    // printf("%.2f,%.2f\n",Sensor_Speed,Speed_target);
}

/***********************************************
����(Ratchet_Wheel)ģʽ����������Pֵ���ò�ͬ�ն�(rigidity)
DFOC_M0_setTorque(Kp*(target-DFOC_M0_Angle()));

�������ã�DFOC_M0_setTorque
��������ص�ϵ����Kp
Ҫ����λ�õ�Ŀ��Ƕȣ�target
��ȡ�����������ĵ����ǰ�Ƕȣ�DFOC_M0_Angle
***********************************************/

/*arduino dengfoc ���仰д������Ч������
float attractor_distance = 45*PI/180.0;
float target = round(DFOC_M0_Angle() / attractor_distance) * attractor_distance;
DFOC_M0_setTorque(Kp * (target - DFOC_M0_Angle()));

*/
void Ratchet_Wheel(float Angle)
{
    // 8���ٴ��,�Ƕ�ֵ��ɻ���ֵ
    float attractor_distance = Angle * PI / 180.0;
    // ����Pֵ,�����������
    float P = 5;
    //
    float target = round(get_Angle() / attractor_distance) * attractor_distance;
    // �նȿ���
    setTorque(P * (target - get_Angle()), _electricalAngle());
}

/***********************************************
�߽���������(Boundary_Moment_limitation)ģʽ��
1����ȡ��ǰ����Ƕ�
2���趨������ť��ת����Χ
3�����Ƕ���ת��Ϊ������

***********************************************/

void Boundary_Moment_limitation(float angle_range)
{
    float P = 5;
    // �趨������ť��ת����Χ 90��
    // float angle_range = 90.0;
    // �趨������ť��ת����Χ�м�ֵ 45��
    float angle_range2 = angle_range / 2;
    // �м�ֵת������
    float limit_range = angle_range2 / 360.0 * _2PI;

    float L1 = zero_electric_angle + limit_range;
    float L2 = zero_electric_angle - limit_range;

    if (get_Angle() > L2 && get_Angle() < L1) {
        setTorque(0, _electricalAngle()); // �޶���
        // 6���ٴ��,�Ƕ�ֵ��ɻ���ֵ
        //        float attractor_distance = 15 * PI / 180.0;
        //        //����Pֵ
        //        float P = 5;
        //        //
        //        float target = round(get_Angle()/attractor_distance)*attractor_distance;
        //        //�նȿ���
        //        setTorque(P*(target - get_Angle()),_electricalAngle());
    } else {

        if (get_Angle() < L2)
            setTorque(P * (L2 - get_Angle()), _electricalAngle());
        if (get_Angle() > L1)
            setTorque(P * (L1 - get_Angle()), _electricalAngle());
    }
}

/***********************************************
����(damping)ģʽ��
***********************************************/

/***********************************************
˳��(smooth)ģʽ��
***********************************************/

/***********************************************
ʧ��(weightlessness)ģʽ��
***********************************************/
