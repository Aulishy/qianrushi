/**
 ****************************************************************************************************
 * @file        adc.h
 * @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       温度、光照、电压ADC采集驱动头文件
 ****************************************************************************************************
 */

#ifndef _ADC_H
#define _ADC_H

/* 包含必要的头文件 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
/* ADC通道定义 */
#define TEMP_ADC_CH     0   /* 温度传感器ADC通道 */
#define LIGHT_ADC_CH    1   /* 光照传感器ADC通道 */
#define VOLTAGE_ADC_CH  0   /* 电压测量ADC通道 */

/* 宏定义 */
#define V_Ro  PCin(4)       /* 参考电阻引脚 */
#define V_Rt  PCin(5)       /* 热敏电阻引脚 */

/* 函数声明 */
void AdcInit(void);                              /* ADC总初始化 */
void Adc1Init(void);                             /* ADC1初始化（温度/光照） */
void Adc2Init(void);                             /* ADC2初始化（电压测量） */
float GetVoltage(uint8_t adcx, uint8_t ch);      /* 获取电压值 */
uint16_t GetAdc(uint8_t adcx, uint8_t ch);       /* 获取ADC原始值 */

void TemperatureAndLightAdcInit(void);           /* 温度/光照ADC初始化 */
uint16_t GetTemperatureAdc(uint8_t ch);          /* 获取温度ADC值 */
float GetTemperature(void);                      /* 获取温度值(摄氏度) */
float GetCPUTemperature(void);                   /* 获取CPU温度（如有） */
uint16_t GetLightAdc(uint8_t ch);                /* 获取光照ADC值 */

#endif /* _ADC_H */