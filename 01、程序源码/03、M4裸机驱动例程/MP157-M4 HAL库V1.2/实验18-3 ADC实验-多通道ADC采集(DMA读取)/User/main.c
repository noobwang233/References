/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-13
 * @brie        多通道ADC采集(DMA读取) 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32MP1开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/KEY/key.h"
#include "./BSP/ADC/adc.h"

#define ADC_DMA_BUF_SIZE        5      /* ADC DMA采集 BUF大小, 应等于ADC通道数的整数倍 */
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0,未完成; 1, 已完成 */

/**
 * @brief       主函数
 * @param       无
 * @retval      无
 */
int main(void)
{
	HAL_Init();				/* 初始化HAL库 */
	/* 初始化M4内核时钟,209M */
	if(IS_ENGINEERING_BOOT_MODE())
	{
	sys_stm32_clock_init(34, 2, 2, 17, 6826);
	}
	usart_init(115200); 		/* 串口初始化为115200 */
	delay_init(209);        /* 延时初始化 */
	led_init();							/* 初始化LED */
	adc_nch_dma_init((uint32_t)&ADC1->DR, (uint32_t)&g_adc_dma_buf);    /* 初始化ADC DMA采集 */ 
	adc_dma_enable(ADC_DMA_BUF_SIZE);   																/* 启动ADC DMA采集 */
  printf("ADC Start\r\n");
	while (1)
	{	
	if (g_adc_dma_sta == 1)
			{
					/* 循环显示通道14~通道19的结果 */
					printf("ADC Value_ch2 =  %d  Voltage =%.3f \r\n", g_adc_dma_buf[0],(float)g_adc_dma_buf[0] * (3.3 / 65536));
					printf("ADC Value_ch10 = %d  Voltage=%.3f \r\n",	g_adc_dma_buf[1],(float)g_adc_dma_buf[1] * (3.3 / 65536));
					printf("ADC Value_ch16 = %d  Voltage =%.3f \r\n",	g_adc_dma_buf[2],(float)g_adc_dma_buf[2] * (3.3 / 65536));
					printf("ADC Value_ch18 = %d  Voltage =%.3f \r\n",	g_adc_dma_buf[3],(float)g_adc_dma_buf[3] * (3.3 / 65536));
					printf("ADC Value_ch19 = %d  Voltage =%.3f \r\n",	g_adc_dma_buf[4],(float)g_adc_dma_buf[4] * (3.3 / 65536));
					g_adc_dma_sta = 0;  								/* 清除DMA采集完成状态标志 */
					adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动下一次ADC DMA采集 */
			}
			LED0_TOGGLE();		/* LED0翻转 */
			delay_ms(2000);		/* 延时2s */
	}
}
