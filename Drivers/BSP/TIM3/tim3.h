// tim3.h
#ifndef __TIM3_H
#define __TIM3_H

#include "./SYSTEM/sys/sys.h"

#define GTIM_TIMX_INT           TIM3
#define GTIM_TIMX_INT_CLK_ENABLE()  __HAL_RCC_TIM3_CLK_ENABLE()
#define GTIM_TIMX_INT_IRQn      TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler TIM3_IRQHandler


extern TIM_HandleTypeDef g_timx_handle;

/* 实时时钟 */
extern uint8_t hour, minute, second;

/* 可调倒计时 */
extern uint8_t cntd_m, cntd_s;
extern uint8_t cntd_run;

/* 提醒标志 */
extern uint8_t remind_5min_flag;
extern uint8_t remind_1min_flag;

void gtim_timx_int_init(uint16_t arr, uint16_t psc);
void DisplayDigitalClock(void);
void CountDown_Set(void);
void Check_Reminder(void);  // 新增：检查倒计时提醒

#endif
