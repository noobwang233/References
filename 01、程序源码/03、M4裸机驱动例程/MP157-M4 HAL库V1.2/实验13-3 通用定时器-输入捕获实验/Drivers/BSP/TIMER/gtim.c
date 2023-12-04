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

/*********************************ͨ�ö�ʱ�����벶��ʵ�����*************************************/

TIM_HandleTypeDef g_timx_cap_chy_handler;      /* ��ʱ��x��� */
TIM_IC_InitTypeDef g_timx_ic_cap_chy_handler;


/**
 * @brief       ͨ�ö�ʱ��TIMX ͨ��Y ���벶�� ��ʼ������
 * @note
 *              ͨ�ö�ʱ����ʱ������APB1,��APB1DIV��2��Ƶ��ʱ��
 *              ͨ�ö�ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ104.5M, ���Զ�ʱ��ʱ�� = 209Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void gtim_timx_cap_chy_init(uint16_t arr, uint16_t psc)
{
		g_timx_cap_chy_handler.Instance = GTIM_TIMX_CAP;             					/* ��ʱ��5 */
		g_timx_cap_chy_handler.Init.Prescaler = psc;                  				/* ��ʱ����Ƶ */
		g_timx_cap_chy_handler.Init.CounterMode = TIM_COUNTERMODE_UP;					/* ���ϼ���ģʽ*/
		g_timx_cap_chy_handler.Init.Period = arr;                     				/* �Զ���װ��ֵ */
		g_timx_cap_chy_handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;		/* ��Ƶ */
		HAL_TIM_IC_Init(&g_timx_cap_chy_handler);
}


/**
 * @brief       ͨ�ö�ʱ�����벶�����ų�ʼ������
 * @param       htim:��ʱ�����
 * @note        �˺����ᱻHAL_TIM_IC_Init()����
 * @retval      ��
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == GTIM_TIMX_CAP)                       					/* ����ͨ������ */
    {
        GPIO_InitTypeDef gpio_init_struct;
        GTIM_TIMX_CAP_CHY_CLK_ENABLE();                         				/* ʹ��TIMxʱ�� */
        GTIM_TIMX_CAP_CHY_GPIO_CLK_ENABLE();                   					/* ��������IO��ʱ�� */

        gpio_init_struct.Pin = GTIM_TIMX_CAP_CHY_GPIO_PIN;  						/* ���벶���GPIO�� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;              					/* ����������� */
        gpio_init_struct.Pull = GPIO_PULLDOWN;                					/* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;							/* ���� */
        gpio_init_struct.Alternate = GTIM_TIMX_CAP_CHY_GPIO_AF; 				/* ����Ϊ����TIM5��ͨ��1 */
        HAL_GPIO_Init(GTIM_TIMX_CAP_CHY_GPIO_PORT, &gpio_init_struct);
			
        g_timx_ic_cap_chy_handler.ICPolarity= TIM_ICPOLARITY_RISING;			/*�����ز���*/
        g_timx_ic_cap_chy_handler.ICSelection = TIM_ICSELECTION_DIRECTTI; /* ӳ�䵽TI1�� */
        g_timx_ic_cap_chy_handler.ICPrescaler = TIM_ICPSC_DIV1;						/* ���������Ƶ */
        g_timx_ic_cap_chy_handler.ICFilter = 0;        										/* ���������˲��������˲� */
        HAL_TIM_IC_ConfigChannel(&g_timx_cap_chy_handler, &g_timx_ic_cap_chy_handler, GTIM_TIMX_CAP_CHY);/* ����TIM5ͨ��1 */
			
        HAL_TIM_IC_Start_IT(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY);	/*����*/
        __HAL_TIM_ENABLE_IT(&g_timx_cap_chy_handler, TIM_IT_UPDATE);			/* ʹ���ж� */

        HAL_NVIC_SetPriority(GTIM_TIMX_CAP_IRQn, 1, 3);  									/* ��ռ1�������ȼ�3 */
        HAL_NVIC_EnableIRQ(GTIM_TIMX_CAP_IRQn);            								/* ����ITMx�ж� */
    }
}


/* ���벶��״̬(g_timxchy_cap_sta)
 * [7]  :0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
 * [6]  :0,��û���񵽸ߵ�ƽ;1,�Ѿ����񵽸ߵ�ƽ��.
 * [5:0]:����ߵ�ƽ������Ĵ���,������63��,���������ֵ = 63*65536 + 65535 = 4194303
 *       ע��:Ϊ��ͨ��,����Ĭ��ARR��CCRy����16λ�Ĵ���,����32λ�Ķ�ʱ��(��:TIM5),Ҳֻ��16λʹ��
 *       ��1us�ļ���Ƶ��,����ʱ��Ϊ:4194303 us, Լ4.19��
 *
 *      (˵��һ�£�����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
 */

uint8_t g_timxchy_cap_sta = 0;  /* ���벶��״̬ */
uint16_t g_timxchy_cap_val =0 ; /* ���벶��ֵ */

/**
 * @brief       ��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void GTIM_TIMX_CAP_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_cap_chy_handler);			/* ��ʱ�����ô����� */
}



/**
 * @brief       ��ʱ�����벶���жϴ���ص�����
 * @param       htim:��ʱ�����ָ��
 * @note        �ú�����HAL_TIM_IRQHandler�лᱻ����
 * @retval      ��
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if ((g_timxchy_cap_sta & 0X80) == 0)         			/* ��δ�ɹ����� */
    {
        if (g_timxchy_cap_sta & 0X40)            			/* ����һ���½��� */
        {
            g_timxchy_cap_sta |= 0X80;           			/* ��ǳɹ�����һ�θߵ�ƽ���� */
            g_timxchy_cap_val = HAL_TIM_ReadCapturedValue(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY); 			/* ��ȡ��ǰ�Ĳ���ֵ */
            TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY);                     			/* һ��Ҫ�����ԭ�������� */
            TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING);			/* ����TIM5ͨ��1�����ز��� */
        }
        else                                     			/* ��δ��ʼ,��һ�β��������� */
        {
            g_timxchy_cap_sta = 0;               			/* ��� */
            g_timxchy_cap_val = 0;
            g_timxchy_cap_sta |= 0X40;           			/* ��ǲ����������� */
            __HAL_TIM_DISABLE(&g_timx_cap_chy_handler);  														/* �رն�ʱ��5 */
            __HAL_TIM_SET_COUNTER(&g_timx_cap_chy_handler,0);
            TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY);   /* һ��Ҫ�����ԭ�������ã��� */
            TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_FALLING);/* ��ʱ��5ͨ��1����Ϊ�½��ز��� */
            __HAL_TIM_ENABLE(&g_timx_cap_chy_handler);   															/* ʹ�ܶ�ʱ��5 */
        }
    }
}



/**
 * @brief       ��ʱ�������жϻص�����
 * @param        htim:��ʱ�����ָ��
 * @note        �˺����ᱻ��ʱ���жϺ�����ͬ���õ�
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_timx_cap_chy_handler))
    {
        if ((g_timxchy_cap_sta & 0X80) == 0)               /* ��δ�ɹ����� */
        {
            if (g_timxchy_cap_sta & 0X40)                  /* �Ѿ����񵽸ߵ�ƽ�� */
            {
                if ((g_timxchy_cap_sta & 0X3F) == 0X3F)    /* �ߵ�ƽ̫���� */
                {
										g_timxchy_cap_val = HAL_TIM_ReadCapturedValue (&g_timx_cap_chy_handler, TIM_CHANNEL_1);     /* ��ȡ��ǰ�Ĳ���??? */
										TIM_RESET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY);									    /* һ��Ҫ�����ԭ�������� */
										TIM_SET_CAPTUREPOLARITY(&g_timx_cap_chy_handler, GTIM_TIMX_CAP_CHY, TIM_ICPOLARITY_RISING); /* ��ʱ��5ͨ��1����Ϊ�½��ز��� */
										g_timxchy_cap_sta |= 0X80;             /* ��ǳɹ�������һ�� */
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









