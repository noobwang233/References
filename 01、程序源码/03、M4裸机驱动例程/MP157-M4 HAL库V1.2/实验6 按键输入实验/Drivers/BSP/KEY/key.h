/**
 ****************************************************************************************************
 * @file        key.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-04
 * @brief       �������� ��������
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
 * V1.0 20200504
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"

/**********************�������Ŷ���*******************************************************/
/* ����KEY0���Ŷ��� */
#define KEY0_GPIO_PORT                  GPIOG
#define KEY0_GPIO_PIN                   GPIO_PIN_3
/* ʹ��PG3ʱ��ʹ�� */
#define KEY0_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)  

/* ����KEY1���Ŷ��� */
#define KEY1_GPIO_PORT                  GPIOH
#define KEY1_GPIO_PIN                   GPIO_PIN_7
/* ʹ��PH7ʱ��ʹ�� */
#define KEY1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)

/* ����WK_UP���Ŷ��� */
#define WKUP_GPIO_PORT                  GPIOA
#define WKUP_GPIO_PIN                   GPIO_PIN_0
/* ʹ��PA0ʱ��ʹ�� */
#define WKUP_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)

/******************************************************************************************/

#define KEY0         HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN)     /* ��ȡKEY0���� */
#define KEY1         HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)     /* ��ȡKEY1���� */
#define WK_UP        HAL_GPIO_ReadPin(WKUP_GPIO_PORT, WKUP_GPIO_PIN)     /* ��ȡWKUP���� */

#define KEY0_PRES    1      /* KEY0���� */
#define KEY1_PRES    2      /* KEY1���� */
#define WKUP_PRES    3      /* KEY_UP���� */

void key_init(void);           /* ������ʼ������ */
uint8_t key_scan(uint8_t mode);/* ����ɨ�躯�� */

#endif











