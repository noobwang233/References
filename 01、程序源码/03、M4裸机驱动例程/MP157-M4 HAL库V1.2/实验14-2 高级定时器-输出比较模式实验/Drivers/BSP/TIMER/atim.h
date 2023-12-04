/**
 ****************************************************************************************************
 * @file        atim.h
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

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"


/********************************�߼���ʱ�� ����*******************************************************/

/* TIMX ����Ƚ�ģʽ ���� 
 * ����ͨ��TIM1������Ƚ�ģʽ,����PE13,PE14���2·PWM,ռ�ձ�50%,����ÿһ·PWM֮�����λ��Ϊ25%
 * �޸�CCRx�����޸���λ.
 * Ĭ�������TIM1/TIM8
 * ע��: ͨ���޸���Щ�궨��,����֧��TIM1~TIM17����һ����ʱ��,����һ��IO��ʹ������Ƚ�ģʽ,���PWM
 */
#define ATIM_TIMX_COMP_CH3_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH3_GPIO_PIN             GPIO_PIN_13
#define ATIM_TIMX_COMP_CH3_GPIO_AF              GPIO_AF1_TIM1                                 /* AF����ѡ�� */
#define ATIM_TIMX_COMP_CH3_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */

#define ATIM_TIMX_COMP_CH4_GPIO_PORT            GPIOE
#define ATIM_TIMX_COMP_CH4_GPIO_PIN             GPIO_PIN_14
#define ATIM_TIMX_COMP_CH4_GPIO_AF              GPIO_AF1_TIM1                                 /* AF����ѡ�� */
#define ATIM_TIMX_COMP_CH4_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */


#define ATIM_TIMX_COMP                          TIM1            
#define ATIM_TIMX_COMP_CH3_CCRX                 TIM_CHANNEL_3        								/* ͨ��3������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CH4_CCRX                 TIM_CHANNEL_4        								/* ͨ��4������ȽϼĴ��� */
#define ATIM_TIMX_COMP_CLK_ENABLE()             do{ __HAL_RCC_TIM1_CLK_ENABLE(); }while(0)   /* TIM1 ʱ��ʹ�� */
 
/***********************************��������************************************************************/
void atim_timx_comp_pwm_init(uint16_t arr, uint16_t psc);   		/* �߼���ʱ�� ����Ƚ�ģʽ���PWM ��ʼ������ */
void atim_timx_comp_pwm_gpio_init(void);                    		/* ��ʱ��ͨ�����ų�ʼ������ */

#endif

















