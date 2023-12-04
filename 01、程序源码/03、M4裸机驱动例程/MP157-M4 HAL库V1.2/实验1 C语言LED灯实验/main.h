/**
 ****************************************************************************************************
 * @file        main.h
 * @author      ����ԭ��Linux�Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-03
 * @brief       ����һЩ�Ĵ�����ַ��Ϣ
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32MP15X������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200503
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __MAIN_H
#define __MAIN_H

/* 
 * �����������ַ 
 */
#define PERIPH_BASE				(0x40000000)											/* ����һ����ַ */
#define MCU_AHB4_PERIPH_BASE  	(PERIPH_BASE + 0x10000000) 	/* AHB4�Ļ���ַΪ0x50000000 */
#define RCC_BASE        		(MCU_AHB4_PERIPH_BASE + 0x0000)	/* RCC�Ļ���ַΪ0x50000000 */
#define GPIOI_BASE      		(MCU_AHB4_PERIPH_BASE + 0xA000)	/* GPIOI�Ļ���ַΪ0x5000A000 */	
#define GPIOF_BASE      		(MCU_AHB4_PERIPH_BASE + 0x7000)	/* GPIOF�Ļ���ַΪ0x50007000 */	

/* 
 * �Ĵ�����ַ 
 */
#define RCC_MC_AHB4ENSETR		*((volatile unsigned int *)(RCC_BASE + 0XAA8))			/* RCC_MC_AHB4ENSETR�Ĵ����ĵ�ַΪ0x50000AA8 */
#define GPIOI_MODER      		*((volatile unsigned int *)(GPIOI_BASE + 0x0000))		/* GPIOI_MODER�ĵ�ַΪ 0x5000A000 */
#define GPIOI_OTYPER      		*((volatile unsigned int *)(GPIOI_BASE + 0x0004))	/* GPIOI_OTYPER�ĵ�ַΪ0x5000A004 */
#define GPIOI_OSPEEDR      		*((volatile unsigned int *)(GPIOI_BASE + 0x0008))	/* GPIOI_OSPEEDR �ĵ�ַΪ0x5000A008 */
#define GPIOI_PUPDR      		*((volatile unsigned int *)(GPIOI_BASE + 0x000C))		/* GPIOI_PUPDR�ĵ�ַΪ0x5000A00C */
#define GPIOI_BSRR      		*((volatile unsigned int *)(GPIOI_BASE + 0x0018)) 	/* GPIOI_BSRR�ĵ�ַΪ 0x5000A018 */

#define GPIOF_MODER      		*((volatile unsigned int *)(GPIOF_BASE + 0x0000))		/* GPIOF_MODER�ĵ�ַΪ0x50007000 */
#define GPIOF_OTYPER      		*((volatile unsigned int *)(GPIOF_BASE + 0x0004))	/* GPIOF_OTYPER�ĵ�ַΪ0x50007004 */
#define GPIOF_OSPEEDR      		*((volatile unsigned int *)(GPIOF_BASE + 0x0008))	/* GPIOF_OSPEEDR�ĵ�ַΪ0x50007008 */
#define GPIOF_PUPDR      		*((volatile unsigned int *)(GPIOF_BASE + 0x000C))		/* GPIOF_PUPDR �ĵ�ַΪ 0x5000700C */
#define GPIOF_BSRR      		*((volatile unsigned int *)(GPIOF_BASE + 0x0018)) 	/* GPIOF_BSRR ��ַΪ0x50007018 */


#define OFF 	0		/* LED�� */
#define ON 		1		/* LED�� */

#endif

