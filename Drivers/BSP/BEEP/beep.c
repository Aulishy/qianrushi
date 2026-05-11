#include "./BSP/BEEP/beep.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIM2/tim2.h"

extern TIM_HandleTypeDef g_tim2_handle;

/**
 * @brief 蜂鸣器初始化
 * @param 无
 * @retval 无
 */
void beep_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    /* 使能GPIO时钟 */
    BEEP_GPIO_CLK_ENABLE();
    
    /* 配置GPIO引脚 */
    gpio_init_struct.Pin = BEEP_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;     // 推挽输出
    gpio_init_struct.Pull = GPIO_PULLDOWN;            // 默认下拉
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;     // 低速
    
    HAL_GPIO_Init(BEEP_GPIO_PORT, &gpio_init_struct);
    
    /* 初始化TIM2：预分频设为72-1，得到1MHz的计数频率（1us/step） */
    tim2_int_init(1000, 72 - 1);
    HAL_TIM_Base_Stop_IT(&g_tim2_handle);             /* 初始状态停止计数 */

    /* 初始状态：关闭蜂鸣器 */
    BEEP_OFF();
}

/**
 * @brief 打开蜂鸣器
 */
void beep_on(void)
{
    BEEP_ON();
}

/**
 * @brief 关闭蜂鸣器
 */
void beep_off(void)
{
    BEEP_OFF();
}

/**
 * @brief 短鸣一声（100ms）
 */
void beep_short(void)
{
    BEEP_ON();
    delay_ms(100);
    BEEP_OFF();
}

/**
 * @brief 短鸣两声（用于按键提示）
 */
void beep_double(void)
{
    BEEP_ON();
    delay_ms(100);
    BEEP_OFF();
    delay_ms(100);
    BEEP_ON();
    delay_ms(100);
    BEEP_OFF();
}

/**
 * @brief 长鸣一声（500ms，用于倒计时结束）
 */
void beep_long(void)
{
    BEEP_ON();
    delay_ms(500);
    BEEP_OFF();
}

/**
 * @brief 以特定频率播放音符 (频率控制核心函数)
 * @param freq: 频率 (单位: Hz)
 * @param duration_ms: 持续时间 (单位: ms)
 */
static void beep_play_note(uint16_t freq, uint16_t duration_ms)
{
    if (freq == 0) {
        HAL_TIM_Base_Stop_IT(&g_tim2_handle);
        BEEP_OFF();
        delay_ms(duration_ms);
        return;
    }
    
    /* 计算自动重装载值：因为每半个周期翻转一次电平，所以中断频率应为目标频率的2倍 */
    /* ARR = 1MHz / (freq * 2) - 1 */
    uint32_t arr = (1000000 / (freq * 2)) - 1;
    
    __HAL_TIM_SET_AUTORELOAD(&g_tim2_handle, arr);
    __HAL_TIM_SET_COUNTER(&g_tim2_handle, 0);
    
    /* 断奏效果逻辑：播放时间的 80% */
    uint32_t play_ms = (duration_ms * 80) / 100;
    uint32_t pause_ms = duration_ms - play_ms;

    HAL_TIM_Base_Start_IT(&g_tim2_handle);            /* 开启硬件定时翻转 */
    delay_ms(play_ms);
    
    HAL_TIM_Base_Stop_IT(&g_tim2_handle);             /* 停止计数 */
    BEEP_OFF();                                       /* 确保引脚回到低电平 */
    delay_ms(pause_ms);
}

/**
 * @brief 定时器溢出回调函数
 * @note  此函数由 HAL_TIM_IRQHandler 调用，优先级极高，彻底消除软件杂音
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        BEEP_TOGGLE();
    }
}

/**
 * @brief 播放 "do re mi fa so la si do" (中音C调)
 */
void beep_play_doremi(void)
{
    // 中音C调频率表: do, re, mi, fa, so, la, si, do(高)
    uint16_t freqs[] = {262, 294, 330, 349, 392, 440, 494, 523};

    for(uint8_t i = 0; i < 8; i++)
    {
        beep_play_note(freqs[i], 500); // 延长音符持续时间到 500ms
        delay_ms(300);                 // 增加音符间的停顿到 300ms
    }
}

/**
 * @brief 播放简单的提示音（3短1长，表示倒计时完成）
 */
void beep_play_finish(void)
{
    beep_short();           // 嘀
    delay_ms(100);
    beep_short();           // 嘀
    delay_ms(100);
    beep_short();           // 嘀
    delay_ms(100);
    beep_long();            // 嘀---
}

/**
 * @brief 播放 "do re mi fa so la xi" (7个音符)
 * @note  由于本系统是无源蜂鸣器简单驱动，此处通过控制鸣叫时长模拟，
 *        在 TaskBeep 任务中调用时，不会阻塞高优先级的显示任务。
 */
void beep_play_scale(void)
{
    // 播放 7 个音阶: do re mi fa so la xi (高音)
    uint16_t scale_freqs[] = {523, 587, 659, 698, 784, 880, 988};
    for(uint8_t i = 0; i < 7; i++)
    {
        beep_play_note(scale_freqs[i], 500); // 延长音符持续时间到 500ms
    }
}

/**
 * @brief 播放尖锐的单次报警音
 */
void beep_alarm(void)
{
    /* 播放 1000Hz 的高频尖锐声音，持续 200ms */
    beep_play_note(1000, 200);
}