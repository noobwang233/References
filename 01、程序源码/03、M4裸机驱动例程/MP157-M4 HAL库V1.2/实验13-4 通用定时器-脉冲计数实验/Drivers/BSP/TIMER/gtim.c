/**
 ****************************************************************************************************
 * @file        gtim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-06
 * @brief       ͨ�ö�ʱ�� ��������
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

#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"

/*********************************ͨ�ö�ʱ���������ʵ�����*************************************/

TIM_HandleTypeDef g_timx_cnt_chy_handler;      /* ��ʱ��x��� */


/* ��¼��ʱ�����������������, ���������������� */
uint32_t g_timxchy_cnt_ofcnt = 0 ;  					/* ����������� */


/**
 * @brief       ͨ�ö�ʱ��TIMX ͨ��Y ������� ��ʼ������
 * @note
 *              ������ѡ��ͨ�ö�ʱ����ʱ��ѡ��: �ⲿʱ��Դģʽ1(SMS[2:0] = 111)
 *              ����CNT�ļ���ʱ��Դ������ TIMX_CH1/CH2, ����ʵ���ⲿ�������(�������CH1/CH2)
 *
 *              ʱ�ӷ�Ƶ�� = psc, һ������Ϊ0, ��ʾÿһ��ʱ�Ӷ������һ��, ����߾���.
 *              ͨ����ȡCNT���������, �����򵥼���, ���Եõ���ǰ�ļ���ֵ, �Ӷ�ʵ���������
 *
 * @param       arr: �Զ���װֵ 
 * @retval      ��
 */
void gtim_timx_cnt_chy_init(uint16_t psc)
{
	 g_timx_cnt_chy_handler.Instance = GTIM_TIMX_CNT;               				/* ��ʱ��x */
    g_timx_cnt_chy_handler.Init.Prescaler = psc;                    			/* ��ʱ����Ƶ */
    g_timx_cnt_chy_handler.Init.CounterMode = TIM_COUNTERMODE_UP;					/*���ϼ���ģʽ */
    g_timx_cnt_chy_handler.Init.Period = 65535;                     			/* �Զ���װ��ֵ */
    g_timx_cnt_chy_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;		/* ��Ƶ */
    HAL_TIM_IC_Init(&g_timx_cnt_chy_handler);
}



/**
 * @brief       ͨ�ö�ʱ�����벶���ʼ���ӿ�
                 HAL����õĽӿڣ��������ò�ͬ�����벶��
 * @param       htim:��ʱ�����
 * @note        �˺����ᱻHAL_TIM_IC_Init()����
 * @retval      ��
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_CNT)              						/*�����������*/
    {
        GPIO_InitTypeDef gpio_init_struct;
        TIM_SlaveConfigTypeDef  tim_slave_config_handle = {0};
        TIM_IC_InitTypeDef timx_ic_cnt_chy_handler = {0};
        GTIM_TIMX_CNT_CHY_CLK_ENABLE();                						/* ʹ��TIMxʱ�� */
        GTIM_TIMX_CNT_CHY_GPIO_CLK_ENABLE();          						/* ����GPIOAʱ�� */

        gpio_init_struct.Pin = GTIM_TIMX_CNT_CHY_GPIO_PIN;  			/* ���벶���GPIO�� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;              		/* ����������� */
        gpio_init_struct.Pull = GPIO_PULLDOWN;                		/* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;				/* ���� */
        gpio_init_struct.Alternate = GTIM_TIMX_CNT_CHY_GPIO_AF;		/* ����Ϊ����ͨ�� */
        HAL_GPIO_Init(GTIM_TIMX_CNT_CHY_GPIO_PORT, &gpio_init_struct);

        tim_slave_config_handle.SlaveMode = TIM_SLAVEMODE_EXTERNAL1; 							/* ��ģʽ���ⲿ����ģʽ1 */
        tim_slave_config_handle.InputTrigger = TIM_TS_TI1FP1; 										/* ���봥����ѡ�� TI1FP1(TIMX_CH1) ��Ϊ����Դ */
        tim_slave_config_handle.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;  		/* �������ԣ������� */ 
        tim_slave_config_handle.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;   	/* ����Ԥ��Ƶ���� */
        tim_slave_config_handle.TriggerFilter = 0x0; 															/* �˲��������в���Ҫ�κ��˲� */
        HAL_TIM_SlaveConfigSynchro(&g_timx_cnt_chy_handler,&tim_slave_config_handle);

        /* �������벶��ģʽ */
        timx_ic_cnt_chy_handler.ICPolarity = TIM_ICPOLARITY_RISING;							/* �����ز��� */
        timx_ic_cnt_chy_handler.ICSelection = TIM_ICSELECTION_DIRECTTI;					/* ӳ�䵽TI1�� */
        timx_ic_cnt_chy_handler.ICPrescaler = TIM_ICPSC_DIV1;										/*�������벻��Ƶ*/
        timx_ic_cnt_chy_handler.ICFilter = 0;    																/* ���������˲��������˲� */
        HAL_TIM_IC_ConfigChannel(&g_timx_cnt_chy_handler, &timx_ic_cnt_chy_handler, GTIM_TIMX_CNT_CHY);		/* ����TIMxͨ��y */

        HAL_TIM_IC_Start(&g_timx_cnt_chy_handler, GTIM_TIMX_CNT_CHY);  	/* ��ʼ����TIMx��ͨ��y */      
        __HAL_TIM_ENABLE_IT(&g_timx_cnt_chy_handler,TIM_IT_UPDATE);			/*ʹ�ܸ����ж�*/

        HAL_NVIC_SetPriority(GTIM_TIMX_CNT_IRQn,1,3);  									/* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */  
        HAL_NVIC_EnableIRQ(GTIM_TIMX_CNT_IRQn);          								/* ����ITMx�ж� */
    }
}



/**
 * @brief       ͨ�ö�ʱ��TIMX ������� �����жϷ�����
 * @param       ��
 * @retval      ��
 */
void GTIM_TIMX_CNT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_cnt_chy_handler);	/* ��ʱ�����ô����� */
}



/**
 * @brief       ��ʱ�������жϻص�����
 * @param        htim:��ʱ�����ָ��
 * @note        �˺����ᱻ��ʱ���жϺ�����ͬ���õ�
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_timx_cnt_chy_handler))
    {
        g_timxchy_cnt_ofcnt++;          			/* �ۼ�������� */
    }
}



/**
 * @brief       ͨ�ö�ʱ��TIMX ͨ��Y ��ȡ��ǰ����ֵ 
 * @param       ��
 * @retval      ��ǰ����ֵ
 */
uint32_t gtim_timx_cnt_chy_get_count(void)
{
    uint32_t count = 0;
    count = g_timxchy_cnt_ofcnt * 65536;														/* �������������Ӧ�ļ���ֵ */
    count += __HAL_TIM_GET_COUNTER(&g_timx_cnt_chy_handler);        /* ���ϵ�ǰCNT��ֵ */
    return count;
}



/**
 * @brief       ͨ�ö�ʱ��TIMX ͨ��Y ����������
 * @param       ��
 * @retval      ��ǰ����ֵ
 */
void gtim_timx_cnt_chy_restart(void)
{
    __HAL_TIM_DISABLE(&g_timx_cnt_chy_handler);           /* �رն�ʱ��TIMX */
    g_timxchy_cnt_ofcnt = 0;                              /* �ۼ������� */
    __HAL_TIM_SET_COUNTER(&g_timx_cnt_chy_handler, 0);    /* ���������� */
    __HAL_TIM_ENABLE(&g_timx_cnt_chy_handler);            /* ʹ�ܶ�ʱ��TIMX */
}















