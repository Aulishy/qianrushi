/**
 * @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       EBD-A23 HAL库 按键驱动源文件
 ****************************************************************************************************
 */
#include "./BSP/KEY/key.h"


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
    /* 每个按键的历史状态移位寄存器，初始全为1(未按下) */
    static uint8_t key_history[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    /* 每个按键的确认状态，初始为1(松开) */
    static uint8_t key_state[4]   = {1, 1, 1, 1}; 
    /* 记录按键事件是否已经上报过(用于不支持连按模式) */
    static uint8_t key_sent[4]    = {0, 0, 0, 0}; 
    
    uint8_t keyval = 0;
    uint8_t current_pins[4];
    int i;

    /* 1. 获取本次(每5ms一次)的实时引脚电平 */
    current_pins[0] = KEY1;
    current_pins[1] = KEY2;
    current_pins[2] = KEY3;
    current_pins[3] = KEY4;

    /* 2. 状态机与移位历史记录判断 */
    for (i = 0; i < 4; i++)
    {
        /* 将新采样值移入最低位 */
        key_history[i] = (key_history[i] << 1) | current_pins[i];
        
        /* 判断连续4次采样(掩码0x0F，即二进制 0000 1111) */
        if ((key_history[i] & 0x0F) == 0x00)       /* 连续4次低电平，确认按键稳定按下 */
            key_state[i] = 0; 
        else if ((key_history[i] & 0x0F) == 0x0F)  /* 连续4次高电平，确认按键稳定松开 */
        {
            key_state[i] = 1; 
            key_sent[i] = 0;  /* 按键松开后，复位发送标志 */
        }
        
        /* 3. 产生键值 */
        if (key_state[i] == 0 && (mode == 1 || key_sent[i] == 0))
        {
            keyval = i + 1;   /* 对应宏定义 KEY1_PRES=1, KEY2_PRES=2 ... */
            key_sent[i] = 1;  /* 标记该按键动作已发送 */
        }
    }

    return keyval;              /* 返回键值 */
}
