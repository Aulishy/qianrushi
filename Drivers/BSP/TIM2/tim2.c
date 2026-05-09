#include "./BSP/TIM2/tim2.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/BEEP/beep.h"

TIM_HandleTypeDef g_tim2_handle;

/* 蜂鸣器控制变量 */
volatile uint8_t beep_10s_flag = 0;      // 10秒蜂鸣器播放标志
volatile uint8_t beep_play_count = 0;    // 蜂鸣器播放次数计数

/**
 * @brief 定时器2中断初始化
 * @param arr: 自动重装载值
 * @param psc: 时钟预分频数
 * @note  定时器溢出时间计算: Tout = (arr+1)*(psc+1)/Tclk
 *        例如: Tclk=80MHz, psc=8000-1, arr=10000-1, 则Tout=1秒
 */
void tim2_int_init(uint16_t arr, uint16_t psc)
{
    /* 使能定时器2时钟 */
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    /* 配置定时器句柄 */
    g_tim2_handle.Instance = TIM2;
    g_tim2_handle.Init.Prescaler = psc;
    g_tim2_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim2_handle.Init.Period = arr;
    g_tim2_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&g_tim2_handle);
    
    /* 使能定时器更新中断 */
    __HAL_TIM_ENABLE_IT(&g_tim2_handle, TIM_IT_UPDATE);
    
    /* 设置中断优先级 */
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 2);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    
    /* 启动定时器中断 */
    HAL_TIM_Base_Start_IT(&g_tim2_handle);
}

/**
 * @brief 定时器2中断服务函数
 * @note  每1秒中断一次，累计10次触发蜂鸣器播放标志
 */
void TIM2_IRQHandler(void)
{
    static uint8_t second_counter = 0;
    
    if(__HAL_TIM_GET_FLAG(&g_tim2_handle, TIM_FLAG_UPDATE) != RESET)
    {
        /* 清除中断标志 */
        __HAL_TIM_CLEAR_IT(&g_tim2_handle, TIM_IT_UPDATE);
        
        /* 秒计数器加1 */
        second_counter++;
        
        /* 每10秒触发一次蜂鸣器 */
        if(second_counter >= 9)
        {
            second_counter = 0;
            beep_10s_flag = 1;      // 设置10秒播放标志
            beep_play_count++;       // 播放次数加1
        }
    }
}

/**
 * @brief 检查并播放10秒蜂鸣器（在主循环中调用）
 * @note  非阻塞方式，不会影响主程序运行
 */
void beep_check_and_play(void)
{
    static uint8_t last_flag = 0;
    
    /* 检测到标志位从0变1时播放 */
    if(beep_10s_flag == 1 && last_flag == 0)
    {
        beep_10s_flag = 0;   // 清除标志
        
        /* 播放提示音 - 短鸣两声，表示10秒到了 */
        beep_double();       // 嘀嘀两声
    }
    
    last_flag = beep_10s_flag;
}