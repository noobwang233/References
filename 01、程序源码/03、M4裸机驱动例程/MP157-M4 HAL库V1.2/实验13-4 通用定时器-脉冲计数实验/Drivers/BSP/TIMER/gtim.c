/**
 ****************************************************************************************************
 * @file        gtim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-06
 * @brief       通用定时器 驱动代码
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

#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"

/*********************************通用定时器脉冲计数实验程序*************************************/

TIM_HandleTypeDef g_timx_cnt_chy_handler;      /* 定时器x句柄 */


/* 记录定时器计数器的溢出次数, 方便计算总脉冲个数 */
uint32_t g_timxchy_cnt_ofcnt = 0 ;  					/* 计数溢出次数 */


/**
 * @brief       通用定时器TIMX 通道Y 脉冲计数 初始化函数
 * @note
 *              本函数选择通用定时器的时钟选择: 外部时钟源模式1(SMS[2:0] = 111)
 *              这样CNT的计数时钟源就来自 TIMX_CH1/CH2, 可以实现外部脉冲计数(脉冲接入CH1/CH2)
 *
 *              时钟分频数 = psc, 一般设置为0, 表示每一个时钟都会计数一次, 以提高精度.
 *              通过读取CNT和溢出次数, 经过简单计算, 可以得到当前的计数值, 从而实现脉冲计数
 *
 * @param       arr: 自动重装值 
 * @retval      无
 */
void gtim_timx_cnt_chy_init(uint16_t psc)
{
	 g_timx_cnt_chy_handler.Instance = GTIM_TIMX_CNT;               				/* 定时器x */
    g_timx_cnt_chy_handler.Init.Prescaler = psc;                    			/* 定时器分频 */
    g_timx_cnt_chy_handler.Init.CounterMode = TIM_COUNTERMODE_UP;					/*向上计数模式 */
    g_timx_cnt_chy_handler.Init.Period = 65535;                     			/* 自动重装载值 */
    g_timx_cnt_chy_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;		/* 分频 */
    HAL_TIM_IC_Init(&g_timx_cnt_chy_handler);
}



/**
 * @brief       通用定时器输入捕获初始化接口
                 HAL库调用的接口，用于配置不同的输入捕获
 * @param       htim:定时器句柄
 * @note        此函数会被HAL_TIM_IC_Init()调用
 * @retval      无
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_CNT)              						/*输入计数捕获*/
    {
        GPIO_InitTypeDef gpio_init_struct;
        TIM_SlaveConfigTypeDef  tim_slave_config_handle = {0};
        TIM_IC_InitTypeDef timx_ic_cnt_chy_handler = {0};
        GTIM_TIMX_CNT_CHY_CLK_ENABLE();                						/* 使能TIMx时钟 */
        GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE();          						/* 开启GPIOA时钟 */

        gpio_init_struct.Pin = GTIM_TIMX_CNT_CHY_GPIO_PIN;  			/* 输入捕获的GPIO口 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;              		/* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLDOWN;                		/* 下拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;				/* 高速 */
        gpio_init_struct.Alternate = GTIM_TIMX_CNT_CHY_GPIO_AF;		/* 复用为捕获通道 */
        HAL_GPIO_Init(GTIM_TIMX_CNT_CHY_GPIO_PORT, &gpio_init_struct);

        tim_slave_config_handle.SlaveMode = TIM_SLAVEMODE_EXTERNAL1; 							/* 从模式：外部触发模式1 */
        tim_slave_config_handle.InputTrigger = TIM_TS_TI1FP1; 										/* 输入触发：选择 TI1FP1(TIMX_CH1) 作为输入源 */
        tim_slave_config_handle.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;  		/* 触发极性：上升沿 */ 
        tim_slave_config_handle.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;   	/* 触发预分频：无 */
        tim_slave_config_handle.TriggerFilter = 0x0; 															/* 滤波：本例中不需要任何滤波 */
        HAL_TIM_SlaveConfigSynchro(&g_timx_cnt_chy_handler,&tim_slave_config_handle);

        /* 配置输入捕获模式 */
        timx_ic_cnt_chy_handler.ICPolarity = TIM_ICPOLARITY_RISING;							/* 上升沿捕获 */
        timx_ic_cnt_chy_handler.ICSelection = TIM_ICSELECTION_DIRECTTI;					/* 映射到TI1上 */
        timx_ic_cnt_chy_handler.ICPrescaler = TIM_ICPSC_DIV1;										/*配置输入不分频*/
        timx_ic_cnt_chy_handler.ICFilter = 0;    																/* 配置输入滤波器，不滤波 */
        HAL_TIM_IC_ConfigChannel(&g_timx_cnt_chy_handler, &timx_ic_cnt_chy_handler, GTIM_TIMX_CNT_CHY);		/* 配置TIMx通道y */

        HAL_TIM_IC_Start(&g_timx_cnt_chy_handler, GTIM_TIMX_CNT_CHY);  	/* 开始捕获TIMx的通道y */      
        __HAL_TIM_ENABLE_IT(&g_timx_cnt_chy_handler,TIM_IT_UPDATE);			/*使能更新中断*/

        HAL_NVIC_SetPriority(GTIM_TIMX_CNT_IRQn,1,3);  									/* 设置中断优先级，抢占优先级1，子优先级3 */  
        HAL_NVIC_EnableIRQ(GTIM_TIMX_CNT_IRQn);          								/* 开启ITMx中断 */
    }
}



/**
 * @brief       通用定时器TIMX 脉冲计数 更新中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_CNT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_cnt_chy_handler);	/* 定时器共用处理函数 */
}



/**
 * @brief       定时器更新中断回调函数
 * @param        htim:定时器句柄指针
 * @note        此函数会被定时器中断函数共同调用的
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_timx_cnt_chy_handler))
    {
        g_timxchy_cnt_ofcnt++;          			/* 累计溢出次数 */
    }
}



/**
 * @brief       通用定时器TIMX 通道Y 获取当前计数值 
 * @param       无
 * @retval      当前计数值
 */
uint32_t gtim_timx_cnt_chy_get_count(void)
{
    uint32_t count = 0;
    count = g_timxchy_cnt_ofcnt * 65536;														/* 计算溢出次数对应的计数值 */
    count += __HAL_TIM_GET_COUNTER(&g_timx_cnt_chy_handler);        /* 加上当前CNT的值 */
    return count;
}



/**
 * @brief       通用定时器TIMX 通道Y 重启计数器
 * @param       无
 * @retval      当前计数值
 */
void gtim_timx_cnt_chy_restart(void)
{
    __HAL_TIM_DISABLE(&g_timx_cnt_chy_handler);           /* 关闭定时器TIMX */
    g_timxchy_cnt_ofcnt = 0;                              /* 累加器清零 */
    __HAL_TIM_SET_COUNTER(&g_timx_cnt_chy_handler, 0);    /* 计数器清零 */
    __HAL_TIM_ENABLE(&g_timx_cnt_chy_handler);            /* 使能定时器TIMX */
}















