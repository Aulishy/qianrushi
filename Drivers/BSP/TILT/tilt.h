#ifndef __TILT_H
#define __TILT_H
#include <stdint.h>

// 倾斜传感器 引脚定义
#define TILT_PIN     GPIO_PIN_5
#define TILT_PORT    GPIOB

// 状态定义
#define TILT_NORMAL    0   // 倒置/倾斜
#define TILT_STABLE    1   // 平放/正常

void TILT_Init(void);      // 初始化
uint8_t TILT_Read(void);   // 读取状态

#endif
