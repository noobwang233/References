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

/*********************************通用定时器输入捕获实验程序*************************************/

TIM_HandleTypeDef g_timx_cap_chy_handler;      /* 定时器x句柄 */
TIM_IC_InitTypeDef g_timx_ic_cap_chy_handler;


/**
 * @brief       通用定时器TIMX 通道Y 输入捕获 初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当APB1DIV≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为104.5M, 所以定时器时钟 = 209Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc)
{
		g_timx_cap_chy_handler.Instance = GTIM_TIMX_CAP;             					/* 定时器5 */
		g_timx_cap_chy_handler.Init.Prescaler = psc;                  				/* 定时器分频 */
		g_timx_cap_chy_handler.Init.CounterMode = TIM_COUNTERMODE_UP;					/* 向上计数模式*/
		g_timx_cap_chy_handler.Init.Period = arr;                     				/* 自动重装载值 */
		g_timx_cap_chy_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;		/* 分频 */
		HAL_TIM_IC_Init(&g_timx_cap_chy_handler);
}


/**
 * @brief       通用定时器输入捕获引脚初始化函数
 * @param       htim:定时器句柄
 * @note        此函数会被HAL_TIM_IC_Init()调用
 * @retval      无
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_CAP)                       					/* 输入通道捕获 */
    {
        GPIO_InitTypeDef gpio_init_struct;
        GTIM_TIMX_CAP_CHY_CLK_ENABLE();                         				/* 使能TIMx时钟 */
        GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();                   					/* 开启捕获IO的时钟 */

        gpio_init_struct.Pin = GTIM_TIMX_CAP_CHY_GPIO_PIN;  						/* 输入捕获的GPIO口 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;              					/* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLDOWN;                					/* 下拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;							/* 高速 */
        gpio_init_struct.Alternate = GTIM_TIMX_CAP_CHY_GPIO_AF; 				/* 复用为捕获TIM5的通道1 */
        HAL_GPIO_Init(GTIM_TIMX_CAP_CHY_GPIO_PORT, &gpio_init_struct);
			
        g_timx_ic_cap_chy_handler.ICPolarity= TIM_ICPOLARITY_RISING;			/*上升沿捕获*/
        g_timx_ic_cap_chy_handler.ICSelection = TIM_ICSELECTION_DIRECTTI; /* 映射到TI1上 */
        g_timx_ic_cap_chy_handler.ICPrescaler = TIM_ICPSC_DIV1;						/* 配置输入分频 */
        g_timx_ic_cap_chy_handler.ICFilter = 0;        										/* 配置输入滤波器，不滤波 */
        HAL_TIM_IC_ConfigChannel(&g_timx_cap_chy_handler, &g_timx_ic_cap_chy_handler, GTIM_TIMX_CAP_CHY);/* 配置TIM5通道1 */
			
        HAL_TIM_IC_Start_IT(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY);	/*启动*/
        __HAL_TIM_ENABLE_IT(&g_timx_cap_chy_handler, TIM_IT_UPDATE);			/* 使能中断 */

        HAL_NVIC_SetPriority(GTIM_TIMX_CAP_IRQn, 1, 3);  									/* 抢占1，子优先级3 */
        HAL_NVIC_EnableIRQ(GTIM_TIMX_CAP_IRQn);            								/* 开启ITMx中断 */
    }
}


/* 输入捕获状态(g_timxchy_cap_sta)
 * [7]  :0,没有成功的捕获;1,成功捕获到一次.
 * [6]  :0,还没捕获到高电平;1,已经捕获到高电平了.
 * [5:0]:捕获高电平后溢出的次数,最多溢出63次,所以最长捕获值 = 63*65536 + 65535 = 4194303
 *       注意:为了通用,我们默认ARR和CCRy都是16位寄存器,对于32位的定时器(如:TIM5),也只按16位使用
 *       按1us的计数频率,最长溢出时间为:4194303 us, 约4.19秒
 *
 *      (说明一下：正常32位定时器来说,1us计数器加1,溢出时间:4294秒)
 */

uint8_t g_timxchy_cap_sta = 0;  /* 输入捕获状态 */
uint16_t g_timxchy_cap_val =0 ; /* 输入捕获值 */

/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_CAP_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_cap_chy_handler);			/* 定时器共用处理函数 */
}



/**
 * @brief       定时器输入捕获中断处理回调函数
 * @param       htim:定时器句柄指针
 * @note        该函数在HAL_TIM_IRQHandler中会被调用
 * @retval      无
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if ((g_timxchy_cap_sta & 0X80) == 0)         			/* 还未成功捕获 */
    {
        if (g_timxchy_cap_sta & 0X40)            			/* 捕获到一个下降沿 */
        {
            g_timxchy_cap_sta |= 0X80;           			/* 标记成功捕获到一次高电平脉宽 */
            g_timxchy_cap_val = HAL_TIM_ReadCapturedValue(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY); 			/* 获取当前的捕获值 */
            TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY);                     			/* 一定要先清除原来的设置 */
            TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);			/* 配置TIM5通道1上升沿捕获 */
        }
        else                                     			/* 还未开始,第一次捕获上升沿 */
        {
            g_timxchy_cap_sta = 0;               			/* 清空 */
            g_timxchy_cap_val = 0;
            g_timxchy_cap_sta |= 0X40;           			/* 标记捕获到了上升沿 */
            __HAL_TIM_DISABLE(&g_timx_cap_chy_handler);  														/* 关闭定时器5 */
            __HAL_TIM_SET_COUNTER(&g_timx_cap_chy_handler,0);
            TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY);   /* 一定要先清除原来的设置！！ */
            TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING);/* 定时器5通道1设置为下降沿捕获 */
            __HAL_TIM_ENABLE(&g_timx_cap_chy_handler);   															/* 使能定时器5 */
        }
    }
}



/**
 * @brief       定时器更新中断回调函数
 * @param        htim:定时器句柄指针
 * @note        此函数会被定时器中断函数共同调用的
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_timx_cap_chy_handler))
    {
        if ((g_timxchy_cap_sta & 0X80) == 0)               /* 还未成功捕获 */
        {
            if (g_timxchy_cap_sta & 0X40)                  /* 已经捕获到高电平了 */
            {
                if ((g_timxchy_cap_sta & 0X3F) == 0X3F)    /* 高电平太长了 */
                {
										g_timxchy_cap_val = HAL_TIM_ReadCapturedValue (&g_timx_cap_chy_handler, TIM_CHANNEL_1);     /* 获取当前的捕获??? */
										TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY);									    /* 一定要先清除原来的设置 */
										TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING); /* 定时器5通道1设置为下降沿捕获 */
										g_timxchy_cap_sta |= 0X80;             /* 标记成功捕获了一次 */
										g_timxchy_cap_val = 0XFFFF;
                }
                else
                {
                    g_timxchy_cap_sta++;
                }
            }
        }
    }
}









