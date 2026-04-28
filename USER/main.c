#include "gd32f10x.h"                   // Device header
#include "systick.h"                    // Systick header
#include "gd32f10x_exti.h"              // EXTI header
#include "usart.h"
#include "gd32f10x_timer.h"
#include "decoder.h"
#include "sensor.h"
#include "tim.h"
#include "servo.h"

void servo_move_test(void (*function) (float target_angle, uint32_t duration_ms));

int main(void)
{
	USART0_init(115200);


	systick_config();
	DMA0_Init();

	sensor_init();
	tim2_init();
	nvic_config();

	u0_printf("hello world\r\n");
	u0_printf("*** prescale number: %d ***\r\n", timer_prescaler_read(TIMER1));
	
	while(1)
	{	
		// servo_move_raw(90); // 设置传感器角度为90度
		// delay_1ms(1000); // 延时1秒
		// servo_move_raw(180); // 设置传感器角度为180度
		// delay_1ms(1000); // 延时1秒
		// servo_move_raw(90); // 设置传感器角度为90度
		// delay_1ms(1000); // 延时1秒
		// servo_move_raw(0); // 设置传感器角度为0度
		// delay_1ms(1000); // 延时1秒
		servo_move_test(servo_move_linear); // 测试线性插值舵机运动
		servo_move_test(servo_move_cubic);  // 测试三次缓动舵机运动
		servo_move_test(servo_move_quintic); // 测试五次缓动舵机运动
		servo_move_test(servo_move_sine);  // 测试正弦缓动舵机运动
		servo_move_test(servo_move_trapezoidal); // 测试梯形速度规划舵机运动
	}
}

void TIMER2_IRQHandler(void)
{
    if(timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP) != RESET){
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);   // 清除更新中断标志

        // user code
		servo_update(); // 更新舵机位置
    }
}

void servo_move_test(void (*function) (float target_angle, uint32_t duration_ms))
{
	// function(90.0, 2000); // 以指定算法将舵机移动到90度，持续时间2000ms
	// while(servo.is_moving); // 等待运动完成
	// delay_1ms(1000); // 运动完成后延时1秒
	function(180.0, 2000); // 以指定算法将舵机移动到180度，持续时间2000ms
	while(servo.is_moving)
	{
		u0_printf("Current Angle: %.2f, Step: %d/%d\r\n", servo.current_angle, servo.now_step, servo.total_steps);
		delay_1ms(10); // 每100ms打印一次当前角度和步
	} // 等待运动完成
	delay_1ms(1000); // 运动完成后延时1秒
	// function(90.0, 2000); // 以指定算法将舵机移动到90度，持续时间2000ms
	// while(servo.is_moving); // 等待运动完成
	// delay_1ms(1000); // 运动完成后延时1秒
	function(0.0, 2000); // 以指定算法将舵机移动到0度，持续时间2000ms
	while(servo.is_moving)
	{
		u0_printf("Current Angle: %.2f, Step: %d/%d\r\n", servo.current_angle, servo.now_step, servo.total_steps);
		delay_1ms(10); // 每100ms打印一次当前角度和步
	} // 等待运动完成
	delay_1ms(1000); // 运动完成后延时1秒
}
