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

/*******************************�߼���ʱ��PWM����ģʽ����**************************************/

TIM_HandleTypeDef g_timx_pwmin_chy_handle;                              /* ��ʱ��x��� */

/* PWM����״̬(g_timxchy_cap_sta)
 * 0,û�гɹ�����.
 * 1,�Ѿ��ɹ�������
 */
uint8_t g_timxchy_pwmin_sta  = 0;   /* PWM����״̬ */
uint16_t g_timxchy_pwmin_psc  = 0;  /* PWM�����Ƶϵ�� */
uint32_t g_timxchy_pwmin_hval = 0 ; /* PWM�ĸߵ�ƽ���� */
uint32_t g_timxchy_pwmin_cval = 0 ; /* PWM�����ڿ�� */

/**
 * @brief       ��ʱ��TIMX ͨ��Y PWM����ģʽ ��ʼ������
 * @note
 *              ͨ�ö�ʱ����ʱ������APB2,��APB2DIV��2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB2ʱ�ӵ�2��, ��APB2Ϊ104.5M, ���Զ�ʱ��ʱ�� = 209Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 *              ��������ʼ����ʱ��: ʹ��psc=0, arr�̶�Ϊ65535. �õ�����ʱ��Ƶ��Ϊ240Mhz,����4.485ns
 *
 * @param       ��
 * @retval      ��
 */
void atim_timx_pwmin_chy_init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};


    atim_timx_pwmin_chy_gpio_init();		/* ��ʱ��������ų�ʼ�� */
    ATIM_TIMX_PWMIN_CHY_CLK_ENABLE();		/* ʹ��TIM8ʱ�� */
    
    g_timx_pwmin_chy_handle.Instance = ATIM_TIMX_PWMIN;																	/* ��ʱ��8 */ 
    g_timx_pwmin_chy_handle.Init.Prescaler = 0;																					/* ��ʱ��Ԥ��Ƶϵ��Ϊ0������Ƶ */
    g_timx_pwmin_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;											/* ���ϼ���ģʽ */
    g_timx_pwmin_chy_handle.Init.Period = 65535;																				/* �Զ���װ��ֵ */
    g_timx_pwmin_chy_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;								/* �ڲ�ʱ�Ӳ���Ƶ*/
    g_timx_pwmin_chy_handle.Init.RepetitionCounter = 0;																	/* �ظ�����Ϊ0 */
    g_timx_pwmin_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; 		/* �Զ�����Ԥװ��ʹ�� */
    HAL_TIM_Base_Init(&g_timx_pwmin_chy_handle);																				/* ʱ����ʼ�� */
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;													/* �ڲ�ʱ�� */
    HAL_TIM_ConfigClockSource(&g_timx_pwmin_chy_handle, &sClockSourceConfig);						/* ʱ�ӳ�ʼ�� */
    
    HAL_TIM_IC_Init(&g_timx_pwmin_chy_handle);														/* ����ͨ����ʼ�� */

    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;													/* ��λģʽ */
    sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;														/* ����ԴΪTI1FP1 */
    sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;				/* ����ģʽ�������ش��� */
    sSlaveConfig.TriggerFilter = 0;																				/* ���˲� */
    HAL_TIM_SlaveConfigSynchro(&g_timx_pwmin_chy_handle,&sSlaveConfig);		/* ��ģʽ���� */
		
		/* ��ʱ�����������TIMx_EGR�Ĵ����е�UGλ�����������(TRGO) */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
		/* ����ʱ���Ĵ�ģʽʧ�� */
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_pwmin_chy_handle, &sMasterConfig);	/* ������ģʽ */
    
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;														/* �����ؼ�� */
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;																	/*ѡ������� IC1ӳ�䵽TI1��*/
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;																						/* ����Ƶ */
    sConfigIC.ICFilter = 0;																														/* ���˲� */
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &sConfigIC, TIM_CHANNEL_1);		/* ����ͨ��1���� */
    
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;													/* �½��ؼ�� */
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;																/*ѡ������� IC2ӳ�䵽TI1��*/
    HAL_TIM_IC_ConfigChannel(&g_timx_pwmin_chy_handle, &sConfigIC, TIM_CHANNEL_2);		/* ����ͨ��2���� */
    
    HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_IRQn,1,3);           												/* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
    HAL_NVIC_EnableIRQ(ATIM_TIMX_PWMIN_IRQn);                 												/* ����ITMx�ж� */
    
    /* TIM1/TIM8 �ж��������벶���жϷ����� */
    if (ATIM_TIMX_PWMIN == TIM1 || ATIM_TIMX_PWMIN == TIM8)
    {
        HAL_NVIC_SetPriority(ATIM_TIMX_PWMIN_CC_IRQn,1,3);    				/* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
        HAL_NVIC_EnableIRQ(ATIM_TIMX_PWMIN_CC_IRQn);          				/* ����ITMx�ж� */
    }
    
    HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle,TIM_CHANNEL_1); 			/* ����TIM8ͨ��1���벶��ģʽ */
    HAL_TIM_IC_Start_IT(&g_timx_pwmin_chy_handle,TIM_CHANNEL_2);			/* ����TIM8ͨ��2���벶��ģʽ */
    __HAL_TIM_ENABLE_IT(&g_timx_pwmin_chy_handle, TIM_IT_UPDATE);     /* ʹ�ܸ����ж� */
}

/**
 * @brief       ��ʱ��PWM����ģʽͨ�����ų�ʼ������
 * @param       ��
 * @note        �˺����ᱻatim_timx_cplm_pwm_init()��������
 * @retval      ��
 */
void atim_timx_pwmin_chy_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    ATIM_TIMX_PWMIN_CHY_GPIO_CLK_ENABLE();   												/* TIM8ͨ��IO�ڣ�PI0��ʱ��ʹ�� */
  
    gpio_init_struct.Pin = ATIM_TIMX_PWMIN_CHY_GPIO_PIN; 						/* ����5 */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;  											/* ����������� */
    gpio_init_struct.Pull = GPIO_PULLDOWN;													/* ����ģʽ */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH ;						/* ����ģʽ */
    gpio_init_struct.Alternate = ATIM_TIMX_PWMIN_CHY_GPIO_AF;				/* AF����ѡ�� */ 
    HAL_GPIO_Init(ATIM_TIMX_PWMIN_CHY_GPIO_PORT, &gpio_init_struct);/* GPIO���ų�ʼ�� */
}

/**
 * @brief       ��ʱ��TIMX ����/��� �жϷ�����
 *   @note      TIM1/TIM8��������������ڸ���/����жϷ���,����������һ������!
 *              ������ͨ��ʱ�������/���/����,��������������洦��!
 * @param       ��
 * @retval      ��
 */
void ATIM_TIMX_PWMIN_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);/* ��ʱ�����ô����� */
}

/**
 * @brief       ��ʱ��TIMX ���벶�� �жϷ�����
 *   @note      ��TIM1/TIM8���������,������ͨ��ʱ��û������жϷ�����!
 * @param       ��
 * @retval      ��
 */
void ATIM_TIMX_PWMIN_CC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_pwmin_chy_handle);/* ��ʱ�����ô����� */
}

/**
 * @brief       ��ʱ��TIM8 PWM����ģʽ ������������
 * @param       ��
 * @retval      ��
 */
void atim_timx_pwmin_chy_restart(void)
{
    INTX_DISABLE();                       /* �ر��ж� */
		//TIM8->DIER=0x0000;									/* �ر��ж� */
    g_timxchy_pwmin_sta = 0;              /* ����״̬,���¿�ʼ��� */
    g_timxchy_pwmin_hval=0;
    g_timxchy_pwmin_cval=0;

    INTX_ENABLE();                      	/* ���ж� */
		//TIM8->DIER=0x07;										/* ���ж� */
}

/**
 * @brief       ��ʱ�����벶���жϴ���ص�����
 * @param       htim:��ʱ�����ָ��
 * @note        �ú�����HAL_TIM_IRQHandler�лᱻ����
 *              �˺����Ƕ�ʱ����ͬ���õĻص�������Ϊ�˷�ֹ�ض������
 *              ���ǰ�gtim.c��HAL_TIM_IC_CaptureCallback()���������ε�.
 * @retval      ��
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    
    if (g_timxchy_pwmin_sta == 0)   				/* ��û�гɹ����� */
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {  
            g_timxchy_pwmin_hval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle,TIM_CHANNEL_2)+2; /* ����ϵ��Ϊ2, ��2 */
            g_timxchy_pwmin_cval = HAL_TIM_ReadCapturedValue(&g_timx_pwmin_chy_handle,TIM_CHANNEL_1)+2; /* ����ϵ��Ϊ2, ��2 */
            g_timxchy_pwmin_sta = 1;        /* ��ǲ���ɹ� */
        }
    }
}







