 /* @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       EBD-A23 HAL库 按键驱动头文件
 ****************************************************************************************************
 */

#ifndef _KEY_H
#define _KEY_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 引脚 定义 */

#define KEY_GPIO_PORT                  GPIOC
#define KEY_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* GPIOC时钟使能 */
#define KEY1_GPIO_PIN                  GPIO_PIN_0
#define KEY2_GPIO_PIN                  GPIO_PIN_1
#define KEY3_GPIO_PIN                  GPIO_PIN_2
#define KEY4_GPIO_PIN                  GPIO_PIN_3

/******************************************************************************************/

#define KEY1        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY1_GPIO_PIN)     /* 读取KEY1电平 */
#define KEY2        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY2_GPIO_PIN)     /* 读取KEY2电平 */
#define KEY3        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY3_GPIO_PIN)     /* 读取KEY3电平 */
#define KEY4        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY4_GPIO_PIN)     /* 读取KEY4电平 */



#define KEY1_PRES    1              /* KEY1按下 */
#define KEY2_PRES    2              /* KEY2按下 */
#define KEY3_PRES    3              /* KEY3按下 */
#define KEY4_PRES    4              /* KEY4按下 */

void KEY_Init(void);                /* 按键初始化函数 */
uint8_t key_scan(uint8_t mode);     /* 按键扫描函数 */

#endif
