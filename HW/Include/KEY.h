#ifndef __KEY_H__
#define __KEY_H_
#include <stdint.h>

typedef enum{
    KEY_UP = 0,            // 抬起
    KEY_DOWN_DELAY,        // 按下消抖
    KEY_DOWN,              // 按下
    KEY_UP_DELAY           // 抬起消抖
}key_state;

typedef struct{
    key_state state;         // 状态
    uint32_t last_time;    // 时间
}key_SM_t;

void Key_Init(void);

void Key_scan_sm(uint32_t gpio_periph, uint32_t pin, void (*on_press) (void), void (*on_release) (void));


extern key_SM_t key_sm;

#endif
