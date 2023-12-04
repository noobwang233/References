/**
 ****************************************************************************************************
 * @file        iic.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-20
 * @brief       IIC ��������
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
 * V1.0 20200520
 * ��һ�η���
 *
 ****************************************************************************************************
 */
#include "./BSP/IIC/iic.h"
#include "./SYSTEM/delay/delay.h"
	I2C_HandleTypeDef g_i2c5_handle;  /* I2C5��� */
	/**
	* @brief   IIC��ʼ������
	* @note    ������֧��ʹ��I2C5,I2Cʱ��ԴΪPCLK1=104.5Mhz��tI2CCLK=1/104500000
	*          Timing����I2C�ٶ����ã���ӦI2CX_TIMINGR�Ĵ������ȼĴ�����λ�������£�
	*          bit[31:28] PRESC  : ʱ���Ƶֵ��ʱ������tPRESC = (PRESC+1)*tI2CCLK
	*          bit[23:20] SCLDEL : ���ݽ���ʱ�䣬Ҳ����SDA��������SCL������֮�����ʱ��
	*              tSCLDEL = (SCLDEL+1)*tPRESC
	*          bit[19:16] SDADEL : ���ݱ���ʱ�䣬Ҳ����SCL�½�����SDA������֮�����ʱ��
	*              tSDADEL = (SDADEL+1)*tPRESC
	*          bit[15:8]  SCLH   : SCL�ߵ�ƽ���ڣ�tSCLH=(SCLH+1)*tPRESC
	*          bit[7:0]   SCLL : SCL�͵�ƽ���ڣ�tSCLL=(SCLL+1)*tPRESC
	*          ͨ��PRESC��SCLH��SCLL����ȷ��I2Cʱ��Ƶ�ʣ����磺
	*          400KHzʱ��I2CX_TIMINGR�Ĵ���=0x00A037BE��PRESEC=0000=0��
	*              tPRESC=(0+1)*(1/104500000)=9.57ns.
	*              SCLH=00110111=55, tSCLH=(55+1)*9.57ns=535.9ns��
	*              SCLL=1011 1110=190, tSCLL=(190+1)*9.57ns=1827.8ns��
	*          SCL��������ֵΪtSCLH+tSCLL=535.9ns+1827.8ns=2363.7nsԼ����423KHz��
	*          ע�⣡0x00A037BE��STM32CubeMX��������ģ������ʵ��ʹ���У�
	*              Timing���������ֱ����STM32CubeMX������
	* @param       ��
	* @retval      ��
	*/
	void iic5_init(void)
	{
		g_i2c5_handle.Instance = I2C5;			/* I2C5 */
		g_i2c5_handle.Init.Timing = 0x00A037BE; /* ����ģʽ400KHz */
		g_i2c5_handle.Init.OwnAddress1 = 0x00;	/* I2C������ַ1(�Զ���,��Ҫ�ʹӻ���ͻ����) */
		/* 7λ��ַģʽ */
		g_i2c5_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		/* �ر�˫��ַģʽ */	
		g_i2c5_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;	
		g_i2c5_handle.Init.OwnAddress2 = 0x00; /* I2C������ַ2(�Զ���,��Ҫ�ʹӻ���ͻ����) */
		/* �رչ㲥����Ѱַģʽ */
		g_i2c5_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		/* �ر�ʱ���ӳ�ģʽ */
		g_i2c5_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;		
		HAL_I2C_Init(&g_i2c5_handle);         /* ����HAL�⽫��������д���Ӧ�Ĵ��� */
		/* ����ģ�������˲��� */
		HAL_I2CEx_ConfigAnalogFilter(&g_i2c5_handle, I2C_ANALOGFILTER_ENABLE); 
		HAL_I2CEx_ConfigDigitalFilter(&g_i2c5_handle, 0);/* �������������˲��� */
	}
	/**
	 * @brief       IIC�ײ�������ʱ������
	 * @param       hrtc:IIC���
	 * @note        �˺����ᱻHAL_I2C_Init()����
	 * @retval      ��
	 */
	void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
	{
	  if(hi2c->Instance == I2C5)
	  {
		GPIO_InitTypeDef GPIO_InitStruct;
		RCC_PeriphCLKInitTypeDef PeriphClkInit;	
		__HAL_RCC_GPIOA_CLK_ENABLE();/* ʹ��GPIOAʱ�� */
		__HAL_RCC_I2C5_CLK_ENABLE();/* ʹ��I2C5ʱ�� */
		/* ����I2C5��I2C3ʱ�� */
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C35;
		/* I2C3,5ʱ��ԴλPCLK1=104.5MHz */
		PeriphClkInit.I2c35ClockSelection = RCC_I2C35CLKSOURCE_PCLK1;
		/* ��ʼ��I2C5ʱ�� */
		HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

		GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;	/* PA11��PA12  */
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C5;		/* ����ΪI2C5 */
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;			/* ��©��� */
		GPIO_InitStruct.Pull = GPIO_NOPULL;				/* ����ģʽ */
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	/* ����ģʽ */
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);			/* ��ʼ��GPIOA */
	  }
	}


