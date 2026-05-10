// tim3.h
#ifndef __TIM3_H
#define __TIM3_H

#include "./SYSTEM/sys/sys.h"

#define GTIM_TIMX_INT           TIM3
#define GTIM_TIMX_INT_CLK_ENABLE()  __HAL_RCC_TIM3_CLK_ENABLE()
#define GTIM_TIMX_INT_IRQn      TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler TIM3_IRQHandler


extern TIM_HandleTypeDef g_timx_handle;

/* 时间管理结构体 */
typedef struct {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} SysTime_t;

typedef struct {
    uint8_t m;
    uint8_t s;
    uint8_t is_running;
    uint8_t is_display_mode; // 0:正常时钟, 1:倒计时
    uint8_t is_finished;
} Countdown_t;

/* 全局状态变量 */
extern uint8_t hour, minute, second;
extern uint8_t cntd_m, cntd_s;
extern uint8_t cntd_run;
extern uint8_t cntd_display;
extern uint8_t clock_run;
extern uint8_t count_finish;

/* 功能接口 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc);

/* 高内聚逻辑控制 */
void Time_Update_Tick(void);               // ISR调用：处理秒级逻辑
void Display_Refresh(void);                // 任务调用：处理UI刷新

#endif
