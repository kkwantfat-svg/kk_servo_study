#include "KEY.h"
#include "gd32f10x_gpio.h"    // GPIO header
#include "systick.h"

#define KEY_DEBOUNCE_TIME 20  // 消抖时间设置为20ms
key_SM_t key_sm = {KEY_UP, 0};  // 设置按键初始状态

void Key_Init(void){
    rcu_periph_clock_enable(RCU_GPIOA);   // 使能时钟

	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7); // 配置PA7为上拉输入
}


/*!
    \brief      按键扫描
    \param[in]  gpio_periph: GPIOx(x = A,B,C,D,E,F,G) 
    \param[in]  pin: GPIO pin
    \param[in]  on_press  按键按下回调函数，传入 0 表示不需要执行操作
    \param[in]  on_release 按键抬起回调函数，传入 0 表示不需要执行操作
*/
void Key_scan_sm(uint32_t gpio_periph, uint32_t pin, void (*on_press) (void), void (*on_release) (void)){
    uint8_t pin_value = gpio_input_bit_get(gpio_periph, pin);
    uint32_t current_time = get_current_time_ms();

    switch(key_sm.state){
        case KEY_UP:
            if(pin_value == 0){   // 按下（低电平有效）
                key_sm.state = KEY_DOWN_DELAY;  // 切换状态
                key_sm.last_time = current_time;  // 记录当前时间
            }
            break;
        case KEY_DOWN_DELAY:
            if(pin_value == 1){   // 释放
                key_sm.state = KEY_UP;
            }else{             // 检测消抖时间是否已过
                if((current_time - key_sm.last_time) >= KEY_DEBOUNCE_TIME){   // 避免了超时溢出，下面这种方式会发生溢出：current_time >= key_sm.last_time + KEY_DEBOUNCE_TIME
                    key_sm.state = KEY_DOWN;

                    // 按键按下，调用回调函数
                    if(on_press) on_press();
                }
            }
            break;
        case KEY_DOWN:
            if(pin_value == 1){   // 释放
                key_sm.state = KEY_UP_DELAY;
                key_sm.last_time = current_time;  // 记录当前时间
            }
            break;
        case KEY_UP_DELAY:
            if(pin_value == 0){   // 按下
                key_sm.state = KEY_DOWN;
            }else{
                if((current_time - key_sm.last_time) >= KEY_DEBOUNCE_TIME){
                    key_sm.state = KEY_UP;

                    // 按键松手，调用回调函数
                    if(on_release) on_release();
                }
            }
            break;
    }
}

