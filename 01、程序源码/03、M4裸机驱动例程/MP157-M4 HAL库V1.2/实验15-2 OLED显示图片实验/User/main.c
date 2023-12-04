/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-11
 * @brie      	OLED ʵ��
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
#include "./BSP/LED/led.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/KEY/key.h"
#include "./BSP/OLED/oled.h"
#include "./BSP/OLED/logo.h"

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
	delay_init(209);        /* ��ʱ��ʼ�� */
	led_init();							/* ��ʼ��LED */
	oled_init();            /* ��ʼ��OLED */
    while (1)
    {
			OLED_Show(0,0,960, 60, Image, 1);		/* ��ʾһ��ͼƬ */
			oled_refresh_gram();								/*�����Դ浽OLED */
			delay_ms(500);											/* ��ʱ500ms */
			LED0_TOGGLE();                    	/* LED0��˸ */
    }
}
