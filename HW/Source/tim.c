#include "gd32f10x_timer.h"

/**
 * Function Name: tim2_init
 * Description: Initialize TIMER2
 */
void tim2_init(void)
{
    rcu_periph_clock_enable(RCU_TIMER2);   // init clock
    timer_parameter_struct timer_struct;
    
    /* deinit a TIMER */
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_struct);

    timer_internal_clock_config(TIMER2);  // 内部时钟
    
    /* 内部时钟 10ms */
    timer_struct.alignedmode = TIMER_COUNTER_EDGE;        // 边沿对齐模式
    timer_struct.clockdivision = TIMER_CKDIV_DIV1;        // 不分频
    timer_struct.counterdirection = TIMER_COUNTER_UP;     // 向上计数模式
    timer_struct.period = 10000 - 1;             // 自动重装载数值
    timer_struct.prescaler = 108 - 1;           // 预分频数值
    
    /* initialize TIMER counter */
    timer_init(TIMER2, &timer_struct);

    // 清除标志位，不然使能中断后会立刻进入中断服务函数
    timer_flag_clear(TIMER2, TIMER_INT_FLAG_UP);

    timer_interrupt_enable(TIMER2, TIMER_INT_UP);   // 使能更新中断

    /* enable TIMER */
    timer_enable(TIMER2);
}

/*
 * Function Name: nvic_config
 * Description: Configure the Nested Vectored Interrupt Controller (NVIC) for TIMER2
 */
void nvic_config(void)
{
    nvic_irq_enable(TIMER2_IRQn, 0, 0);   // 使能定时器2中断
}


// void TIMER2_IRQHandler(void)
// {
//     if(timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP) != RESET){
//         timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);   // 清除更新中断标志

//         // user code

//     }
// }
