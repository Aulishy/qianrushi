#ifndef __BEEP_H
#define __BEEP_H

#include "./SYSTEM/sys/sys.h"

/* 引脚定义 */
#define BEEP_GPIO_PORT      GPIOE
#define BEEP_GPIO_PIN       GPIO_PIN_5
#define BEEP_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()

/* 蜂鸣器控制宏 */
#define BEEP_ON()           HAL_GPIO_WritePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_PIN_SET)
#define BEEP_OFF()          HAL_GPIO_WritePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_PIN_RESET)
#define BEEP_TOGGLE()       HAL_GPIO_TogglePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN)

/* 函数声明 */
void beep_init(void);           // 蜂鸣器初始化
void beep_on(void);             // 打开蜂鸣器
void beep_off(void);            // 关闭蜂鸣器
void beep_short(void);          // 短鸣一声
void beep_double(void);         // 短鸣两声
void beep_long(void);           // 长鸣一声
void beep_play_doremi(void);    // 播放do re mi音乐

#endif
