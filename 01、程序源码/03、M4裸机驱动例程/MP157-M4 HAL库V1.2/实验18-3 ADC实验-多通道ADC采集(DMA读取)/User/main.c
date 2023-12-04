/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-13
 * @brie        ��ͨ��ADC�ɼ�(DMA��ȡ) ʵ��
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

#define ADC_DMA_BUF_SIZE        5      /* ADC DMA�ɼ� BUF��С, Ӧ����ADCͨ������������ */
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */
extern uint8_t g_adc_dma_sta;               /* DMA����״̬��־, 0,δ���; 1, ����� */

/**
 * @brief       ������
 * @param       ��
 * @retval      ��
 */
int main(void)
{
	HAL_Init();				/* ��ʼ��HAL�� */
	/* ��ʼ��M4�ں�ʱ��,209M */
	if(IS_ENGINEERING_BOOT_MODE())
	{
	sys_stm32_clock_init(34, 2, 2, 17, 6826);
	}
	usart_init(115200); 		/* ���ڳ�ʼ��Ϊ115200 */
	delay_init(209);        /* ��ʱ��ʼ�� */
	led_init();							/* ��ʼ��LED */
	adc_nch_dma_init((uint32_t)&ADC1->DR, (uint32_t)&g_adc_dma_buf);    /* ��ʼ��ADC DMA�ɼ� */ 
	adc_dma_enable(ADC_DMA_BUF_SIZE);   																/* ����ADC DMA�ɼ� */
  printf("ADC Start\r\n");
	while (1)
	{	
	if (g_adc_dma_sta == 1)
			{
					/* ѭ����ʾͨ��14~ͨ��19�Ľ�� */
					printf("ADC Value_ch2 =  %d  Voltage =%.3f \r\n", g_adc_dma_buf[0],(float)g_adc_dma_buf[0] * (3.3 / 65536));
					printf("ADC Value_ch10 = %d  Voltage=%.3f \r\n",	g_adc_dma_buf[1],(float)g_adc_dma_buf[1] * (3.3 / 65536));
					printf("ADC Value_ch16 = %d  Voltage =%.3f \r\n",	g_adc_dma_buf[2],(float)g_adc_dma_buf[2] * (3.3 / 65536));
					printf("ADC Value_ch18 = %d  Voltage =%.3f \r\n",	g_adc_dma_buf[3],(float)g_adc_dma_buf[3] * (3.3 / 65536));
					printf("ADC Value_ch19 = %d  Voltage =%.3f \r\n",	g_adc_dma_buf[4],(float)g_adc_dma_buf[4] * (3.3 / 65536));
					g_adc_dma_sta = 0;  								/* ���DMA�ɼ����״̬��־ */
					adc_dma_enable(ADC_DMA_BUF_SIZE);   /* ������һ��ADC DMA�ɼ� */
			}
			LED0_TOGGLE();		/* LED0��ת */
			delay_ms(2000);		/* ��ʱ2s */
	}
}
