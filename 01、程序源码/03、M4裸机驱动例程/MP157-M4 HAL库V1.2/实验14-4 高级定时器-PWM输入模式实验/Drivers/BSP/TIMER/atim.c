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

/*******************************高级定时器PWM输入模式程序**************************************/

TIM_HandleTypeDef g_timx_pwmin_chy_handle;                              /* 定时器x句柄 */

/* PWM输入状态(g_timxchy_cap_sta)
 * 0,没有成功捕获.
 * 1,已经成功捕获了
 */
uint8_t g_timxchy_pwmin_sta  = 0;   /* PWM输入状态 */
uint16_t g_timxchy_pwmin_psc  = 0;  /* PWM输入分频系数 */
uint32_t g_timxchy_pwmin_hval = 0 ; /* PWM的高电平脉宽 */
uint32_t g_timxchy_pwmin_cval = 0 ; /* PWM的周期宽度 */

/**
 * @brief       定时器TIMX 通道Y PWM输入模式 初始化函数
 * @note
 *              通用定时器的时钟来自APB2,当APB2DIV≥2分频的时候
 *              通用定时器的时钟为APB2时钟的2倍, 而APB2为104.5M, 所以定时器时钟 = 209Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 *              本函数初始化的时候: 使用psc=0, arr固定为65535. 得到采样时钟频率为240Mhz,精度4.485ns
 *
 * @param       无
 * @retval      无
 */
void atim_timx_pwmin_chy_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};


    atim_timx_pwmin_chy_gpio_init();		/* 定时器相关引脚初始化 */
    ATIM_TIMX_PWMIN_CHY_CLK_ENABLE();		/* 使能TIM8时钟 */
    
    g_timx_pwmin_chy_handle.Instance = ATIM_TIMX_PWMIN;																	/* 定时器8 */ 
    g_timx_pwmin_chy_handle.Init.Prescaler = 0;																					/* 定时器预分频系数为0，不分频 */
    g_timx_pwmin_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;											/* 向上计数模式 */
    g_timx_pwmin_chy_handle.Init.Period = 65535;																				/* 自动重装载值 */
    g_timx_pwmin_chy_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;								/* 内部时钟不分频*/
    g_timx_pwmin_chy_handle.Init.RepetitionCounter = 0;																	/* 重复计数为0 */
    g_timx_pwmin_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; 		/* 自动重载预装载使能 */
    HAL_TIM_Base_Init(&g_timx_pwmin_chy_handle);																				/* 时基初始化 */
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;													/* 内部时钟 */
    HAL_TIM_ConfigClockSource(&g_timx_pwmin_chy_handle, &sClockSourceConfig);						/* 时钟初始化 */
    
    HAL_TIM_IC_Init(&g_timx_pwmin_chy_handle);														/* 捕获通道初始化 */

    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;													/* 复位模式 */
    sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;														/* 触发源为TI1FP1 */
    sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;				/* 触发模式：上升沿触发 */
    sSlaveConfig.TriggerFilter = 0;																				/* 不滤波 */
    HAL_TIM_SlaveConfigSynchro(&g_timx_pwmin_chy_handle,&sSlaveConfig);		/* 从模式配置 */
		
		/* 定时器触发输出，TIMx_EGR寄存器中的UG位用作触发输出(TRGO) */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
		/* 主定时器的从模式失能 */
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_pwmin_chy_handle, &sMasterConfig);	/* 配置主模式 */
    
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;														/* 上升沿检测 */
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;																	/*选择输入端 IC1映射到TI1上*/
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;																						/* 不分频 */
    sConfigIC.ICFilter = 0;																														/* 不滤波 */
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &sConfigIC, TIM_CHANNEL_1);		/* 捕获通道1配置 */
    
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;													/* 下降沿检测 */
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;																/*选择输入端 IC2映射到TI1上*/
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &sConfigIC, TIM_CHANNEL_2);		/* 捕获通道2配置 */
    
    HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_IRQn,1,3);           												/* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(ATIM_TIMX_PWMIN_IRQn);                 												/* 开启ITMx中断 */
    
    /* TIM1/TIM8 有独立的输入捕获中断服务函数 */
    if (ATIM_TIMX_PWMIN == TIM1 || ATIM_TIMX_PWMIN == TIM8)
    {
        HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_CC_IRQn,1,3);    				/* 设置中断优先级，抢占优先级1，子优先级3 */
        HAL_NVIC_EnableIRQ(ATIM_TIMX_PWMIN_CC_IRQn);          				/* 开启ITMx中断 */
    }
    
    HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle,TIM_CHANNEL_1); 			/* 启动TIM8通道1输入捕获模式 */
    HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle,TIM_CHANNEL_2);			/* 启动TIM8通道2输入捕获模式 */
    __HAL_TIM_ENABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_UPDATE);     /* 使能更新中断 */
}

/**
 * @brief       定时器PWM输入模式通道引脚初始化函数
 * @param       无
 * @note        此函数会被atim_timx_cplm_pwm_init()函数调用
 * @retval      无
 */
void atim_timx_pwmin_chy_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE();   												/* TIM8通道IO口（PI0）时钟使能 */
  
    gpio_init_struct.Pin = ATIM_TIMX_PWMIN_CHY_GPIO_PIN; 						/* 引脚5 */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;  											/* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLDOWN;													/* 下拉模式 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH ;						/* 高速模式 */
    gpio_init_struct.Alternate = ATIM_TIMX_PWMIN_CHY_GPIO_AF;				/* AF功能选择 */ 
    HAL_GPIO_Init(ATIM_TIMX_PWMIN_CHY_GPIO_PORT, &gpio_init_struct);/* GPIO引脚初始化 */
}

/**
 * @brief       定时器TIMX 更新/溢出 中断服务函数
 *   @note      TIM1/TIM8的这个函数仅用于更新/溢出中断服务,捕获在另外一个函数!
 *              其他普通定时器则更新/溢出/捕获,都在这个函数里面处理!
 * @param       无
 * @retval      无
 */
void ATIM_TIMX_PWMIN_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);/* 定时器共用处理函数 */
}

/**
 * @brief       定时器TIMX 输入捕获 中断服务函数
 *   @note      仅TIM1/TIM8有这个函数,其他普通定时器没有这个中断服务函数!
 * @param       无
 * @retval      无
 */
void ATIM_TIMX_PWMIN_CC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);/* 定时器共用处理函数 */
}

/**
 * @brief       定时器TIM8 PWM输入模式 重新启动捕获
 * @param       无
 * @retval      无
 */
void atim_timx_pwmin_chy_restart(void)
{
    INTX_DISABLE();                       /* 关闭中断 */
		//TIM8->DIER=0x0000;									/* 关闭中断 */
    g_timxchy_pwmin_sta = 0;              /* 清零状态,重新开始检测 */
    g_timxchy_pwmin_hval=0;
    g_timxchy_pwmin_cval=0;

    INTX_ENABLE();                      	/* 打开中断 */
		//TIM8->DIER=0x07;										/* 打开中断 */
}

/**
 * @brief       定时器输入捕获中断处理回调函数
 * @param       htim:定时器句柄指针
 * @note        该函数在HAL_TIM_IRQHandler中会被调用
 *              此函数是定时器共同调用的回调函数，为了防止重定义错误，
 *              我们把gtim.c的HAL_TIM_IC_CaptureCallback()函数先屏蔽掉.
 * @retval      无
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    
    if (g_timxchy_pwmin_sta == 0)   				/* 还没有成功捕获 */
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {  
            g_timxchy_pwmin_hval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle,TIM_CHANNEL_2)+2; /* 修正系数为2, 加2 */
            g_timxchy_pwmin_cval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle,TIM_CHANNEL_1)+2; /* 修正系数为2, 加2 */
            g_timxchy_pwmin_sta = 1;        /* 标记捕获成功 */
        }
    }
}







