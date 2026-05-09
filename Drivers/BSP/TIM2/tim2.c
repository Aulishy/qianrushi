#include "./BSP/TIM2/tim2.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/BEEP/beep.h"

TIM_HandleTypeDef g_tim2_handle;

/**
 * @note 原有的10秒定时逻辑已移除，TIM2现已作为蜂鸣器专用的频率驱动定时器
 */

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
 */
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim2_handle);
}