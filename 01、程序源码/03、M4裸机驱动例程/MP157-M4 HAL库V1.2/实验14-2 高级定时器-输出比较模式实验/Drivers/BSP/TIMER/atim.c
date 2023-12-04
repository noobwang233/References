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

/*********************************  高级定时器输出比较程序*************************************/

TIM_HandleTypeDef g_timx_comp_pwm_handle;                              /* 定时器x句柄 */

/**
 * @brief       高级定时器TIMX 输出比较模式 初始化函数（使用输出比较模式）
 * @note
 *              配置高级定时器TIMX 2路输出比较模式PWM输出,实现50%占空比,不同相位控制
 *              注意,本例程输出比较模式,每2个计数周期才能完成一个PWM输出,因此输出频率减半
 *              另外,我们还可以开启中断在中断里面修改CCRx,从而实现不同频率/不同相位的控制
 *              但是我们不推荐这么使用,因为这可能导致非常频繁的中断,从而占用大量CPU资源
 *
 *              高级定时器的时钟来自APB2,当APB2DIV≥2分频的时候
 *              高级定时器的时钟为APB2时钟的2倍, 而APB2为104.5M, 所以定时器时钟 = 209Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    atim_timx_comp_pwm_gpio_init();
    ATIM_TIMX_COMP_CLK_ENABLE();

    g_timx_comp_pwm_handle.Instance = ATIM_TIMX_COMP;                       					/* 定时器1 */
    g_timx_comp_pwm_handle.Init.Prescaler = psc  ;                          					/* 定时器分频 */
    g_timx_comp_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;           					/* 向上计数模式 */
    g_timx_comp_pwm_handle.Init.Period = arr;                               					/* 自动重装载值 */
    g_timx_comp_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;     					/* 不分频 */
    g_timx_comp_pwm_handle.Init.RepetitionCounter = 0;																/* 重复计数器寄存器为0 */
    g_timx_comp_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;		/* 不使能影子寄存器TIMx_ARR */
    HAL_TIM_Base_Init(&g_timx_comp_pwm_handle);																				/* 初始化定时器时基 */

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;												/* 使用内部时钟 */
    HAL_TIM_ConfigClockSource(&g_timx_comp_pwm_handle, &sClockSourceConfig);					/* 时钟源配置 */

    HAL_TIM_OC_Init(&g_timx_comp_pwm_handle);                               					/* 输出比较模式初始化 */
		/* 定时器主从模式配置，这里我们没有用到主从模式 */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_comp_pwm_handle, &sMasterConfig);
		/* 定时器通道3和通道4的配置 */
    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;        					/* 输出比较模式翻转功能 */
    sConfigOC.Pulse = 500-1;															/* 设置通道3输出比较寄存器的值为500-1 */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;						/* 输出比较极性为高 */ 
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;					/* 互补输出比较极性位高 */
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;						/* 失能输出比较快速模式 */
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;				/* 选择空闲状态下非工作状态 */
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;			/* 设置空闲状态下非工作状态 */
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_3);	/* 初始化定时器的输出比较通道3 */
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_3);					/* 通道3预装载使能 */

    sConfigOC.Pulse = 250-1;																											/* 设置通道4输出比较寄存器的值为250-1 */
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_4);	/* 初始化定时器的输出比较通道4 */
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_4);					/* 通道4预装载使能 */

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE; 											/* 设置运行模式下非工作状态选项 */
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE; 										/* 设置在空载下非工作状态选项 */
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;														/* 锁电平参数 */
    sBreakDeadTimeConfig.DeadTime = 0;																						/* 死区时间设置为0 */
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE; 													/* 失能TIMx刹车输入 */
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;									/* TIM1刹车输入管脚极性为高电平有效 */
    sBreakDeadTimeConfig.BreakFilter = 0;																					/* TIM1刹车输入滤波为0 */
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE; 												/* TIM1刹车输入BRK2失能 */
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH; 								/* 刹车输入BRK2高电平有效 */
    sBreakDeadTimeConfig.Break2Filter = 0; 																				/* TIM1刹车输入2滤波为0 */
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE; 					/* 失能自动输出功能，只能后期手动通过软件设置MOE */
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_comp_pwm_handle, &sBreakDeadTimeConfig);/* 配置间隔功能，停滞时间，锁定级别 */
}

/**
 * @brief       定时器输出比较模式通道引脚初始化函数
 * @param       无
 * @note        此函数会被atim_timx_comp_pwm_init()函数调用
 * @retval      无
 */
void atim_timx_comp_pwm_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE();														/* 使能GPIOE时钟 */
    ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH3_GPIO_PIN;							/* PE13 */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;												/* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;														/* 无上/下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;									/* 速度等级为高 */
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH3_GPIO_AF;				/* 复用选择为AF1 */
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH3_GPIO_PORT, &gpio_init_struct);	/* 初始化GPIOE */

    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH4_GPIO_PIN;							/* PE14 */
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH4_GPIO_AF;				/* 复用选择为AF1 */
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH4_GPIO_PORT, &gpio_init_struct);	/* 初始化GPIOE */
}









