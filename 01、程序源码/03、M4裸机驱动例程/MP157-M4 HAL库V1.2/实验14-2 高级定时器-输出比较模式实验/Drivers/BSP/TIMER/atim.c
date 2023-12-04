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

/*********************************  �߼���ʱ������Ƚϳ���*************************************/

TIM_HandleTypeDef g_timx_comp_pwm_handle;                              /* ��ʱ��x��� */

/**
 * @brief       �߼���ʱ��TIMX ����Ƚ�ģʽ ��ʼ��������ʹ������Ƚ�ģʽ��
 * @note
 *              ���ø߼���ʱ��TIMX 2·����Ƚ�ģʽPWM���,ʵ��50%ռ�ձ�,��ͬ��λ����
 *              ע��,����������Ƚ�ģʽ,ÿ2���������ڲ������һ��PWM���,������Ƶ�ʼ���
 *              ����,���ǻ����Կ����ж����ж������޸�CCRx,�Ӷ�ʵ�ֲ�ͬƵ��/��ͬ��λ�Ŀ���
 *              �������ǲ��Ƽ���ôʹ��,��Ϊ����ܵ��·ǳ�Ƶ�����ж�,�Ӷ�ռ�ô���CPU��Դ
 *
 *              �߼���ʱ����ʱ������APB2,��APB2DIV��2��Ƶ��ʱ��
 *              �߼���ʱ����ʱ��ΪAPB2ʱ�ӵ�2��, ��APB2Ϊ104.5M, ���Զ�ʱ��ʱ�� = 209Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    atim_timx_comp_pwm_gpio_init();
    ATIM_TIMX_COMP_CLK_ENABLE();

    g_timx_comp_pwm_handle.Instance = ATIM_TIMX_COMP;                       					/* ��ʱ��1 */
    g_timx_comp_pwm_handle.Init.Prescaler = psc  ;                          					/* ��ʱ����Ƶ */
    g_timx_comp_pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP;           					/* ���ϼ���ģʽ */
    g_timx_comp_pwm_handle.Init.Period = arr;                               					/* �Զ���װ��ֵ */
    g_timx_comp_pwm_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;     					/* ����Ƶ */
    g_timx_comp_pwm_handle.Init.RepetitionCounter = 0;																/* �ظ��������Ĵ���Ϊ0 */
    g_timx_comp_pwm_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;		/* ��ʹ��Ӱ�ӼĴ���TIMx_ARR */
    HAL_TIM_Base_Init(&g_timx_comp_pwm_handle);																				/* ��ʼ����ʱ��ʱ�� */

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;												/* ʹ���ڲ�ʱ�� */
    HAL_TIM_ConfigClockSource(&g_timx_comp_pwm_handle, &sClockSourceConfig);					/* ʱ��Դ���� */

    HAL_TIM_OC_Init(&g_timx_comp_pwm_handle);                               					/* ����Ƚ�ģʽ��ʼ�� */
		/* ��ʱ������ģʽ���ã���������û���õ�����ģʽ */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&g_timx_comp_pwm_handle, &sMasterConfig);
		/* ��ʱ��ͨ��3��ͨ��4������ */
    sConfigOC.OCMode = TIM_OCMODE_TOGGLE;        					/* ����Ƚ�ģʽ��ת���� */
    sConfigOC.Pulse = 500-1;															/* ����ͨ��3����ȽϼĴ�����ֵΪ500-1 */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;						/* ����Ƚϼ���Ϊ�� */ 
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;					/* ��������Ƚϼ���λ�� */
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;						/* ʧ������ȽϿ���ģʽ */
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;				/* ѡ�����״̬�·ǹ���״̬ */
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;			/* ���ÿ���״̬�·ǹ���״̬ */
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_3);	/* ��ʼ����ʱ��������Ƚ�ͨ��3 */
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_3);					/* ͨ��3Ԥװ��ʹ�� */

    sConfigOC.Pulse = 250-1;																											/* ����ͨ��4����ȽϼĴ�����ֵΪ250-1 */
    HAL_TIM_OC_ConfigChannel(&g_timx_comp_pwm_handle, &sConfigOC, TIM_CHANNEL_4);	/* ��ʼ����ʱ��������Ƚ�ͨ��4 */
    __HAL_TIM_ENABLE_OCxPRELOAD(&g_timx_comp_pwm_handle, TIM_CHANNEL_4);					/* ͨ��4Ԥװ��ʹ�� */

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE; 											/* ��������ģʽ�·ǹ���״̬ѡ�� */
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE; 										/* �����ڿ����·ǹ���״̬ѡ�� */
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;														/* ����ƽ���� */
    sBreakDeadTimeConfig.DeadTime = 0;																						/* ����ʱ������Ϊ0 */
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE; 													/* ʧ��TIMxɲ������ */
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;									/* TIM1ɲ������ܽż���Ϊ�ߵ�ƽ��Ч */
    sBreakDeadTimeConfig.BreakFilter = 0;																					/* TIM1ɲ�������˲�Ϊ0 */
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE; 												/* TIM1ɲ������BRK2ʧ�� */
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH; 								/* ɲ������BRK2�ߵ�ƽ��Ч */
    sBreakDeadTimeConfig.Break2Filter = 0; 																				/* TIM1ɲ������2�˲�Ϊ0 */
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE; 					/* ʧ���Զ�������ܣ�ֻ�ܺ����ֶ�ͨ���������MOE */
    HAL_TIMEx_ConfigBreakDeadTime(&g_timx_comp_pwm_handle, &sBreakDeadTimeConfig);/* ���ü�����ܣ�ͣ��ʱ�䣬�������� */
}

/**
 * @brief       ��ʱ������Ƚ�ģʽͨ�����ų�ʼ������
 * @param       ��
 * @note        �˺����ᱻatim_timx_comp_pwm_init()��������
 * @retval      ��
 */
void atim_timx_comp_pwm_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE();														/* ʹ��GPIOEʱ�� */
    ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE();

    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH3_GPIO_PIN;							/* PE13 */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;												/* ����������� */
    gpio_init_struct.Pull = GPIO_NOPULL;														/* ����/���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;									/* �ٶȵȼ�Ϊ�� */
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH3_GPIO_AF;				/* ����ѡ��ΪAF1 */
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH3_GPIO_PORT, &gpio_init_struct);	/* ��ʼ��GPIOE */

    gpio_init_struct.Pin = ATIM_TIMX_COMP_CH4_GPIO_PIN;							/* PE14 */
    gpio_init_struct.Alternate = ATIM_TIMX_COMP_CH4_GPIO_AF;				/* ����ѡ��ΪAF1 */
    HAL_GPIO_Init(ATIM_TIMX_COMP_CH4_GPIO_PORT, &gpio_init_struct);	/* ��ʼ��GPIOE */
}









