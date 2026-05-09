/**
 * @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       EBD-A23 HAL库 按键驱动源文件
 ****************************************************************************************************
 */
#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       按键初始化函数
 * @param       无
 * @retval      无
 */
void KEY_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOC_CLK_ENABLE();                               /* GPIOC时钟使能 */

    gpio_init_struct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;/* KEY1~4引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入模式 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 内部上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);                    /* KEY引脚模式设置及初始化 */
}

/**
 * @brief       按键扫描函数
 * @note        该函数响应优先级(同时按下多个按键时): KEY1 < KEY2 < KEY3 < KEY4!!
 * @param       mode: 0 / 1, 具体含义如下:
 *   @arg       0,  不支持连续按(当按键按下不放时，只有第一次调用会返回键值，
 *                  必须松开以后，再次按下才会返回其它键值)
 *   @arg       1,  支持连续按(当按键按下不放时，每次调用该函数都会返回键值)
 * @retval      返回键值, 具体如下:
 *              KEY1_PRES, 1, KEY1按下
 *              KEY2_PRES, 2, KEY2按下
 *              KEY3_PRES, 3, KEY3按下
 *              KEY4_PRES, 4, KEY4按下
*/
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;  /* 按键松开标志 */
    uint8_t keyval = 0;

    if (mode) key_up = 1;       /* 支持连按 */

    if (key_up && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 ||KEY4 == 0))  
    {                           /* 按键松开标志为1, 且有任意一个按键按下 */
        delay_ms(10);           /* 延时去抖动 */
        key_up = 0;

        if (KEY1 == 0)  keyval = KEY1_PRES;

        if (KEY2 == 0)  keyval = KEY2_PRES;

        if (KEY3 == 0)  keyval = KEY3_PRES;

        if (KEY4 == 0)  keyval = KEY4_PRES;
    }
    else if (KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1) 
    {                           /* 没有任何按键按下, 标记按键松开 */
        key_up = 1;
    }

    return keyval;              /* 返回键值 */
}
