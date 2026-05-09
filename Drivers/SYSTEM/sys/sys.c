/**
 * ****************************************************************************************************
 * @file        sys.c
 * @author      HNUST
 * @version     V1.0
 * @date        2024-07-21
 * @brief       EBD-A23 HAL库系统初始化相关函数（时钟配置/中断优先级/GPIO等）
 * @license     Copyright (c) 2024, 湖南科技大学信息与电气工程学院
 *
 * 注意：部分延时函数使用了空闲总线，基于STM32硬件架构的HAL库实现
 ****************************************************************************************************
 */

 #include "./SYSTEM/sys/sys.h"


 /**
  * @brief       设置中断向量表偏移地址
  * @param       baseaddr: 基地址
  * @param       offset: 偏移量（必须是0或0X100的倍数）
  * @retval      无
  */
 void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
 {
     /* 设置NVIC的中断向量表偏移寄存器，VTOR的第29位保留，实际[8:0]有效 */
     SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
 }
 
 /**
  * @brief       执行WFI指令（执行完该指令后芯片进入休眠模式，等待中断唤醒）
  * @param       无
  * @retval      无
  */
 void sys_wfi_set(void)
 {
     __ASM volatile("wfi");
 }
 
 /**
  * @brief       关闭所有中断（除了硬故障和NMI中断）
  * @param       无
  * @retval      无
  */
 void sys_intx_disable(void)
 {
     __ASM volatile("cpsid i");
 }
 
 /**
  * @brief       开启所有中断
  * @param       无
  * @retval      无
  */
 void sys_intx_enable(void)
 {
     __ASM volatile("cpsie i");
 }
 
 /**
  * @brief       设置栈顶地址
  * @note        这里使用了内部函数，在此仅为MDK环境调用，实际映射到系统函数
  * @param       addr: 栈顶地址
  * @retval      无
  */
 void sys_msr_msp(uint32_t addr)
 {
     __set_MSP(addr);  /* 设置栈顶地址 */
 }
 
 /**
  * @brief       进入待机模式
  * @param       无
  * @retval      无
  */
 void sys_standby(void)
 {
     __HAL_RCC_PWR_CLK_ENABLE();    /* 使能电源时钟 */
     SET_BIT(PWR->CR, PWR_CR_PDDS); /* 进入待机模式 */
 }
 
 /**
  * @brief       系统软复位
  * @param       无
  * @retval      无
  */
 void sys_soft_reset(void)
 {
     NVIC_SystemReset();
 }
 
 /**
  * @brief       系统时钟初始化函数
  * @param       plln: PLL倍频系数（PLL倍频），取值范围：2~16
  *               注意：中断向量表位置已在SystemInit()中初始化
  * @retval      无
  */
 void sys_stm32_clock_init(uint32_t plln)
 {
     HAL_StatusTypeDef ret = HAL_ERROR;
     RCC_OscInitTypeDef rcc_osc_init = {0};
     RCC_ClkInitTypeDef rcc_clk_init = {0};
 
     rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;       /* 选择要配置的HSE */
     rcc_osc_init.HSEState = RCC_HSE_ON;                         /* 开启HSE */
     rcc_osc_init.HSEPredivValue = RCC_HSE_PREDIV_DIV1;          /* HSE预分频系数 */
     rcc_osc_init.PLL.PLLState = RCC_PLL_ON;                     /* 开启PLL */
     rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;             /* PLL时钟源选择HSE */
     rcc_osc_init.PLL.PLLMUL = plln;                             /* PLL倍频系数 */
     ret = HAL_RCC_OscConfig(&rcc_osc_init);                     /* 初始化 */
 
     if (ret != HAL_OK)
     {
         while (1);                                              /* 时钟初始化失败，在此处进行错误处理，可根据需要添加用户代码 */
     }
 
     /* 选择PLL作为系统时钟源，并配置HCLK、PCLK1和PCLK2 */
     rcc_clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
     rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;        /* 设置系统时钟来自PLL */
     rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;               /* AHB分频系数为1 */
     rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV2;                /* APB1分频系数为2 */
     rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV1;                /* APB2分频系数为1 */
     ret = HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_2);  /* 同时设置FLASH延时周期为2WS，可根据CPU主频调整 */
 
     if (ret != HAL_OK)
     {
         while (1);                                              /* 时钟初始化失败，在此处进行错误处理，可根据需要添加用户代码 */
     }
 }