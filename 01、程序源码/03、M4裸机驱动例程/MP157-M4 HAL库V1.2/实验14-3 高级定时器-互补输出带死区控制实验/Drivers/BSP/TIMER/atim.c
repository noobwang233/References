/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-06
 * @brief       �߼���ʱ�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32MP1������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200506
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"

/*******************************����������������Ƴ���**************************************/

TIM_HandleTypeDef g_timx_cplm_pwm_handle;                              /* ��ʱ��x��� */

/**
 * @brief       �߼���ʱ��TIMX ������� ��ʼ��������ʹ��PWMģʽ1��
 * @note
 *              ���ø߼���ʱ��TIMX �������, һ·OC3 һ·OC3N, ���ҿ�����������ʱ��
 *              �߼���ʱ����ʱ������APB2,��APB2DIV��2��Ƶ��ʱ��
 *              �߼���ʱ����ʱ��ΪAPB2ʱ�ӵ�2��, ��APB2Ϊ104.5M, ���Զ�ʱ��ʱ�� = 209Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void atim_timx_cplm_pwm_init(uint16_t arr, uint16_t psc)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    atim_timx_cplm_pwm_gpio_init();			/* GPIO��ʼ�� */
    ATIM_TIMX_CPLM_CLK_ENABLE();				/* TIM1 ʱ��ʹ�� */

    g_timx_cplm_pwm_handle.Instance = TIM1;																						/* ��ʱ��1 */
    g_timx_cplm_pwm_handle.Init.Prescaler = psc;																			/* ��ʱ����Ƶ */
    g_timx_cplm_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;											/* ���ϼ���ģʽ*/
    g_timx_cplm_pwm_handle.Init.Period = arr;																					/* �Զ���װ��ֵ */
    g_timx_cplm_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;								/* 1��Ƶ */
    g_timx_cplm_pwm_handle.Init.RepetitionCounter = 0;																/* �ظ��������Ĵ���Ϊ0 */
    g_timx_cplm_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;		/* ʹ��Ӱ�ӼĴ���TIMx_ARR */
    HAL_TIM_Base_Init(&g_timx_cplm_pwm_handle);																				/* ��ʼ����ʱ��ʱ�� */
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; 												/* ʹ���ڲ�ʱ�� */
    HAL_TIM_ConfigClockSource(&g_timx_cplm_pwm_handle, &sClockSourceConfig);					/* ʱ��Դ���� */
    HAL_TIM_PWM_Init(&g_timx_cplm_pwm_handle) ;																				/* PWMģʽ��ʼ�� */
    /* ��ʱ������ģʽ���ã���������û���õ�����ģʽ */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_cplm_pwm_handle, &sMasterConfig);
    
    sConfigOC.OCMode = TIM_OCMODE_PWM1;																											/* PWMģʽ1 */
    sConfigOC.Pulse = 300;																																	/* ����ͨ��3��ռ�ձ�Ϊ30% */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; 																							/* ͨ��3�������Ϊ�� */ 
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;																						/*�����������Ϊ�� */ 
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;																							/* ��ʹ�ÿ���ģʽ */
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET; 																					/* ����ͨ��3�Ŀ���״̬��Ϊ�͵�ƽ */
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_SET;																					/* ���û������ͨ���Ŀ���״̬��Ϊ�ߵ�ƽ */
    HAL_TIM_PWM_ConfigChannel(&g_timx_cplm_pwm_handle, &sConfigOC, ATIM_TIMX_CPLM_CHY_CCRY);/* ��ʼ����ʱ����ͨ��3 PWM��� */
    
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;																/* ����ģʽ�¡��ر�״̬��ѡ������ѡ��Disable�����ر� */
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;																/* ������ģʽ�¡��ر�״̬��ѡ������ѡ��Disable�����ر� */
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;																			/* ��������ѡ��Off�����ر� */
    sBreakDeadTimeConfig.DeadTime = 100; 																										/* ����������ʱʱ�䣬������������������Ϊʮ��������100 */
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE; 																		/* ��������ɲ��״̬����������ΪEnable������ɲ�� */
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW; 														/* ��������ɲ�����ԣ���������ΪLow����ɲ����ƽΪ�� */
    sBreakDeadTimeConfig.BreakFilter = 0;																										/* ��ʹ���˲� */
		 /* û��ʹ�õ�ɲ������2�����ﴦ�ڹر�״̬ */
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_LOW;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE; 										  /* ���������Զ����״̬������ѡ��ʹ��Enable */
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_cplm_pwm_handle, &sBreakDeadTimeConfig);					/* ����ɲ������ */

    HAL_TIM_PWM_Start(&g_timx_cplm_pwm_handle, ATIM_TIMX_CPLM_CHY_CCRY);										/* ����ͨ��3��ʼ����PWM�ź� */
    HAL_TIMEx_PWMN_Start(&g_timx_cplm_pwm_handle,ATIM_TIMX_CPLM_CHY_CCRY);									/* �ڻ�������Ͽ�ʼPWM�ź����� */
}

/**
 * @brief       ��ʱ���������ģʽͨ�����ų�ʼ������
 * @param       ��
 * @note        �˺����ᱻatim_timx_cplm_pwm_init()��������
 * @retval      ��
 */
void atim_timx_cplm_pwm_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    ATIM_TIMX_CPLM_CHY_GPIO_CLK_ENABLE();   	/* ͨ��X��ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_CPLM_CHYN_GPIO_CLK_ENABLE();  	/* ͨ��X����ͨ����ӦIO��ʱ��ʹ�� */
    ATIM_TIMX_CPLM_BKIN_GPIO_CLK_ENABLE();  	/* ͨ��Xɲ�������ӦIO��ʱ��ʹ�� */
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHY_GPIO_PIN;		/* ָ������Ϊ12 */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP; 							/* ����������� */
    gpio_init_struct.Pull = GPIO_PULLUP; 									/* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH ;				/* �ٶȵȼ�Ϊ�� */
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHY_GPIO_AF;					/* ����ѡ��ΪAF1 */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHY_GPIO_PORT, &gpio_init_struct);		/* ��ʼ��GPIOE */
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_CHYN_GPIO_PIN;							/* ָ������Ϊ13 */
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_CHYN_GPIO_AF;					/* ����ѡ��ΪAF1 */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_CHYN_GPIO_PORT, &gpio_init_struct);	/* ��ʼ��GPIOE */
  
    gpio_init_struct.Pin = ATIM_TIMX_CPLM_BKIN_GPIO_PIN;							/* ָ������Ϊ6 */
    gpio_init_struct.Alternate = ATIM_TIMX_CPLM_BKIN_GPIO_AF;					/* ����ѡ��ΪAF1 */
    HAL_GPIO_Init(ATIM_TIMX_CPLM_BKIN_GPIO_PORT, &gpio_init_struct);	/* ��ʼ��GPIOE */
}








