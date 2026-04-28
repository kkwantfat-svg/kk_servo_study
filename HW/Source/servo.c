#include "Servo.h"
#include <math.h>
#include "sensor.h"

Servo_TypeDef servo;

/********************************** 基础算法库 ******************************/
// 线性插值：匀速变化
static float Linear(float t)
{
    return t;
}
// 三次缓动：速度连续，缓起缓停
static float Cubic(float t)
{
    return -2*t*t*t + 3*t*t;   // 满足f(0)=0, f(1)=1, f'(0)=0, f'(1)=0，速度连续，缓起缓停
}
// 五次缓动：加速度也连续，超柔顺
static float Quintic(float t)
{
    return 6*t*t*t*t*t - 15*t*t*t*t + 10*t*t*t;   // 满足f(0)=0, f(1)=1, f'(0)=0, f'(1)=0, f''(0)=0, f''(1)=0，比三次缓动更柔顺
}
// 正弦缓动：接近自然运动
static float Sine(float t)
{
    return -(cosf(3.1415926f * t) - 1) / 2.0f;   // 满足f(0)=0, f(1)=1，接近自然运动，速度在中间较快，两端较慢
}



/***********===================== 舵机控制函数 =====================***********/
/*
 * Function Name: servo_move_raw
 * Description: Move the servo to a target angle immediately
 */
void servo_move_raw(float target_angle)
{
    sensor_set_angle((uint8_t)target_angle);  // 直接设置角度，强制转换为uint8_t类型，确保在0-255范围内，虽然实际使用中只会用到0-180度
}

/*
 * Function Name: servo_move_linear
 * Description: Move the servo to a target angle over a specified duration
 */
void servo_move_linear(float target_angle, uint32_t duration_ms)
{
    // 计算步数和每步的角度增量
    servo.start_angle = servo.current_angle;  // 全局变量，初始为0
    servo.end_angle = target_angle;
    servo.total_steps = duration_ms / 10;  // 每10ms更新一次
    if(servo.total_steps == 0) servo.total_steps = 1; // 最少1步
    servo.now_step = 0;
    servo.is_moving = 10; // 假设10代表线性插值算法
}

/*
 * Function Name: servo_move_cubic
 * Description: Move the servo to a target angle over a specified duration using cubic interpolation
 */
void servo_move_cubic(float target_angle, uint32_t duration_ms)
{
    // 计算步数和每步的角度增量
    servo.start_angle = servo.current_angle;  // 全局变量，初始为0
    servo.end_angle = target_angle;
    servo.total_steps = duration_ms / 10;  // 每10ms更新一次
    if(servo.total_steps == 0) servo.total_steps = 1; // 最少1步
    servo.now_step = 0;
    servo.is_moving = 11; // 假设11代表三次缓动算法
}

/*
 * Function Name: servo_move_quintic
 * Description: Move the servo to a target angle over a specified duration using quintic interpolation
 */
void servo_move_quintic(float target_angle, uint32_t duration_ms)
{
    // 计算步数和每步的角度增量
    servo.start_angle = servo.current_angle;  // 全局变量，初始为0
    servo.end_angle = target_angle;
    servo.total_steps = duration_ms / 10;  // 每10ms更新一次
    if(servo.total_steps == 0) servo.total_steps = 1; // 最少1步
    servo.now_step = 0;
    servo.is_moving = 12; // 假设12代表五次缓动算法
}

/*
 * Function Name: servo_move_sine
 * Description: Move the servo to a target angle over a specified duration using sine interpolation
 */
void servo_move_sine(float target_angle, uint32_t duration_ms)
{
    // 计算步数和每步的角度增量
    servo.start_angle = servo.current_angle;  // 全局变量，初始为0
    servo.end_angle = target_angle;
    servo.total_steps = duration_ms / 10;  // 每10ms更新一次
    if(servo.total_steps == 0) servo.total_steps = 1; // 最少1步
    servo.now_step = 0;
    servo.is_moving = 13; // 假设13代表正弦缓动算法
}

/*
 * Function Name: servo_move_trapezoidal
 * Description: Move the servo to a target angle over a specified duration using trapezoidal interpolation
 */
void servo_move_trapezoidal(float target_angle, uint32_t duration_ms)
{
    // 计算步数和每步的角度增量
    servo.start_angle = servo.current_angle;  // 全局变量，初始为0
    servo.end_angle = target_angle;
    servo.total_steps = duration_ms / 10;  // 每10ms更新一次
    if(servo.total_steps == 0) servo.total_steps = 1; // 最少1步
    servo.now_step = 0;
    servo.is_moving = 14; // 假设14代表梯形速度算法

    // 自动计算最大速度与最大加速度
    // 规划加速度占总时长的20%，匀速占60%，减速占20% 由梯形面积S为（上底+下底）* 高 / 2，坐标轴为v-t图
    // 即 target_angle - start_angle = (0.6 * servo.total_steps + servo.total_steps) * max_speed / 2 * 10ms，解出max_speed后再计算max_acceleration
    // 支持双向运动，方向由角度差决定
    servo.max_speed = 2.0f * (servo.end_angle - servo.start_angle) / (0.6f * servo.total_steps + servo.total_steps); // 单位为度/step
    servo.max_acceleration = servo.max_speed / (0.2f * servo.total_steps); // 单位为度/step^2
}

/*
 * Function Name: servo_update
 * Description: Update the servo position based on its movement state
 */
void servo_update(void)
{
    if(servo.is_moving == 0) return; // 不在运动中
    static uint8_t flag = 0; // 用于控制运动结束时的角度修正只执行一次

    switch(servo.is_moving)
    { 
        case 10: // 线性插值算法
            if(servo.now_step < servo.total_steps){
                // 线性插值计算当前角度
                float t = (float)servo.now_step / servo.total_steps;
                servo.current_angle = servo.start_angle + Linear(t) * (servo.end_angle - servo.start_angle);      
                servo.now_step++;
            } else {
                // 运动结束，精确对准目标角度, else 执行第二次时再把 servo.is_moving 置为0，避免其置0后，while循环直接退出，导致 current_angle 无法精确对准 end_angle
                if(flag == 0){
                    servo.current_angle = servo.end_angle;
                    flag = 1;
                } else {
                    flag = 0;
                    servo.is_moving = 0;
                }
            }
            break;   
        case 11: // 三次缓动算法
            if(servo.now_step < servo.total_steps){
                // 三次缓动计算当前角度
                float t = (float)servo.now_step / servo.total_steps;
                servo.current_angle = servo.start_angle + Cubic(t) * (servo.end_angle - servo.start_angle);
                servo.now_step++;
            } else {
                // 运动结束，精确对准目标角度, else 执行第二次时再把 servo.is_moving 置为0，避免其置0后，while循环直接退出，导致 current_angle 无法精确对准 end_angle
                if(flag == 0){
                    servo.current_angle = servo.end_angle;
                    flag = 1;
                } else {
                    flag = 0;
                    servo.is_moving = 0;
                }
            }
            break;
        case 12: // 五次缓动算法
            if(servo.now_step < servo.total_steps){
                // 五次缓动计算当前角度
                float t = (float)servo.now_step / servo.total_steps;
                servo.current_angle = servo.start_angle + Quintic(t) * (servo.end_angle - servo.start_angle);
                servo.now_step++;
            } else {
                // 运动结束，精确对准目标角度, else 执行第二次时再把 servo.is_moving 置为0，避免其置0后，while循环直接退出，导致 current_angle 无法精确对准 end_angle
                if(flag == 0){
                    servo.current_angle = servo.end_angle;
                    flag = 1;
                } else {
                    flag = 0;
                    servo.is_moving = 0;
                }
            }
            break;
        case 13: // 正弦缓动算法
            if(servo.now_step < servo.total_steps){
                // 正弦缓动计算当前角度
                float t = (float)servo.now_step / servo.total_steps;
                servo.current_angle = servo.start_angle + Sine(t) * (servo.end_angle - servo.start_angle);
                servo.now_step++;
            } else {
                // 运动结束，精确对准目标角度, else 执行第二次时再把 servo.is_moving 置为0，避免其置0后，while循环直接退出，导致 current_angle 无法精确对准 end_angle
                if(flag == 0){
                    servo.current_angle = servo.end_angle;
                    flag = 1;
                } else {
                    flag = 0;
                    servo.is_moving = 0;
                }
            }
            break;
        case 14: // 梯形速度算法  （支持双向运动）
            if(servo.now_step < servo.total_steps * 0.2f){ // 加速阶段
                servo.current_angle = servo.start_angle + 0.5f * servo.max_acceleration * servo.now_step * servo.now_step; // s = 0.5*a*t^2
                servo.now_step++;
                if(servo.now_step >= servo.total_steps * 0.2f) servo.start_angle = servo.current_angle; // 加速阶段结束，更新起始角度，进入匀速阶段
            }
            else if(servo.now_step < servo.total_steps * 0.8f){ // 匀速阶段
                servo.current_angle = servo.start_angle + servo.max_speed * (servo.now_step - 0.2f * servo.total_steps); // s = s1 + v*t
                servo.now_step++;
                if(servo.now_step >= servo.total_steps * 0.8f) servo.start_angle = servo.current_angle; // 匀速阶段结束，更新起始角度，进入减速阶段
            }
            else if(servo.now_step < servo.total_steps){ // 减速阶段
                servo.current_angle = servo.start_angle + servo.max_speed * (servo.now_step - 0.8f * servo.total_steps) - 0.5f * servo.max_acceleration * (servo.now_step - 0.8f * servo.total_steps) * (servo.now_step - 0.8f * servo.total_steps); // s = s1 + v*t - 0.5*a*t^2
                servo.now_step++;
            } else {
                // 运动结束，精确对准目标角度, else 执行第二次时再把 servo.is_moving 置为0，避免其置0后，while循环直接退出，导致 current_angle 无法精确对准 end_angle
                if(flag == 0){
                    servo.current_angle = servo.end_angle;
                    flag = 1;
                } else {
                    flag = 0;
                    servo.is_moving = 0;
                }
            }
            break;
    }
    sensor_set_angle((uint8_t)servo.current_angle); // 更新舵机角度
}

/*  对于带编码器的电机来说 只需将 servo.current_angle 作为位置 PID 的目标值即可（进行一些适当转化）
// 定时器中断 10ms
void Timer_10ms_IRQ(void)
{
    // 1. 更新运动规划（生成新的期望位置）
    servo_update();
    
    // 2. 单位转换（度 -> 编码器脉冲）
    int32_t target_pulse = (int32_t)(servo.current_angle / 360.0f * PULSES_PER_REV);
    
    // 3. 读取实际编码器脉冲数
    int32_t actual_pulse = encoder_get_count();
    
    // 4. PID 计算（位置环）
    int32_t pid_out = pid_position_update(target_pulse, actual_pulse);
    
    // 5. 可选的期望速度前馈（需先换算速度）
    float target_speed_deg_per_sec = ...; // 从 servo.max_speed 或差分得到
    int32_t ff = speed_to_pwm(target_speed_deg_per_sec);
    
    // 6. 输出 PWM
    motor_set_pwm(pid_out + ff);
}
*/