#include "tilt.h"
#include "./SYSTEM/delay/delay.h"

/**
  * @brief  倾斜传感器 PB5 初始化（上拉输入）
  * @retval 无
  */
void TILT_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    // 使能 GPIOB 时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // PB5 上拉输入模式
    gpio_init_struct.Pin = TILT_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;   // 上拉输入，抗干扰强
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(TILT_PORT, &gpio_init_struct);
}

/**
  * @brief  读取倾斜传感器状态
  * @retval 1=平放   0=倒置/倾斜
  */
uint8_t TILT_Read(void)
{
    // 消抖
    delay_ms(10);

    if(HAL_GPIO_ReadPin(TILT_PORT, TILT_PIN) == 0)
    {
        return TILT_NORMAL;   // 倒置
    }
    else
    {
        return TILT_STABLE;   // 平放
    }
}
