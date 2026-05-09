/**
 ****************************************************************************************************
 * @file        usart.c
 * @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       EBD-A23 HAL库串口1初始化相关函数，支持printf
 * @license     Copyright (c) 2024, 湖南科技大学信息与电气工程学院
 *
 * 注意：部分延时函数使用了空闲总线，基于STM32硬件架构的HAL库实现
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"


/* 是否使用操作系统，如果有相关的头文件就包含进来. */
#if SYS_SUPPORT_OS
#include "os.h" /* 操作系统 使用 */
#endif

/******************************************************************************************/
/* 下面这部分代码是重定向printf函数，提供一个必须的选项use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器的时候 */
__asm(".global __use_no_semihosting\n\t");  /* 使用半主机模式禁用功能 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6需要确保main函数为无参数形式，否则会报错 */

#else
/* 使用AC5编译器的时候，需要定义__FILE 和 禁用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* 这里可以任意定义。如果唯一使用的文件是通过printf()进行调试的标准输出，则无需文件处理。 */
};

#endif

/* 禁用半主机模式时，需要定义_ttywrch\_sys_exit\_sys_command_string函数，可以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE 在stdio.h里定义. */
FILE __stdout;

/* MDK需要定义fputc函数，printf函数最终会通过调用fputc来实现串口发送 */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->SR & 0X40) == 0);     /* 等待上一次发送完成 */

    USART_UX->DR = (uint8_t)ch;             /* 将要发送的ch写入DR寄存器 */
    return ch;
}
#endif
/******************************************************************************************/

#if USART_EN_RX /* 如果使能了接收 */

/* 接收缓冲区, 最大USART_REC_LEN个字节. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接收状态
 *  bit15:      接收完成标志
 *  bit14:      收到0x0d
 *  bit13~0:    接收到的有效字节数目
*/
uint16_t g_usart_rx_sta = 0;

uint8_t g_rx_buffer[RXBUFFERSIZE];  /* HAL库使用的串口接收缓冲区 */

UART_HandleTypeDef g_uart1_handle;  /* UART句柄 */

/**
 * @brief       串口初始化函数
 * @param       baudrate: 波特率，根据需求设置波特率值
 * @note        注意: 若设置不正确的时钟源，可能导致串口波特率设置错误。
 *              对应的USART时钟源在sys_stm32_clock_init()函数里设置。
 * @retval      无
 */
void usart_init(uint32_t baudrate)
{
    /*UART 初始化设置*/
    g_uart1_handle.Instance        = USART_UX;              /* USART_UX */
    g_uart1_handle.Init.BaudRate   = baudrate;              /* 波特率 */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;    /* 字长为8位数据格式 */
    g_uart1_handle.Init.StopBits   = UART_STOPBITS_1;       /* 一个停止位 */
    g_uart1_handle.Init.Parity     = UART_PARITY_NONE;      /* 无奇偶校验位 */
    g_uart1_handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;   /* 无硬件流控 */
    g_uart1_handle.Init.Mode       = UART_MODE_TX_RX;       /* 收发模式 */
    HAL_UART_Init(&g_uart1_handle);                         /* HAL_UART_Init()会使能UART1 */

    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲器以及接收缓冲器最大接收数据个数 */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE); 
}

/**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄指针
 * @note        该函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;

    if (huart->Instance == USART_UX)                            /* 如果是串口1，进行串口1 MSP初始化 */
    {
        USART_TX_GPIO_CLK_ENABLE();                             /* 使能串口TX引脚时钟 */
        USART_RX_GPIO_CLK_ENABLE();                             /* 使能串口RX引脚时钟 */
        USART_UX_CLK_ENABLE();                                  /* 使能串口时钟 */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;               /* 串口发送引脚号 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO速度设置为高速 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct);
                
        gpio_init_struct.Pin = USART_RX_GPIO_PIN;               /* 串口RX引脚 模式设置 */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct);   /* 串口RX引脚 需设置为输入模式 */
        
#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);                      /* 使能USART1中断通道 */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3);              /* 抢占优先级2，子优先级3: 组2->抢占优先级3，响应优先级3 */
#endif
    }
}

/**
 * @brief       串口数据接收完成回调函数
 *              数据处理在此处完成
 * @param       huart:串口句柄
 * @retval      无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART_UX)                    /* 如果是串口1 */
    {
        if ((g_usart_rx_sta & 0x8000) == 0)             /* 接收未完成 */
        {
            if (g_usart_rx_sta & 0x4000)                /* 接收到了0x0d（回车） */
            {
                if (g_rx_buffer[0] != 0x0a)             /* 接收到的不是0x0a（换行） */
                {
                    g_usart_rx_sta = 0;                 /* 接收错误,重新开始 */
                }
                else                                    /* 接收到了0x0a（换行） */
                {
                    g_usart_rx_sta |= 0x8000;           /* 接收完成了 */
                }
            }
            else                                        /* 还没收到0X0d（回车） */
            {
                if (g_rx_buffer[0] == 0x0d)
                    g_usart_rx_sta |= 0x4000;
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0];
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0;             /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }

        HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
    }
}

/**
 * @brief       串口1中断服务函数
 * @param       无
 * @retval      无
 */
void USART_UX_IRQHandler(void)
{
#if SYS_SUPPORT_OS                          /* 使用OS */
    OSIntEnter();    
#endif

    HAL_UART_IRQHandler(&g_uart1_handle);   /* 调用HAL库中断处理公用函数 */

#if SYS_SUPPORT_OS                          /* 使用OS */
    OSIntExit();
#endif

}

#endif