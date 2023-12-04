/**
 ****************************************************************************************************
 * @file        iic.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-20
 * @brief       IIC 驱动代码
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
 * V1.0 20200520
 * 第一次发布
 *
 ****************************************************************************************************
 */
#include "./BSP/IIC/iic.h"
#include "./SYSTEM/delay/delay.h"
	I2C_HandleTypeDef g_i2c5_handle;  /* I2C5句柄 */
	/**
	* @brief   IIC初始化函数
	* @note    本函数支持使用I2C5,I2C时钟源为PCLK1=104.5Mhz，tI2CCLK=1/104500000
	*          Timing负责I2C速度设置，对应I2CX_TIMINGR寄存器，比寄存器各位描述如下：
	*          bit[31:28] PRESC  : 时间分频值，时钟周期tPRESC = (PRESC+1)*tI2CCLK
	*          bit[23:20] SCLDEL : 数据建立时间，也就是SDA跳变沿与SCL上升沿之间的延时，
	*              tSCLDEL = (SCLDEL+1)*tPRESC
	*          bit[19:16] SDADEL : 数据保持时间，也就是SCL下降沿与SDA跳变沿之间的延时，
	*              tSDADEL = (SDADEL+1)*tPRESC
	*          bit[15:8]  SCLH   : SCL高电平周期，tSCLH=(SCLH+1)*tPRESC
	*          bit[7:0]   SCLL : SCL低电平周期，tSCLL=(SCLL+1)*tPRESC
	*          通过PRESC、SCLH和SCLL即可确定I2C时钟频率，比如：
	*          400KHz时：I2CX_TIMINGR寄存器=0x00A037BE，PRESEC=0000=0，
	*              tPRESC=(0+1)*(1/104500000)=9.57ns.
	*              SCLH=00110111=55, tSCLH=(55+1)*9.57ns=535.9ns，
	*              SCLL=1011 1110=190, tSCLL=(190+1)*9.57ns=1827.8ns，
	*          SCL周期理论值为tSCLH+tSCLL=535.9ns+1827.8ns=2363.7ns约等于423KHz。
	*          注意！0x00A037BE是STM32CubeMX计算出来的，因此在实际使用中，
	*              Timing属性最好是直接用STM32CubeMX来计算
	* @param       无
	* @retval      无
	*/
	void iic5_init(void)
	{
		g_i2c5_handle.Instance = I2C5;			/* I2C5 */
		g_i2c5_handle.Init.Timing = 0x00A037BE; /* 高速模式400KHz */
		g_i2c5_handle.Init.OwnAddress1 = 0x00;	/* I2C主机地址1(自定义,不要和从机冲突即可) */
		/* 7位地址模式 */
		g_i2c5_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		/* 关闭双地址模式 */	
		g_i2c5_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;	
		g_i2c5_handle.Init.OwnAddress2 = 0x00; /* I2C主机地址2(自定义,不要和从机冲突即可) */
		/* 关闭广播呼叫寻址模式 */
		g_i2c5_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		/* 关闭时钟延长模式 */
		g_i2c5_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;		
		HAL_I2C_Init(&g_i2c5_handle);         /* 调用HAL库将以上配置写入对应寄存器 */
		/* 配置模拟噪声滤波器 */
		HAL_I2CEx_ConfigAnalogFilter(&g_i2c5_handle, I2C_ANALOGFILTER_ENABLE); 
		HAL_I2CEx_ConfigDigitalFilter(&g_i2c5_handle, 0);/* 配置数字噪声滤波器 */
	}
	/**
	 * @brief       IIC底层驱动，时钟配置
	 * @param       hrtc:IIC句柄
	 * @note        此函数会被HAL_I2C_Init()调用
	 * @retval      无
	 */
	void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
	{
	  if(hi2c->Instance == I2C5)
	  {
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_PeriphCLKInitTypeDef PeriphClkInit;	
		__HAL_RCC_GPIOA_CLK_ENABLE();/* 使能GPIOA时钟 */
		__HAL_RCC_I2C5_CLK_ENABLE();/* 使能I2C5时钟 */
		/* 配置I2C5和I2C3时钟 */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C35;
		/* I2C3,5时钟源位PCLK1=104.5MHz */
		PeriphClkInit.I2c35ClockSelection = RCC_I2C35CLKSOURCE_PCLK1;
		/* 初始化I2C5时钟 */
		HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;	/* PA11和PA12  */
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C5;		/* 复用为I2C5 */
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;			/* 开漏输出 */
		GPIO_InitStruct.Pull = GPIO_NOPULL;				/* 上拉模式 */
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	/* 高速模式 */
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);			/* 初始化GPIOA */
	  }
	}


