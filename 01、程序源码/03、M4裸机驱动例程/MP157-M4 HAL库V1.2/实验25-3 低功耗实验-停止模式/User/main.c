/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-13
 * @brie        睡眠模式 实验
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
#include "./BSP/LED/led.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/KEY/key.h"
#include "./USMART/usmart.h"
#include "./BSP/RNG/rng.h"
#include "./BSP/PWR/pwr.h"


/**
 * @brief       主函数
 * @param       无
 * @retval      无
 */
int main(void)
{
    uint8_t t = 0;
    uint8_t key = 0;
	
	HAL_Init();				/* 初始化HAL库 			*/
	/* 初始化M4内核时钟,209M */
	if(IS_ENGINEERING_BOOT_MODE())
	{
		sys_stm32_clock_init(34, 2, 2, 17, 6826);
	}
	usart_init(115200); 	/* 串口初始化为115200 	*/
	delay_init(209);        /* 延时初始化 			*/
	led_init();				/* 初始化LED  			*/
	key_init();            	/* 初始化按键 			*/
	pwr_wkup_key_init();                /* 唤醒按键初始化 */
	
    while (1)
    {
        key = key_scan(0);

        if (key == KEY0_PRES)	/* 进入睡眠模式 */
        {
            LED1(0);            /* 点亮绿灯,提示进入睡眠模式 */

            pwr_enter_stop();  /* 进入睡眠模式 */
            
            HAL_ResumeTick();   /* 恢复滴答时钟 */
            LED1(1);            /* 关闭绿灯,提示退出睡眠模式 */
        }

        if ((t % 20) == 0)
        {
			t = 0;
            LED0_TOGGLE();      /* 每200ms,翻转一次LED0 */
        }

        delay_ms(10);
        t++;
    }
}
