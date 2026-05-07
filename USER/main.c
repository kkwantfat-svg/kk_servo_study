#include "gd32f10x.h"                   // Device header
#include "systick.h"                    // Systick header
#include "gd32f10x_exti.h"              // EXTI header
#include "usart.h"
#include "gd32f10x_timer.h"
#include "decoder.h"
#include "sensor.h"
#include "tim.h"
#include "servo.h"

// 测试函数声明
void servo_test_algorithm(uint8_t algorithm);
void show_menu(void);

int main(void)
{
	USART0_init(115200);
	systick_config();
	DMA0_Init();

	sensor_init();
	tim2_init();
	nvic_config();

	u0_printf("\r\n=== Servo Motion Control System ===\r\n");
	u0_printf("Initialize success!\r\n\r\n");
	
	while(1)
	{	
		show_menu();
		u0_printf("\nSelect algorithm (0-5): ");
		
		// 等待用户输入
		uint16_t recv_len = 0;
		uint8_t input_buffer[2] = {0};
		recv_len = receive_string(input_buffer, 2);  // 接收一个字符，留一个位置给 '\0'
		u0_printf("You entered: %s\r\n", input_buffer);
		
		if(recv_len > 0)
		{
			uint8_t choice = input_buffer[0] - '0';  // 将ASCII转为数字
			
			if(choice >= 0 && choice <= 5)
			{
				servo_test_algorithm(choice);
			}
			else
			{
				u0_printf("Invalid input! Please select 0-5.\r\n");
			}
		}
	}
}

// 显示菜单
void show_menu(void)
{
	u0_printf("\r\n========== TEST MENU ==========\r\n");
	u0_printf("1. Linear Motion      (匀速)\r\n");
	u0_printf("2. Cubic Easing       (三次缓动)\r\n");
	u0_printf("3. Quintic Easing     (五次缓动)\r\n");
	u0_printf("4. Sine Easing        (正弦缓动)\r\n");
	u0_printf("5. Trapezoidal Motion (梯形规划)\r\n");
	u0_printf("0. Raw Control        (直接控制)\r\n");
	u0_printf("==============================\r\n");
}

// 测试单个算法
void servo_test_algorithm(uint8_t algorithm)
{
	switch(algorithm)
	{
		case 0:  // 直接控制
			u0_printf("\n--- Raw Control Test ---\r\n");
			servo_move_raw(0);
			delay_1ms(1000);
			servo_move_raw(90);
			delay_1ms(1000);
			servo_move_raw(180);
			delay_1ms(1000);
			servo_move_raw(90);
			delay_1ms(1000);
			break;
			
		case 1:  // 线性运动
			u0_printf("\n--- Linear Motion Test (2000ms) ---\r\n");
			servo_move_linear(90.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_linear(180.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_linear(0.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			u0_printf("Test completed!\r\n");
			break;
			
		case 2:  // 三次缓动
			u0_printf("\n--- Cubic Easing Test (2000ms) ---\r\n");
			servo_move_cubic(90.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_cubic(180.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_cubic(0.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			u0_printf("Test completed!\r\n");
			break;
			
		case 3:  // 五次缓动
			u0_printf("\n--- Quintic Easing Test (2000ms) ---\r\n");
			servo_move_quintic(90.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_quintic(180.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_quintic(0.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			u0_printf("Test completed!\r\n");
			break;
			
		case 4:  // 正弦缓动
			u0_printf("\n--- Sine Easing Test (2000ms) ---\r\n");
			servo_move_sine(90.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_sine(180.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_sine(0.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			u0_printf("Test completed!\r\n");
			break;
			
		case 5:  // 梯形速度规划
			u0_printf("\n--- Trapezoidal Motion Test (2000ms) ---\r\n");
			servo_move_trapezoidal(90.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_trapezoidal(180.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			
			servo_move_trapezoidal(0.0, 2000);
			while(servo.is_moving) delay_1ms(10);
			delay_1ms(500);
			u0_printf("Test completed!\r\n");
			break;
			
		default:
			u0_printf("Unknown algorithm!\r\n");
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
