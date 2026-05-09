#include "./BSP/BEEP/beep.h"
#include "./SYSTEM/delay/delay.h"

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
 * @brief 播放"do re mi fa so la si do"音乐
 * @note  简化版，只使用延时控制
 */
void beep_play_doremi(void)
{
    /* 播放8个音符，每个音符200ms，间隔50ms */
    uint8_t i;
    
    for(i = 0; i < 8; i++)
    {
        BEEP_ON();
        delay_ms(200);      // 每个音符持续200ms
        BEEP_OFF();
        delay_ms(50);       // 音符间隔50ms
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