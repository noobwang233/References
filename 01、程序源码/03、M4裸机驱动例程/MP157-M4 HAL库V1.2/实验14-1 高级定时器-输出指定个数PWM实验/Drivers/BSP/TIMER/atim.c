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

/*********************************�߼���ʱ�����ָ������PWM����*************************************/

TIM_HandleTypeDef g_timx_npwm_chy_handle;     		/* ��ʱ��x��� */
TIM_OC_InitTypeDef g_timx_oc_npwm_chy_handle; 		/* ��ʱ�������� */


/**
 * @brief       ��ʱ��TIMX ͨ��Y ���ָ������PWM ��ʼ������
 * @note
 *              ��ʱ����ʱ������APB1,��D2PPRE1��2��Ƶ��ʱ��
 *              ��ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ120M, ���Զ�ʱ��ʱ�� = 240Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void gtim_timx_npwm_chy_init(uint16_t arr,uint16_t psc)
{
	g_timx_npwm_chy_handle.Instance = GTIM_TIMX_NPWM;       							/* ��ʱ��x */
	g_timx_npwm_chy_handle.Init.Prescaler = psc;             							/* ��ʱ����Ƶ */
	g_timx_npwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;					/*���ϼ���ģʽ*/
	g_timx_npwm_chy_handle.Init.Period = arr;                							/* �Զ���װ��ֵ */
	g_timx_npwm_chy_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;		/* ��Ƶ */
	g_timx_npwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /* ʹ��TIMx_ARR �Ĵ������л��� */
	HAL_TIM_PWM_Init(&g_timx_npwm_chy_handle);   													/* ��ʼ��PWM */
	
	g_timx_oc_npwm_chy_handle.OCMode = TIM_OCMODE_PWM1;   								/* ģʽѡ��PWM1 */
	g_timx_oc_npwm_chy_handle.Pulse = arr/2;  														/* ���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50% */   
	g_timx_oc_npwm_chy_handle.OCPolarity = TIM_OCPOLARITY_HIGH;						/*����Ƚϼ���Ϊ��*/
	HAL_TIM_PWM_ConfigChannel(&g_timx_npwm_chy_handle, &g_timx_oc_npwm_chy_handle, GTIM_TIMX_NPWM_CHY);/* ����TIMxͨ��y */
	
	HAL_TIM_PWM_Start(&g_timx_npwm_chy_handle, GTIM_TIMX_NPWM_CHY);				/*����ͨ��y*/
	__HAL_TIM_ENABLE_IT(&g_timx_npwm_chy_handle, TIM_IT_UPDATE);					/* ʹ�ܸ����ж� */
	
	HAL_NVIC_SetPriority(GTIM_TIMX_NPWM_IRQn, 1, 3); 											/* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */ 
	HAL_NVIC_EnableIRQ(GTIM_TIMX_NPWM_IRQn);          										/* ����ITMx�ж� */
}



/**
 * @brief       ��ʱ���ײ�������ʱ��ʹ�ܣ���������
                 �˺����ᱻHAL_TIM_PWM_Init()����
 * @param       htim:��ʱ�����
 * @retval      ��
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_NPWM)
    {
        GPIO_InitTypeDef gpio_init_struct;
        GTIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE();   												 /* ����ͨ��y��GPIOʱ�� */
        GTIM_TIMX_NPWM_CHY_CLK_ENABLE();

        gpio_init_struct.Pin = GTIM_TIMX_NPWM_CHY_GPIO_PIN;  							/* ͨ��y��GPIO�� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;               						/* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                    					/* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  							/* ���� */
        gpio_init_struct.Alternate = GTIM_TIMX_NPWM_CHY_GPIO_AF; 					/* ��ʱ��xͨ��y��GPIO�ڸ��� */
        HAL_GPIO_Init(GTIM_TIMX_NPWM_CHY_GPIO_PORT, &gpio_init_struct);		/* ��ʼ��GPIO */
    }
}



/* g_npwm_remain��ʾ��ǰ��ʣ�¶��ٸ�����Ҫ���� 
 * ÿ����෢��256������
 */
static uint32_t g_npwm_remain = 0;
/**
 * @brief       ��ʱ��TIMX NPWM����PWM����
 * @param       rcr: PWM�ĸ���, 1~2^32�η���
 * @retval      ��
 */
void gtim_timx_npwm_chy_set(uint32_t npwm)
{
    if (npwm == 0)return ;
    g_npwm_remain = npwm;               /* ����������� */
    GTIM_TIMX_NPWM->EGR |= 1 << 0;      /* ����һ�θ����¼�,���ж����洦��������� */
    GTIM_TIMX_NPWM->CR1 |= 1 << 0;      /* ʹ�ܶ�ʱ��TIMX */
	 //  __HAL_TIM_ENABLE(&g_timx_npwm_chy_handle);  /* ʹ�ܶ�ʱ��TIMX */
}



/**
 * @brief       ��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void GTIM_TIMX_NPWM_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_npwm_chy_handle);
}



/**
 * @brief       ��ʱ�������жϻص�����
 * @param       htim:��ʱ�����ָ��
 * @note        �˺����ᱻ��ʱ���жϺ�����ͬ���õ�
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_timx_npwm_chy_handle)) 		/* ���ָ������PWMʵ��ص�ִ�е����� */
    {
        uint16_t npwm = 0;
        
        if (g_npwm_remain > 256)        			/* ���д���256��������Ҫ���� */
        {
            g_npwm_remain=g_npwm_remain - 256;
            npwm = 256;
        }
        else if (g_npwm_remain % 256)   			/* ����λ��������256��������Ҫ���� */
        {
            npwm = g_npwm_remain % 256; 
            g_npwm_remain = 0;          			/* û�������� */
        }
        if (npwm)   													/* ������Ҫ���� */
        { 
            GTIM_TIMX_NPWM->RCR = npwm - 1; 	/* �����ظ������Ĵ���ֵΪnpwm-1, ��npwm������ */
            GTIM_TIMX_NPWM->EGR |= 1 << 0;  	/* ����һ�θ����¼�,�Ը���RCR�Ĵ��� */
            GTIM_TIMX_NPWM->CR1 |= 1 << 0;  	/* ʹ�ܶ�ʱ��TIMX */
					  //__HAL_TIM_ENABLE(&g_timx_npwm_chy_handle);  /* ʹ�ܶ�ʱ��TIMX */
        }
        else
        { 
            GTIM_TIMX_NPWM->CR1 &= ~(1 << 0);   					/* �رն�ʱ��TIMX */
        }
					__HAL_TIM_CLEAR_IT(&g_timx_npwm_chy_handle, TIM_IT_UPDATE);  /* �����ʱ������жϱ�־λ */

    }
}








