#include "FOC.h"

int PP  = 7; // pole_pairs极对数 磁极数/2
int DIR = 1; // 无刷电机纠偏旋转方向

/*******************************************
SysTick初始化(模拟micros函数初始化)：

备注：
    1.从0xFFFFFF到0循环计数
    2.开启后无法再使用delay延时函数
*******************************************/
void Systick_CountMode(void)
{
    SysTick->LOAD = 0xFFFFFF - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
}
/*******************************************
电角度求解：

备注：
    1.     电角度      =   机械角度  *  极对数
    2._electricalAngle = shaft_angle * pole_pairs
*******************************************/
float _electricalAngle(void)
{
    return _normalizeAngle((float)(DIR * PP) * get_Angle_2PI() - zero_electric_angle);
}
/*******************************************
角度归一化处理:
*******************************************/
float _normalizeAngle(float angle)
{
    float a = fmod(angle, 2 * PI);
    return a >= 0 ? a : (a + 2 * PI);
    // 三目运算符。格式：condition ? expr1 : expr2
    // 其中,condition式要求值的条件表达式,如果条件成立,则返回 expr1 的值。可以将三目运算符看作if-else的简化形式
}
/*******************************************
PWM输出计算:
*******************************************/
void setPWM(float Ua, float Ub, float Uc)
{
    // 输出电压限幅
    Ua = _constrain(Ua, 0.0f, voltage_limit);
    Ub = _constrain(Ub, 0.0f, voltage_limit);
    Uc = _constrain(Uc, 0.0f, voltage_limit);
    // 占空比计算+输出限幅
    dc_a = _constrain(Ua / voltage_power_supply, 0.0f, 1.0f);
    dc_b = _constrain(Ub / voltage_power_supply, 0.0f, 1.0f);
    dc_c = _constrain(Uc / voltage_power_supply, 0.0f, 1.0f);
    // PWM注入
    PWM_SetCompare1(dc_a * 2400);
    PWM_SetCompare2(dc_b * 2400);
    PWM_SetCompare3(dc_c * 2400);
}
/*******************************************
力矩控制:
*******************************************/
void setTorque(float Uq, float angle_el)
{
    // 力矩限幅
    Uq = _constrain(Uq, -voltage_power_supply / 2, voltage_power_supply / 2);
    /*float Ud=0;*/
    angle_el = _normalizeAngle(angle_el);
    // 帕克逆变换
    Ualpha = -Uq * sin(angle_el);
    Ubeta  = Uq * cos(angle_el);
    // 克拉克逆变换
    Ua = Ualpha + voltage_power_supply / 2;
    Ub = (sqrt(3) * Ubeta - Ualpha) / 2 + voltage_power_supply / 2;
    Uc = (-Ualpha - sqrt(3) * Ubeta) / 2 + voltage_power_supply / 2;
    // PWM赋值
    setPWM(Ua, Ub, Uc);
}
/*******************************************
FOC控制初始化:
*******************************************/
void FOC_Init(float power_supply)
{
    voltage_power_supply = power_supply; // 电源电压
    PWMABC_Init();                       // 3相PMW波初始化(30000HZ)
    NVIC_Config();                       // 中断优先级分配
    PID_init();                          // 初始化pid变量
}
/*******************************************
FOC编码器数据初始化:
*******************************************/
void FOC_AS5600_Init(int _PP, int _DIR)
{
    AS5600_IIC_Init(); // AS5600模拟I2C初始化

    PP  = _PP;
    DIR = _DIR;
    setTorque(3, _3PI_2);

    delay_init(); // 延时函数初始化
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000); // 等待外设准备完毕

    zero_electric_angle = _electricalAngle(); // 设置零点角度
    setTorque(0, _3PI_2);

    Sensor_Speed = get_Speed(); // 速度初始值计算
}
/***********************************************
电机角度控制:(rad)
***********************************************/
void Set_Angle(float Angle)
{
    // 角度读取
    Sensor_Angle = get_Angle();
    // 角度控制
    Angle_Out = Angle_Control((Angle - DIR * Sensor_Angle) * 180 / PI);
    // 控制力矩限幅
    Moment_limiting();
    setTorque(Angle_Out, _electricalAngle());
    // 角度打印
    // printf("%.2f,%.2f\n",Sensor_Angle,Angle_target);
}
/***********************************************
电机速度控制:(rad/s)
***********************************************/
void Set_Speed(float Speed)
{
    // 速度读取
    Sensor_Speed = get_Speed();
    // 速度控制
    Speed_Out = Speed_Control(Speed - Sensor_Speed);
    // 控制力矩限幅
    Moment_limiting();
    // 驱动器力矩控制
    setTorque(Speed_Out, _electricalAngle());
    // 速度打印
    // printf("%.2f,%.2f\n",Sensor_Speed,Speed_target);
}

/***********************************************
棘轮(Ratchet_Wheel)模式：调整比例P值设置不同刚度(rigidity)
DFOC_M0_setTorque(Kp*(target-DFOC_M0_Angle()));

力矩设置：DFOC_M0_setTorque
换算成力矩的系数：Kp
要锁定位置的目标角度：target
获取编码器读到的电机当前角度：DFOC_M0_Angle
***********************************************/

/*arduino dengfoc 三句话写出棘轮效果代码
float attractor_distance = 45*PI/180.0;
float target = round(DFOC_M0_Angle() / attractor_distance) * attractor_distance;
DFOC_M0_setTorque(Kp * (target - DFOC_M0_Angle()));

*/
void Ratchet_Wheel(float Angle)
{
    // 8个顿挫点,角度值变成弧度值
    float attractor_distance = Angle * PI / 180.0;
    // 设置P值,电机抖动增加
    float P = 5;
    //
    float target = round(get_Angle() / attractor_distance) * attractor_distance;
    // 刚度控制
    setTorque(P * (target - get_Angle()), _electricalAngle());
}

/***********************************************
边界限制力矩(Boundary_Moment_limitation)模式：
1，读取当前电机角度
2，设定允许旋钮的转动范围
3，将角度制转换为弧度制

***********************************************/

void Boundary_Moment_limitation(float angle_range)
{
    float P = 5;
    // 设定允许旋钮的转动范围 90°
    // float angle_range = 90.0;
    // 设定允许旋钮的转动范围中间值 45°
    float angle_range2 = angle_range / 2;
    // 中间值转弧度制
    float limit_range = angle_range2 / 360.0 * _2PI;

    float L1 = zero_electric_angle + limit_range;
    float L2 = zero_electric_angle - limit_range;

    if (get_Angle() > L2 && get_Angle() < L1) {
        setTorque(0, _electricalAngle()); // 无动作
        // 6个顿挫点,角度值变成弧度值
        //        float attractor_distance = 15 * PI / 180.0;
        //        //设置P值
        //        float P = 5;
        //        //
        //        float target = round(get_Angle()/attractor_distance)*attractor_distance;
        //        //刚度控制
        //        setTorque(P*(target - get_Angle()),_electricalAngle());
    } else {

        if (get_Angle() < L2)
            setTorque(P * (L2 - get_Angle()), _electricalAngle());
        if (get_Angle() > L1)
            setTorque(P * (L1 - get_Angle()), _electricalAngle());
    }
}

/***********************************************
阻尼(damping)模式：
***********************************************/

/***********************************************
顺滑(smooth)模式：
***********************************************/

/***********************************************
失重(weightlessness)模式：
***********************************************/
