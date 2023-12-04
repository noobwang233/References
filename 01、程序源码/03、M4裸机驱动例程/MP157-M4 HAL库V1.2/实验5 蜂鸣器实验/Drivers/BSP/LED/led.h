/**
 ****************************************************************************************************
 * @file        led.h
 * @author      ����ԭ��Linux�Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-04
 * @brief       LED ��������
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

#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"

/* PI0���� ���� */
#define LED0_GPIO_PORT                  GPIOI
#define LED0_GPIO_PIN                   GPIO_PIN_0
/* GPIOIʱ��ʹ�� */
#define LED0_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)   
/* PF3���� ���� */
#define LED1_GPIO_PORT                  GPIOF
#define LED1_GPIO_PIN                   GPIO_PIN_3
/* GPIOF��ʱ��ʹ�� */
#define LED1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   
/* LED0��LED1�˿ڶ��� */
#define LED0(x)  do{ x ? \
                 HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_SET) : \
                 HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)       /* LED0 = RED */
#define LED1(x)  do{ x ? \
                 HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET) : \
                 HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)       /* LED1 = GREEN */

/* LEDȡ������ */
#define LED0_TOGGLE()    do{ HAL_GPIO_TogglePin(LED0_GPIO_PORT, \ 											LED0_GPIO_PIN); }while(0)   /* LED0 = !LED0 */
#define LED1_TOGGLE()    do{ HAL_GPIO_TogglePin(LED1_GPIO_PORT, \ 											LED1_GPIO_PIN); }while(0)   /* LED1 = !LED1 */

void led_init(void);    /* ��������������ʼ�� LED */

#endif


