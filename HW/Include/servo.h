#ifndef __SERVO_H
#define __SERVO_H
#include <stdint.h>

// ===================== 舵机PWM参数配置 =====================
// 你的TIM3配置：PSC=72-1，ARR=20000-1
// 1个计数 = 1us，周期20ms，标准舵机信号
#define SERVO_MIN_PULSE      500     // 0°对应的脉宽(us)
#define SERVO_MAX_PULSE      2500    // 180°对应的脉宽(us)
#define MAX_SPLINE_POINTS    6       // 三次样条最多支持6个轨迹点

// ===================== 舵机控制结构体 =====================
typedef struct{
    float current_angle;             // 当前舵机角度

    uint8_t  is_moving;              // 运动状态：0=停止，10~18对应不同算法
    uint32_t now_step;               // 当前走到第几步
    uint32_t total_steps;            // 总步数

    float start_angle;               // 运动起始角度
    float end_angle;                 // 运动目标角度

    // 梯形速度规划
    float max_speed;                // 最大速度 
    float max_acceleration;         // 最大加速度
}Servo_TypeDef;

extern Servo_TypeDef servo;


// ===================== 舵机控制函数 =====================
// 直接设置角度，不进行插值
void servo_move_raw(float target_angle);
// 线性插值：匀速变化
void servo_move_linear(float target_angle, uint32_t duration_ms);
// 三次缓动：速度连续，缓起缓停
void servo_move_cubic(float target_angle, uint32_t duration_ms);
// 五次缓动：加速度也连续，超柔顺，大负载推荐
void servo_move_quintic(float target_angle, uint32_t duration_ms);
// 正弦缓动：接近自然运动
void servo_move_sine(float target_angle, uint32_t duration_ms);
// 梯形速度规划：自动计算加速度和最大速度，适合大幅度运动，工业常用，加-匀-减三段
void servo_move_trapezoidal(float target_angle, uint32_t duration_ms);

/*===========================================================================
函数	速度曲线	          启停冲击	    时间利用率	         适用场景
linear	矩形（突变）	        大	       最高（无加减速）	  低速、不关心冲击
cubic	S形（速度连续）	        小	           中	           日常平滑运动
quintic	加速度连续	           极小	          较低	         对振动敏感的设备
trapezoidal	梯形（速度连续）	小	           高	          大范围、时间优先
===========================================================================*/

// 更新舵机位置，应该在定时器中断服务函数中调用
void servo_update(void);

#endif
