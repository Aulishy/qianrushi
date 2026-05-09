#ifndef __TIM2_H
#define __TIM2_H

#include "./SYSTEM/sys/sys.h"

/* 外部变量声明 */
extern volatile uint8_t beep_10s_flag;     // 10秒蜂鸣器播放标志
extern volatile uint8_t beep_play_count;   // 蜂鸣器播放次数计数

/* 函数声明 */
void tim2_int_init(uint16_t arr, uint16_t psc);  // 定时器2初始化
void beep_check_and_play(void);                   // 检查并播放10秒蜂鸣器

#endif