/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-06
 * @brief       高级定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32MP1开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200506
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"

/*******************************互补输出带死区控制程序**************************************/

TIM_HandleTypeDef g_timx_cplm_pwm_handle;                              /* 定时器x句柄 */

/**
 * @brief       高级定时器TIMX 互补输出 初始化函数（使用PWM模式1）
 * @note
 *              配置高级定时器TIMX 互补输出, 一路OC3 一路OC3N, 并且可以设置死区时间
 *              高级定时器的时钟来自APB2,当APB2DIV≥2分频的时候
 *              高级定时器的时钟为APB2时钟的2倍, 而APB2为104.5M, 所以定时器时钟 = 209Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    atim_timx_cplm_pwm_gpio_init();			/* GPIO初始化 */
    ATIM_TIMX_CPLM_CLK_ENABLE();				/* TIM1 时钟使能 */

    g_timx_cplm_pwm_handle.Instance = TIM1;																						/* 定时器1 */
    g_timx_cplm_pwm_handle.Init.Prescaler = psc;																			/* 定时器分频 */
    g_timx_cplm_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;											/* 向上计数模式*/
    g_timx_cplm_pwm_handle.Init.Period = arr;																					/* 自动重装载值 */
    g_timx_cplm_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;								/* 1分频 */
    g_timx_cplm_pwm_handle.Init.RepetitionCounter = 0;																/* 重复计数器寄存器为0 */
    g_timx_cplm_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;		/* 使能影子寄存器TIMx_ARR */
    HAL_TIM_Base_Init(&g_timx_cplm_pwm_handle);																				/* 初始化定时器时基 */
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; 												/* 使用内部时钟 */
    HAL_TIM_ConfigClockSource(&g_timx_cplm_pwm_handle, &sClockSourceConfig);					/* 时钟源配置 */
    HAL_TIM_PWM_Init(&g_timx_cplm_pwm_handle) ;																				/* PWM模式初始化 */
    /* 定时器主从模式配置，这里我们没有用到主从模式 */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_cplm_pwm_handle, &sMasterConfig);
    
    sConfigOC.OCMode = TIM_OCMODE_PWM1;																											/* PWM模式1 */
    sConfigOC.Pulse = 300;																																	/* 设置通道3的占空比为30% */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; 																							/* 通道3输出极性为低 */ 
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;																						/*互补输出极性为低 */ 
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;																							/* 不使用快速模式 */
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET; 																					/* 设置通道3的空闲状态下为低电平 */
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;																					/* 设置互补输出通道的空闲状态下为高电平 */
    HAL_TIM_PWM_ConfigChannel(&g_timx_cplm_pwm_handle, &sConfigOC, ATIM_TIMX_CPLM_CHY_CCRY);/* 初始化定时器的通道3 PWM输出 */
    
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;																/* 运行模式下“关闭状态”选择，这里选择Disable，即关闭 */
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;																/* 即空闲模式下“关闭状态”选择，这里选择Disable，即关闭 */
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;																			/* 锁定设置选择Off，即关闭 */
    sBreakDeadTimeConfig.DeadTime = 100; 																										/* 配置死区延时时间，这里配置死区发生器为十进制数的100 */
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE; 																		/* 用于配置刹车状态，这里配置为Enable，即打开刹车 */
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW; 														/* 用于配置刹车极性，这里配置为Low，即刹车电平为低 */
    sBreakDeadTimeConfig.BreakFilter = 0;																										/* 不使用滤波 */
		 /* 没有使用到刹车输入2，这里处于关闭状态 */
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_LOW;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE; 										  /* 用于配置自动输出状态，这里选择使能Enable */
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle, &sBreakDeadTimeConfig);					/* 配置刹车功能 */

    HAL_TIM_PWM_Start(&g_timx_cplm_pwm_handle, ATIM_TIMX_CPLM_CHY_CCRY);										/* 配置通道3开始生成PWM信号 */
    HAL_TIMEx_PWMN_Start(&g_timx_cplm_pwm_handle,ATIM_TIMX_CPLM_CHY_CCRY);									/* 在互补输出上开始PWM信号生成 */
}

/**
 * @brief       定时器互补输出模式通道引脚初始化函数
 * @param       无
 * @note        此函数会被atim_timx_cplm_pwm_init()函数调用
 * @retval      无
 */
void atim_timx_cplm_pwm_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE();   	/* 通道X对应IO口时钟使能 */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE();  	/* 通道X互补通道对应IO口时钟使能 */
    ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE();  	/* 通道X刹车输入对应IO口时钟使能 */
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHY_GPIO_PIN;		/* 指定引脚为12 */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP; 							/* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP; 									/* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH ;				/* 速度等级为高 */
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHY_GPIO_AF;					/* 复用选择为AF1 */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHY_GPIO_PORT, &gpio_init_struct);		/* 初始化GPIOE */
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHYN_GPIO_PIN;							/* 指定引脚为13 */
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHYN_GPIO_AF;					/* 复用选择为AF1 */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, &gpio_init_struct);	/* 初始化GPIOE */
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_BKIN_GPIO_PIN;							/* 指定引脚为6 */
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_BKIN_GPIO_AF;					/* 复用选择为AF1 */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, &gpio_init_struct);	/* 初始化GPIOE */
}








