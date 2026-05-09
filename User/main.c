/**
 ****************************************************************************************************
 * @file        main.c
 * @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       uC/OS-II 多任务示例程序 - 显示温度和光照
 * @license     Copyright (c) 2024, 湖南科技大学信息与电气工程学院
 * 
 * 功能说明：
 *   1. ADC任务：实时采集环境信息（温度、光照）
 *   2. 串口任务：与上位机通信，接收控制命令
 *   3. 显示任务：在数码管上交替显示温度和光照
 ****************************************************************************************************
 */

#include "SYSTEM/sys/sys.h"
#include "SYSTEM/usart/usart.h"
#include "SYSTEM/delay/delay.h"
#include "BSP/LED/led.h"
#include "BSP/adc/adc.h"
#include "BSP/TILT/tilt.h"
#include "BSP/BEEP/beep.h"
#include "BSP/KEY/key.h"
#include "BSP/TIM3/tim3.h"

#include "os.h"
#include "cpu.h"

/* ======================== 数据类型定义 ======================== */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

/* ======================== 任务配置参数 ======================== */

/* 任务堆栈大小 */
#define ADC_TASK_STK_SIZE      128     /* ADC采集任务堆栈 */
#define UART_TASK_STK_SIZE     128     /* 串口通信任务堆栈 */
#define DISPLAY_TASK_STK_SIZE  128     /* 显示任务堆栈 */
#define KEY_TASK_STK_SIZE      128     /* 按键任务堆栈 */
#define BEEP_TASK_STK_SIZE     128     /* 蜂鸣器任务堆栈 */
#define START_TASK_STK_SIZE    128     /* 启动任务堆栈 */

/* 任务优先级（数字越小优先级越高） */
#define ADC_TASK_PRIO          5       /* ADC采集任务优先级 */
#define UART_TASK_PRIO         6       /* 串口通信任务优先级 */
#define DISPLAY_TASK_PRIO      7       /* 显示任务优先级 */
#define KEY_TASK_PRIO          8       /* 按键任务优先级 */
#define BEEP_TASK_PRIO         9       /* 蜂鸣器任务优先级 */
#define START_TASK_PRIO        10      /* 启动任务优先级（最低） */

/* ======================== 全局变量 ======================== */

/* 任务堆栈 */
OS_STK  g_adc_task_stk[ADC_TASK_STK_SIZE];
OS_STK  g_uart_task_stk[UART_TASK_STK_SIZE];
OS_STK  g_display_task_stk[DISPLAY_TASK_STK_SIZE];
OS_STK  g_key_task_stk[KEY_TASK_STK_SIZE];
OS_STK  g_beep_task_stk[BEEP_TASK_STK_SIZE];
OS_STK  g_start_task_stk[START_TASK_STK_SIZE];

/* 信号量：用于触发音乐播放 */
OS_EVENT *g_beep_sem;

/* 系统运行标志 */
volatile uint8_t g_system_running = 1;

/* 当前采集的数据 */
volatile float   g_current_temp = 25.0f;    /* 当前温度 */
volatile uint16_t g_current_light = 0;      /* 当前光照 */

/* 显示模式状态机：0=时钟, 1=温度, 2=光照 */
volatile uint8_t g_display_mode = 0;

/* ======================== 函数声明 ======================== */

static void TaskStart(void *pdata);      /* 启动任务 */
static void TaskAdc(void *pdata);        /* ADC采集任务 */
static void TaskUart(void *pdata);       /* 串口通信任务 */
static void TaskDisplay(void *pdata);    /* 显示任务 */
static void TaskKey(void *pdata);        /* 按键扫描任务 */
static void TaskBeep(void *pdata);       /* 蜂鸣器播放任务 */
static void ProcessCommand(uint8_t cmd); /* 处理命令 */
static void SystemHardwareInit(void);    /* 硬件初始化 */

/* ======================== 主函数 ======================== */

int main(void)
{
    SystemHardwareInit();
    OSInit();
    OSTaskCreate(TaskStart, (void *)0, 
                 (OS_STK *)&g_start_task_stk[START_TASK_STK_SIZE - 1], 
                 START_TASK_PRIO);
    OSStart();
    return 0;
}

/* ======================== 硬件初始化 ======================== */

static void SystemHardwareInit(void)
{
    HAL_Init();
    sys_stm32_clock_init(RCC_PLL_MUL9);
    delay_init(72);
    LED_Init();
    usart_init(115200);
    TemperatureAndLightAdcInit();       /* 初始化温度/光照ADC */
    Adc1Init();
    Adc2Init();
    TILT_Init(); 
	beep_init();
    KEY_Init();                         /* 初始化按键外设 */
    gtim_timx_int_init(10000 - 1, 7200 - 1); /* 初始化TIM3定时器(提供1秒时钟跳动) */
    printf("\r\n========================================\r\n");
    printf("    uC/OS-II Multi-Task System Start\r\n");
    printf("    Temperature and Light Display\r\n");
    printf("========================================\r\n\r\n");
}

/* ======================== 启动任务 ======================== */

static void TaskStart(void *pdata)
{
    OS_CPU_SR cpu_sr = 0u;
    pdata = pdata;
    
    /* 创建信号量 */
    g_beep_sem = OSSemCreate(0);

    OS_ENTER_CRITICAL();
    OSTaskCreate(TaskAdc, (void *)0, 
                 (OS_STK *)&g_adc_task_stk[ADC_TASK_STK_SIZE - 1], 
                 ADC_TASK_PRIO);
    OSTaskCreate(TaskUart, (void *)0, 
                 (OS_STK *)&g_uart_task_stk[UART_TASK_STK_SIZE - 1], 
                 UART_TASK_PRIO);
    OSTaskCreate(TaskDisplay, (void *)0, 
                 (OS_STK *)&g_display_task_stk[DISPLAY_TASK_STK_SIZE - 1], 
                 DISPLAY_TASK_PRIO);
    OSTaskCreate(TaskKey, (void *)0, 
                 (OS_STK *)&g_key_task_stk[KEY_TASK_STK_SIZE - 1], 
                 KEY_TASK_PRIO);
    OSTaskCreate(TaskBeep, (void *)0, 
                 (OS_STK *)&g_beep_task_stk[BEEP_TASK_STK_SIZE - 1], 
                 BEEP_TASK_PRIO);
    OSTaskSuspend(START_TASK_PRIO);
    OS_EXIT_CRITICAL();
}

/* ======================== ADC采集任务 ======================== */

static void TaskAdc(void *pdata)
{
    pdata = pdata;
    
    printf("[ADC Task] Start successful...\r\n");
    
    while (g_system_running)
    {
        /* 采集温度 */
        g_current_temp = GetTemperature();
        
        /* 采集光照 */
        g_current_light = GetLightAdc(14);  /* 使用通道14采集光照 */
        
        printf("[ADC] Temp: %.1f C, Light: %d\r\n", g_current_temp, g_current_light);
        
        OSTimeDlyHMSM(0, 0, 1, 0);  /* 每1秒采集一次 */
    }
}

/* ======================== 串口通信任务 ======================== */

static void TaskUart(void *pdata)
{
    pdata = pdata;
    uint8_t rx_cmd;
    
    printf("[UART Task] Ready, commands: T(Temp), L(Light), S(State), R(Reset)\r\n");
    
    while (g_system_running)
    {
        if (g_usart_rx_sta & 0x8000)  /* 收到完整命令 */
        {
            rx_cmd = g_usart_rx_buf[0];
            g_usart_rx_sta = 0;
            ProcessCommand(rx_cmd);
        }
        OSTimeDlyHMSM(0, 0, 0, 50);
    }
}


/* ======================== 显示任务 ======================== */
static void TaskDisplay(void *pdata)
{
    pdata = pdata;
    uint8_t temp_int, temp_dec;
    uint8_t light_d1, light_d2, light_d3, light_d4;
    
    printf("[Display Task] Start successful...\r\n");
    
    while (g_system_running)
    {
        /* 状态指示灯（低电平0为亮）：根据当前模式亮起对应的指示灯 */
        LED1((g_display_mode == 0) ? 0 : 1);
        LED2((g_display_mode == 1) ? 0 : 1);
        LED3((g_display_mode == 2) ? 0 : 1);
        
        /* ========== 获取最新数据 ========== */
        temp_int = (uint8_t)g_current_temp;
        temp_dec = (uint8_t)((g_current_temp - temp_int) * 10);
        
        light_d1 = g_current_light / 1000;
        light_d2 = (g_current_light % 1000) / 100;
        light_d3 = (g_current_light % 100) / 10;
        light_d4 = g_current_light % 10;
        
        /* ========== 刷新数码管 (单次刷新，减小对其他任务的干扰) ========== */
        if(g_display_mode == 0)  /* 模式0：显示实时时钟 */
        {
            SetLed(0, hour / 10);      delay_us(100);
            SetLed(1, hour % 10);      delay_us(100);
            SetLed(2, 10);             delay_us(100);
            SetLed(3, minute / 10);    delay_us(100);
            SetLed(4, minute % 10);    delay_us(100);
            SetLed(5, 10);             delay_us(100);
            SetLed(6, second / 10);    delay_us(100);
            SetLed(7, second % 10);    delay_us(100);
        }
        else if(g_display_mode == 1)  /* 模式1：显示温度 */
        {
            SetLed(0, temp_int / 10);           delay_us(100);
            PortationDisplay(1, temp_int % 10); delay_us(100);
            SetLed(2, temp_dec);                delay_us(100);
        }
        else if(g_display_mode == 2)  /* 模式2：显示光照强度 */
        {
            if(g_current_light >= 1000) {
                SetLed(4, light_d1);   delay_us(100);
            }
            SetLed(5, light_d2);       delay_us(100);
            SetLed(6, light_d3);       delay_us(100);
            SetLed(7, light_d4);       delay_us(100);
        }
        
        LedValue(0x0000); /* 清除段码，防止重影 */
        
        /* 让出微小的时间片，增加系统调度频率以获得更平滑的音频 */
        OSTimeDlyHMSM(0, 0, 0, 2);
    }
}

/* ======================== 按键扫描任务 ======================== */

static void TaskKey(void *pdata)
{
    pdata = pdata;
    uint8_t key_val;
    
    printf("[KEY Task] Start successful...\r\n");
    
    while(g_system_running)
    {
        key_val = key_scan(0);  /* 扫描按键，不支持连按 */
        
        if(key_val == KEY1_PRES)
        {
            /* KEY1按下，循环切换显示模式 */
            g_display_mode++;
            if(g_display_mode > 2) g_display_mode = 0;
            
            printf("[State] Display Mode Changed: %d\r\n", g_display_mode);
            beep_double(); /* 蜂鸣器滴嘀两声提示 */
        }
        
        if(key_val == KEY2_PRES)
        {
            /* KEY2按下，释放信号量通知播放任务 */
            OSSemPost(g_beep_sem);
            printf("[Action] Playing Do-Re-Mi...\r\n");
        }
        
        OSTimeDlyHMSM(0, 0, 0, 20);  /* 每20ms扫描一次按键，配合驱动内部去抖 */
    }
}

/* ======================== 蜂鸣器播放任务 ======================== */

static void TaskBeep(void *pdata)
{
    uint8_t err;
    pdata = pdata;
    
    while(g_system_running)
    {
        /* 永久等待信号量 */
        OSSemPend(g_beep_sem, 0, &err);
        
        if(err == OS_ERR_NONE)
        {
            /* 调用底层驱动播放音阶 */
            beep_play_scale();
        }
    }
}

/* ======================== 命令处理 ======================== */

static void ProcessCommand(uint8_t cmd)
{
    switch (cmd)
    {
        case 'T':
        case 't':
            printf("Temperature: %.1f C\r\n", g_current_temp);
            break;
            
        case 'L':
        case 'l':
            printf("Light: %d\r\n", g_current_light);
            break;
            
        case 'S':
        case 's':
            printf("\r\n========== System Status ==========\r\n");
            printf("Temperature: %.1f C\r\n", g_current_temp);
            printf("Light: %d\r\n", g_current_light);
            printf("====================================\r\n");
            break;
            
        case 'R':
        case 'r':
            printf("System rebooting...\r\n");
            delay_ms(100);
            sys_soft_reset();
            break;
            
        default:
            printf("Commands: T(Temp), L(Light), S(State), R(Reset)\r\n");
            break;
    }
}