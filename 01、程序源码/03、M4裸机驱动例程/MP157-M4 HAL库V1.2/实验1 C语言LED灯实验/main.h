/**
 ****************************************************************************************************
 * @file        main.h
 * @author      正点原子Linux团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-03
 * @brief       定义一些寄存器地址信息
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32MP15X开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200503
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __MAIN_H
#define __MAIN_H

/* 
 * 各个外设基地址 
 */
#define PERIPH_BASE				(0x40000000)											/* 定义一个地址 */
#define MCU_AHB4_PERIPH_BASE  	(PERIPH_BASE + 0x10000000) 	/* AHB4的基地址为0x50000000 */
#define RCC_BASE        		(MCU_AHB4_PERIPH_BASE + 0x0000)	/* RCC的基地址为0x50000000 */
#define GPIOI_BASE      		(MCU_AHB4_PERIPH_BASE + 0xA000)	/* GPIOI的基地址为0x5000A000 */	
#define GPIOF_BASE      		(MCU_AHB4_PERIPH_BASE + 0x7000)	/* GPIOF的基地址为0x50007000 */	

/* 
 * 寄存器地址 
 */
#define RCC_MC_AHB4ENSETR		*((volatile unsigned int *)(RCC_BASE + 0XAA8))			/* RCC_MC_AHB4ENSETR寄存器的地址为0x50000AA8 */
#define GPIOI_MODER      		*((volatile unsigned int *)(GPIOI_BASE + 0x0000))		/* GPIOI_MODER的地址为 0x5000A000 */
#define GPIOI_OTYPER      		*((volatile unsigned int *)(GPIOI_BASE + 0x0004))	/* GPIOI_OTYPER的地址为0x5000A004 */
#define GPIOI_OSPEEDR      		*((volatile unsigned int *)(GPIOI_BASE + 0x0008))	/* GPIOI_OSPEEDR 的地址为0x5000A008 */
#define GPIOI_PUPDR      		*((volatile unsigned int *)(GPIOI_BASE + 0x000C))		/* GPIOI_PUPDR的地址为0x5000A00C */
#define GPIOI_BSRR      		*((volatile unsigned int *)(GPIOI_BASE + 0x0018)) 	/* GPIOI_BSRR的地址为 0x5000A018 */

#define GPIOF_MODER      		*((volatile unsigned int *)(GPIOF_BASE + 0x0000))		/* GPIOF_MODER的地址为0x50007000 */
#define GPIOF_OTYPER      		*((volatile unsigned int *)(GPIOF_BASE + 0x0004))	/* GPIOF_OTYPER的地址为0x50007004 */
#define GPIOF_OSPEEDR      		*((volatile unsigned int *)(GPIOF_BASE + 0x0008))	/* GPIOF_OSPEEDR的地址为0x50007008 */
#define GPIOF_PUPDR      		*((volatile unsigned int *)(GPIOF_BASE + 0x000C))		/* GPIOF_PUPDR 的地址为 0x5000700C */
#define GPIOF_BSRR      		*((volatile unsigned int *)(GPIOF_BASE + 0x0018)) 	/* GPIOF_BSRR 地址为0x50007018 */


#define OFF 	0		/* LED关 */
#define ON 		1		/* LED开 */

#endif

