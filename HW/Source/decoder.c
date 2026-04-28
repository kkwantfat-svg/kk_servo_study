#include "gd32f10x.h"                   // Device header
#include "gd32f10x_timer.h"
#include "gd32f10x_gpio.h"
#include "decoder.h"

void Decoder_init(void)
{
    // 开启时钟
    rcu_periph_clock_enable(RCU_TIMER2);
    rcu_periph_clock_enable(RCU_GPIOB);  
    rcu_periph_clock_enable(RCU_AF);   // 所需通道需要重定向

    // 引脚重定向
    gpio_afio_deinit();
    gpio_pin_remap_config(GPIO_TIMER2_PARTIAL_REMAP, ENABLE);

    // 初始化对应gpio输入通道的模式
    gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);  // timer2 通道0 与 通道1 配置为上拉输入

    // 初始化时基单元，配置预分频系数与自动重装载数值
    timer_parameter_struct timer_struct;
 
    /* deinit a TIMER */
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_struct);
    
    timer_struct.alignedmode = TIMER_COUNTER_EDGE;        // 边沿对齐模式
    timer_struct.clockdivision = TIMER_CKDIV_DIV1;        // 不分频
    timer_struct.counterdirection = TIMER_COUNTER_UP;     // 向上计数模式
    timer_struct.period = 10 - 1;      // 自动重装载数值
    timer_struct.prescaler = 2 - 1;   // 预分频数值
    
    /* initialize TIMER counter */
    timer_init(TIMER2, &timer_struct);

    // 配置输入捕获参数
    timer_ic_parameter_struct icpara_struct;
    timer_channel_input_struct_para_init(&icpara_struct);

    icpara_struct.icfilter = 15;
    icpara_struct.icpolarity = TIMER_IC_POLARITY_RISING;
    icpara_struct.icprescaler = TIMER_IC_PSC_DIV1;
    icpara_struct.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_input_capture_config(TIMER2, TIMER_CH_0, &icpara_struct);
    timer_input_capture_config(TIMER2, TIMER_CH_1, &icpara_struct);

    // 配置正交编码器模式
    timer_quadrature_decoder_mode_config(TIMER2, TIMER_QUAD_DECODER_MODE0, TIMER_IC_POLARITY_RISING, TIMER_IC_POLARITY_FALLING);

    // 使能时钟
    timer_enable(TIMER2);
}

