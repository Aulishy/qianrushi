#include "./BSP/TIM3/tim3.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/IWDG/iwdg.h"

TIM_HandleTypeDef g_timx_handle;

/* 正向时钟 */
uint8_t hour = 0, minute = 0, second = 0;

/* 倒计时变量 */
uint8_t cntd_m = 0;    // 倒计时分钟
uint8_t cntd_s = 0;    // 倒计时秒
uint8_t cntd_run = 0;  // 倒计时运行标志
uint8_t clock_run = 1; // 默认正常时钟运行
uint8_t count_finish = 0; 
uint8_t cntd_display=0;

/* 内部状态转换辅助 */
static void Clock_HandleTick(void);
static void Countdown_HandleTick(void);

/**
 ****************************************************************************************************
 * @brief       通用定时器TIMX定时中断初始化
 ****************************************************************************************************
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();

    g_timx_handle.Instance = GTIM_TIMX_INT;
    g_timx_handle.Init.Prescaler = psc;
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_handle.Init.Period = arr;
    HAL_TIM_Base_Init(&g_timx_handle);

    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);

    HAL_TIM_Base_Start_IT(&g_timx_handle);
}

/**
 ****************************************************************************************************
 * @brief       定时器中断服务函数（1秒）
 ****************************************************************************************************
 */
void GTIM_TIMX_INT_IRQHandler(void)
{
    if(__HAL_TIM_GET_FLAG(&g_timx_handle, TIM_FLAG_UPDATE) != RESET)
    {
        Time_Update_Tick();
        __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);
    }
}

/**
 * @brief 处理每秒一次的时间更新逻辑（高内聚）
 */
void Time_Update_Tick(void)
{
    if(clock_run) Clock_HandleTick();
    if(cntd_run)  Countdown_HandleTick();
}

static void Clock_HandleTick(void)
{
    if(++second >= 60)
    {
        second = 0;
        if(++minute >= 60)
        {
            minute = 0;
            if(++hour >= 24) hour = 0;
        }
    }
}

static void Countdown_HandleTick(void)
{
    if(cntd_s > 0)
    {
        cntd_s--;
    }
    else if(cntd_m > 0)
    {
        cntd_m--;
        cntd_s = 59;
    }
    else // 计时完成
    {
        cntd_run = 0;
        clock_run = 1;
        count_finish = 1;
        cntd_display = 0;
        IWDG_Feed();
    }
}

/**
 * @brief UI显示刷新（剥离了延时，适合在任务循环中调用）
 */
void Display_Refresh(void)
{
    if(cntd_display && (cntd_m > 0 || cntd_s > 0))
    {
        /* 显示内容： 00-MM-SS */
        SetLed(0, 0);            
        SetLed(1, 0);            
        SetLed(2, 10); // 杠
        SetLed(3, cntd_m / 10);  
        SetLed(4, cntd_m % 10);  
        SetLed(5, 10);
        SetLed(6, cntd_s / 10);  
        SetLed(7, cntd_s % 10);  
    }
    else
    {
        /* 显示正常时钟 HH-MM-SS */
        SetLed(0, hour / 10);    
        SetLed(1, hour % 10);    
        SetLed(2, 10);
        SetLed(3, minute / 10);  
        SetLed(4, minute % 10);  
        SetLed(5, 10);
        SetLed(6, second / 10);  
        SetLed(7, second % 10);  
    }
}