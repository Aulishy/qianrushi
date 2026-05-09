#include "./BSP/iwdg/iwdg.h"

/**
 * @brief  独立看门狗初始化
 * @param  rlr: 重装载值（12位，范围0~4095）
 * @note   时钟源LSI≈32kHz，预分频默认64（PR=4）
 *         超时时间 = (4 * 2^PR * RLR) / LSI频率
 *         例：rlr=625 → 约2秒（(4*64*625)/32000 ≈ 2s）
 */
void IWDG_Init(uint16_t rlr)
{
    /* 1. 解锁写保护（写入0x5555） */
    IWDG->KR = 0x5555;

    /* 2. 设置预分频系数为64（PR=4，对应时钟32kHz/64=500Hz） */
    IWDG->PR = 4;

    /* 3. 设置重装载值 */
    IWDG->RLR = rlr;

    /* 4. 重载计数器（写入0xAAAA，避免立即复位） */
    IWDG->KR = 0xAAAA;

    /* 5. 启动看门狗（写入0xCCCC，启动后不可关闭，只能复位） */
    IWDG->KR = 0xCCCC;
}

/**
 * @brief  喂狗（刷新计数器，防止复位）
 * @param  无
 * @retval 无
 */
void IWDG_Feed(void)
{
    /* 写入0xAAAA，重载计数器，关键操作！ */
    IWDG->KR = 0xAAAA;
}

