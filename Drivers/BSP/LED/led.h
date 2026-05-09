 * @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       EBD-A23 HAL库LED驱动头文件
 * @license     Copyright (c) 2024, 湖南科技大学信息与电气工程学院
 ****************************************************************************************************
 */

#ifndef _LED_H
#define _LED_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚定义 */

#define LED_GPIO_PORT                  GPIOE
#define LED_GPIO_PIN                   GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
#define LED_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)          /* GPIOE时钟使能 */

#define LEDSEL_GPIO_PORT               GPIOB
#define LEDSEL_GPIO_PIN                GPIO_PIN_4
#define LEDSEL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)          /* GPIOB时钟使能 */

/******************************************************************************************/
/* LED端口定义 */
#define LEDSEL(x) do{ x ? \
                      HAL_GPIO_WritePin(LEDSEL_GPIO_PORT, LEDSEL_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDSEL_GPIO_PORT, LEDSEL_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LEDSEL电平：0表示控制数码管，1表示控制LED灯 */
#define LEDSEL0(x) do{ x ? \
                      HAL_GPIO_WritePin(LEDSEL_GPIO_PORT, GPIO_PIN_0, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDSEL_GPIO_PORT, GPIO_PIN_0, GPIO_PIN_RESET); \
                   }while(0)      /* 设置LEDSEL0电平 */
#define LEDSEL1(x) do{ x ? \
                      HAL_GPIO_WritePin(LEDSEL_GPIO_PORT, GPIO_PIN_1, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDSEL_GPIO_PORT, GPIO_PIN_1, GPIO_PIN_RESET); \
                   }while(0)      /* 设置LEDSEL1电平 */
#define LEDSEL2(x) do{ x ? \
                      HAL_GPIO_WritePin(LEDSEL_GPIO_PORT, GPIO_PIN_2, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LEDSEL_GPIO_PORT, GPIO_PIN_2, GPIO_PIN_RESET); \
                   }while(0)      /* 设置LEDSEL2电平 */

#define LED1(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_8, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_8, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LED1电平 */
#define LED2(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_9, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_9, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LED2电平 */
#define LED3(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_10, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_10, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LED3电平 */
#define LED4(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_11, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_11, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LED4电平 */
#define LED5(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_12, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_12, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LED5电平 */
#define LED6(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_13, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_13, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LED6电平 */
#define LED7(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_14, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_14, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LED7电平 */
#define LED8(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_15, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_15, GPIO_PIN_RESET); \
                  }while(0)      /* 设置LED8电平 */

#define MSGLED1(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_0, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_0, GPIO_PIN_RESET); \
                  }while(0)      /* 设置消息LED1电平 */
#define MSGLED2(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_1, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_1, GPIO_PIN_RESET); \
                  }while(0)      /* 设置消息LED2电平 */
#define MSGLED3(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_2, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_2, GPIO_PIN_RESET); \
                  }while(0)      /* 设置消息LED3电平 */
#define MSGLED4(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_3, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED_GPIO_PORT, GPIO_PIN_3, GPIO_PIN_RESET); \
                  }while(0)      /* 设置消息LED4电平 */

/* 直接操作寄存器的方法控制IO */
#define digitalHi(p,i)          {p->BSRR=i;}                //设置为高电平
#define digitalLo(p,i)          {p->BSRR=(uint32_t)i << 16;}//设置为低电平
#define digitalToggle(p,i)      {p->ODR ^=i;}               //翻转状态

/* 控制输出IO的宏 */
#define L1_TOGGLE   digitalToggle(LED_GPIO_PORT,GPIO_PIN_8)
#define L1_OFF      digitalHi(LED_GPIO_PORT,GPIO_PIN_8)
#define L1_ON       digitalLo(LED_GPIO_PORT,GPIO_PIN_8)


/* LED翻转控制 */
#define LED1_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_8); }while(0)         /* 翻转LED1 */
#define LED2_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_9); }while(0)         /* 翻转LED2 */
#define LED3_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_10); }while(0)        /* 翻转LED3 */
#define LED4_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_11); }while(0)        /* 翻转LED4 */
#define LED5_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_12); }while(0)        /* 翻转LED5 */
#define LED6_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_13); }while(0)        /* 翻转LED6 */
#define LED7_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_14); }while(0)        /* 翻转LED7 */
#define LED8_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_15); }while(0)        /* 翻转LED8 */

#define MSGLED1_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_0); }while(0)      /* 翻转消息LED1 */
#define MSGLED2_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_1); }while(0)      /* 翻转消息LED2 */
#define MSGLED3_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_2); }while(0)      /* 翻转消息LED3 */
#define MSGLED4_TOGGLE()   do{ HAL_GPIO_TogglePin(LED_GPIO_PORT, GPIO_PIN_3); }while(0)      /* 翻转消息LED4 */

/******************************************************************************************/
/* 外部接口函数 */
void LED_Init(void);                                                                         /* 初始化 */
void SetLed(unsigned char w, unsigned char value);
void ToggleLED0(void);
