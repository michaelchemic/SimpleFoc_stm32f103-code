#ifndef __SYS_H
#define __SYS_H

#include "stm32f10x.h"
#include "delay.h"
#include "control.h"
#include "PWM.h"
#include "pid.h"
#include "Timer.h"
#include "usart.h"
#include "FOC.h"
#include "AS5600.h"
#include "LowPass_Filter.h"

////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// 具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
// IO口操作宏定义
#define BITBAND(addr, bitnum)  ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)         *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
// IO口地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) // 0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) // 0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) // 0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) // 0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) // 0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) // 0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) // 0x40011E0C

#define GPIOA_IDR_Addr (GPIOA_BASE + 8) // 0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8) // 0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8) // 0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8) // 0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8) // 0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8) // 0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8) // 0x40011E08

// IO口操作,只对单一的IO口!
// 确保n的值小于16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // 输出
#define PAin(n)  BIT_ADDR(GPIOA_IDR_Addr, n) // 输入

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // 输出
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n) // 输入

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // 输出
#define PCin(n)  BIT_ADDR(GPIOC_IDR_Addr, n) // 输入

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // 输出
#define PDin(n)  BIT_ADDR(GPIOD_IDR_Addr, n) // 输入

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) // 输出
#define PEin(n)  BIT_ADDR(GPIOE_IDR_Addr, n) // 输入

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) // 输出
#define PFin(n)  BIT_ADDR(GPIOF_IDR_Addr, n) // 输入

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) // 输出
#define PGin(n)  BIT_ADDR(GPIOG_IDR_Addr, n) // 输入

/////////////////////////////////////////////////////////////////
// Ex_NVIC_Config专用定义
#define GPIO_A 0
#define GPIO_B 1
#define GPIO_C 2
#define GPIO_D 3
#define GPIO_E 4
#define GPIO_F 5
#define GPIO_G 6

#define FTIR   1 // 下降沿触发
#define RTIR   2 // 上升沿触发

// JTAG模式设置定义
#define JTAG_SWD_DISABLE 0X02
#define SWD_ENABLE       0X01
#define JTAG_SWD_ENABLE  0X00

// 常量宏定义
#define _2_SQRT3 1.15470053838
#define _SQRT3   1.73205080757
#define _1_SQRT3 0.57735026919
#define _SQRT3_2 0.86602540378
#define _SQRT2   1.41421356237
#define _120_D2R 2.09439510239
#define PI       3.14159265359
#define _PI      3.14159265359
#define _PI_2    1.57079632679
#define _PI_3    1.0471975512
#define _2PI     6.28318530718
#define _3PI_2   4.71238898038
#define _PI_6    0.52359877559

extern float voltage_limit;
extern float voltage_power_supply;
extern float shaft_angle, open_loop_timestamp;
extern float zero_electric_angle, Ualpha, Ubeta, Ua, Ub, Uc, dc_a, dc_b, dc_c;
extern float Sensor_Angle;
// extern float Angle_target;
extern float Sensor_Speed;
// extern float Speed_target;
extern uint8_t Total_Control_Flag;

void NVIC_Config(void);
void Motor_Function(void);
#endif
