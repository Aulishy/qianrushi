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
        // ========= 正向时钟 =========
        if(clock_run)
        {
            second++;
            if(second >= 60)
            {
                second = 0;
                minute++;
                if(minute >= 60)
                {
                    minute = 0;
                    hour++;
                    if(hour >= 24) hour = 0;
                }
            }
        }

        // ========= 倒计时 =========
        if(cntd_run)
        {
            if(cntd_s > 0)
            {
                cntd_s--;
            }
            else
            {
                if(cntd_m > 0)
                {
                    cntd_m--;
                    cntd_s = 59;
                }
                else
                {
                    cntd_run = 0;
                    clock_run = 1;      // 倒计时结束，恢复时钟运行
                    count_finish = 1;
					cntd_display=0;
                    IWDG_Feed();
                    //beep_play_doremi();  // 播放音乐
                }
            }
        }

        

        __HAL_TIM_CLEAR_IT(&g_timx_handle, TIM_IT_UPDATE);
    }
}

/*****************************************************
* 显示函数：倒计时模式优先显示，否则显示正常时钟
******************************************************/
void DisplayDigitalClock(void)
{
    #define N 1
    // 显示倒计时
    if(cntd_display && (cntd_m>0 || cntd_s>0))
    {
        SetLed(0, 0);             delay_ms(N);
        SetLed(1, 0);             delay_ms(N);
        SetLed(2, 64);            delay_ms(N);
        SetLed(3, cntd_m / 10);   delay_ms(N);
        SetLed(4, cntd_m % 10);   delay_ms(N);
        SetLed(5, 64);            delay_ms(N);
        SetLed(6, cntd_s / 10);   delay_ms(N);
        SetLed(7, cntd_s % 10);   delay_ms(N);
    }
    // 显示正常时间
    else
    {
        SetLed(0, hour / 10);     delay_ms(N);
        SetLed(1, hour % 10);     delay_ms(N);
        SetLed(2, 64);            delay_ms(N);
        SetLed(3, minute / 10);   delay_ms(N);
        SetLed(4, minute % 10);   delay_ms(N);
        SetLed(5, 64);            delay_ms(N);
        SetLed(6, second / 10);   delay_ms(N);
        SetLed(7, second % 10);   delay_ms(N);
    }
}

/*****************************************************
* 按键控制：KEY1+分钟 KEY2开始倒计时 KEY3恢复正常时钟
******************************************************/
void CountDown_Set(void)
{
    uint8_t key = key_scan(0);

    // KEY1：倒计时 +1 分钟（不启动倒计时，保持时钟运行）
    if(key == KEY1_PRES)
    {
        cntd_m++;
        if(cntd_m >= 60) cntd_m = 0;
        cntd_s = 0;
        cntd_run = 0;      // 不启动倒计时
        clock_run = 1;     // 保持时钟运行，让用户能看到设置的时间
		cntd_display=1;
        count_finish = 0;  // 复位完成标志
    }

    // KEY2：开始倒计时
    if(key == KEY2_PRES)
    {
        if(cntd_m > 0 || cntd_s > 0)
        {
            cntd_run = 1;    // 启动倒计时
            clock_run = 0;   // 停止时钟
            count_finish = 0; // 复位完成标志
        }
    }

    // KEY3：回到正常正向计时
    if(key == KEY3_PRES)
    {
        hour = 0;
        minute = 0;
        second = 0;
        cntd_m = 0;
        cntd_s = 0;
		cntd_display=0;
        cntd_run = 0;
        clock_run = 1;      // 启动正常时钟
        count_finish = 0;   // 复位完成标志
    }
}