/**
 ****************************************************************************************************
 * @file        dac.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-16
 * @brief       DAC 驱动代码
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
 * 修改说明
 * V1.0 20200516
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/DAC/dac.h"
#include "./SYSTEM/delay/delay.h"


DAC_HandleTypeDef g_dac_handle;           /* DAC句柄 */

/**
 * @brief     DAC初始化函数
 *   @note    本函数支持DAC1_OUT1/2通道初始化
 *            DAC的输入时钟来自APB1, 时钟频率=104.5Mhz=9.5ns
 *            DAC在输出buffer关闭的时候, 输出建立时间: tSETTLING = 2us (STM32MP1数据手册有写)
 *            因此DAC输出的最高速度约为:500Khz, 以10个点为一个周期, 最大能输出50Khz左右的波形
 *
 * @param     outx: 要初始化的通道. 1,通道1; 2,通道2
 * @retval    无
 */
void dac_init(uint32_t outx)
{
	DAC_ChannelConfTypeDef dac_ch_conf;     	/* DAC1通道配置结构体 */

	g_dac_handle.Instance = DAC_DACX;					/* DAC1 */
	HAL_DAC_Init(&g_dac_handle);              /* 初始化DAC */

	dac_ch_conf.DAC_Trigger = DAC_TRIGGER_NONE;  																/* 不使用触发功能 */
	dac_ch_conf.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_DISABLE; 	/* 不使能低功耗模式 */
	dac_ch_conf.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE; 									/* DAC采样保持关闭 */
	dac_ch_conf.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;											/* DAC输出缓冲关闭 */
	dac_ch_conf.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;					/* DAC输出不连接到外围设备 */
	dac_ch_conf.DAC_UserTrimming = DAC_TRIMMING_FACTORY;												/* 校准模式为出厂模式 */
	HAL_DAC_ConfigChannel(&g_dac_handle, &dac_ch_conf, outx); 									/* DAC通道配置 */

	HAL_DAC_Start(&g_dac_handle, outx);  																				/* 开启DAC通道 */
}

/**
 * @brief     DAC底层驱动，时钟配置，引脚 配置
 * @param     hdac:DAC句柄
 * @note      此函数会被HAL_DAC_Init()调用
 * @retval    无
 */
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{      
    GPIO_InitTypeDef gpio_init_struct;

    DAC_DACX_CHY_CLK_ENABLE();             										/* 使能DAC时钟 */
    DAC_DACX_CHY_GPIO_CLK_ENABLE();             							/* 开启GPIOA时钟 */

    gpio_init_struct.Pin = DAC_DACX_CHY_GPIO_PIN;  					 	/* PA4引脚 */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG; 								/* 模拟模式 */
    gpio_init_struct.Pull = GPIO_NOPULL;      								/* 不带上下拉 */
    HAL_GPIO_Init(DAC_DACX_CHY_GPIO_PORT, &gpio_init_struct);	/* 初始化GPIO */
}






