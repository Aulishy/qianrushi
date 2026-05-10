/**
 ****************************************************************************************************
 * @file        led.c
 * @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       EBD-A23 HAL库LED驱动源文件
 * @license     Copyright (c) 2024, 湖南科技大学信息与电气工程学院
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"

/***************************私有全局变量***************************/
uint8_t segTable[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40};

/**
 * @brief       初始化LED相关IO口, 使用前调用
 * @param       无
 * @retval      无
 */
void LED_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOE_CLK_ENABLE();                           /* GPIOE时钟使能 */
    __HAL_RCC_GPIOB_CLK_ENABLE();                           /* GPIOB时钟使能 */

    __HAL_RCC_AFIO_CLK_ENABLE();
    //注意在stm32f1xx_hal_msp.c中已经关闭JTAG，这里重复关闭一下
    __HAL_AFIO_REMAP_SWJ_NOJTAG();                          // 禁用JTAG，仅SWD可用，PB3,PB4可当作普通IO使用
    //__HAL_AFIO_REMAP_SWJ_NONJTRST();                      // 使用JTAG，仅禁用JTRST，SWD可用，PB4可当作普通IO使用

    gpio_init_struct.Pin = ((uint16_t)0x000F) | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                           GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15; /* PE8-15引脚 加上PE0-3 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */

    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);                /* 初始化GPIOE的8-15引脚 */

    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4; /* PB0-2,4引脚 */
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);                /* 初始化PB0-2,4引脚 */

    /*配置GPIO引脚输出电平 */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
                      | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE, ((uint16_t)0x000F), GPIO_PIN_RESET);  // PE0-3输出低电平

    /*配置GPIO引脚输出电平 */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

/***************************************
* 流水灯显示，传入数值对应显示二进制值
* value: 显示对应的数值，对应LED二进制值
****************************************/
void LedValue(uint8_t value)
{
// 方法一：
//    LED1(((value & 0x01) ? 1 : 0));
//    LED2(((value & 0x02) ? 1 : 0));
//    LED3(((value & 0x04) ? 1 : 0));
//    LED4(((value & 0x08) ? 1 : 0));
//    LED5(((value & 0x10) ? 1 : 0));
//    LED6(((value & 0x20) ? 1 : 0));
//    LED7(((value & 0x40) ? 1 : 0));
//    LED8(((value & 0x80) ? 1 : 0));
// 方法二：
//    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8,  (value&0x01)?GPIO_PIN_SET:GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9,  (value&0x02)?GPIO_PIN_SET:GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, (value&0x04)?GPIO_PIN_SET:GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, (value&0x08)?GPIO_PIN_SET:GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, (value&0x10)?GPIO_PIN_SET:GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, (value&0x20)?GPIO_PIN_SET:GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, (value&0x40)?GPIO_PIN_SET:GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, (value&0x80)?GPIO_PIN_SET:GPIO_PIN_RESET);

// 方法三：
    GPIOE->ODR &= ~(0xff << 8);
    GPIOE->ODR |= value << 8;

// 方法四：
//    GPIOE->BRR = 0xff<<8;
//    GPIOE->BSRR = value<<8;
}

/***************************************
* 数码管显示一位数字的数值
* 参数 w: 显示的位置索引，从0开始，范围0-7
*      value: 要显示的数字值
****************************************/
void SetLed(uint8_t w, uint8_t value)
{
    GPIOB->ODR &= ~(0x07 << 0);
    GPIOB->ODR |= (w & 0x07);
    LedValue(segTable[value]);
}

/***************************************
* 数码管显示一位带小数点的数值
* 参数 w: 显示的位置索引，从0开始，范围0-7
*      value: 要显示的数字值
****************************************/
void PortationDisplay(uint8_t w, uint8_t value)
{
    GPIOB->ODR &= ~(0x07 << 0);
    GPIOB->ODR |= (w & 0x07);
    LedValue(segTable[value] + 0x80);
}