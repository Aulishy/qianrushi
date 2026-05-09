#ifndef __IWDG_H
#define __IWDG_H

#include "./SYSTEM/sys/sys.h"

void IWDG_Init(uint16_t rlr);  // 初始化独立看门狗（设置重载值）
void IWDG_Feed(void);          // 喂狗（刷新计数器，你 tim3.c 里要调用这个）

#endif

