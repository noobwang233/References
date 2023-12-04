/**
 ****************************************************************************************************
 * @file        ds18b20.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-25
 * @brief       DS18B20�����¶ȴ����� ��������
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
 * V1.0 20200525
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/delay/delay.h"
#include "./BSP/DS18B20/ds18b20.h"



/**
 * @brief       ��λDS18B20
 * @param       data: Ҫд�������
 * @retval      ��
 */
static void ds18b20_reset(void)
{
    DS18B20_DQ_OUT(0);  /* ������������DQ,��λ */
    delay_us(750);      /* ����750us */
    DS18B20_DQ_OUT(1);  /* DQ=1, �����ͷŸ�λ */
    delay_us(15);       /* �ӳ�15US */
}

/**
 * @brief       �ȴ�DS18B20�Ļ�Ӧ
 * @param       ��
 * @retval      0, DS18B20����
 *              1, DS18B20�쳣/������
 */
uint8_t ds18b20_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (DS18B20_DQ_IN && retry < 200)    		/* ��ȡDQֵ���ȴ�DQ���, �ȴ�200us */
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 200) 													/* ���ȴ�ʱ�����200usʱ��DS18B20�쳣 */
    {
        rval = 1;
    }
    else																				/* ���ȴ�ʱ��С��200usʱ��DS18B20���� */									
    {
        retry = 0;

        while (!DS18B20_DQ_IN && retry < 240)   /* �ȴ�DQ���, �ȴ�240us */
        {
            retry++;
            delay_us(1);
        }

        if (retry >= 240) rval = 1;							/* ����240us������ΪDS18B20�쳣 */
    }

    return rval;
}

/**
 * @brief       ��DS18B20��ȡһ��λ
 * @param       ��
 * @retval      ��ȡ����λֵ: 0 / 1
 */
static uint8_t ds18b20_read_bit(void)
{
    uint8_t data = 0;		/* ������ֵĬ��Ϊ0 */
    DS18B20_DQ_OUT(0);	/* ����������DQ���� */
    delay_us(2);				/* ��ʱ2us */
    DS18B20_DQ_OUT(1);	/* ����������DQ���ߣ��ͷ����� */
    delay_us(12);				/* ��ʱ12us����������ȡ */

    if (DS18B20_DQ_IN)	/* ������ֵΪ1 */
    {
        data = 1;
    }

    delay_us(50);			  /* ��ʱ50us����Ϊ��������ʱ������Ϊ60us */
    return data;
}

/**
 * @brief       ��DS18B20��ȡһ���ֽ�
 * @param       ��
 * @retval      ����������
 */
static uint8_t ds18b20_read_byte(void)
{
    uint8_t i, b, data = 0;

    for (i = 0; i < 8; i++)			/* һ���ֽ�8λ����8�� */
    {
        b = ds18b20_read_bit(); /* DS18B20�������λ���� ,��λ���ݺ���� */
        
        data |= b << i;         /* ���data��ÿһλ */ 
    }

    return data;
}

/**
 * @brief       дһ���ֽڵ�DS18B20
 * @param       data: Ҫд����ֽ�
 * @retval      ��
 */
static void ds18b20_write_byte(uint8_t data)
{
    uint8_t j;

    for (j = 1; j <= 8; j++)
    {
				/* д 1 ����*/
        if (data & 0x01)
        {
            DS18B20_DQ_OUT(0);  /* ��������DQ */
            delay_us(2);				/* ����2 us��ʱ�� */
            DS18B20_DQ_OUT(1);	/* DQ=1, �����ͷ�DQ */
            delay_us(60);				/* ��ʱ60us */
        }
				/*  д 0 ����*/
        else
        {
            DS18B20_DQ_OUT(0);  /* ��������DQ */
            delay_us(60);				/* ����60 us��ʱ�� */
            DS18B20_DQ_OUT(1);	/* DQ=1, �����ͷ�DQ */
            delay_us(2);				/* ��ʱ2 us */
        }

        data >>= 1;             /* ����,��ȡ��һλ���� */
    }
}

/**
 * @brief       ��ʼ�¶�ת��
 * @param       ��
 * @retval      ��
 */
static void ds18b20_start(void)
{
    ds18b20_reset();						/* ��λDS18B20 */
    ds18b20_check();						/* �ȴ�DS18B20�Ļ�Ӧ */
    ds18b20_write_byte(0xcc);   /*  ����ROM */
    ds18b20_write_byte(0x44);   /*  ��ʼ�¶�ת�� */
}

/**
 * @brief       ��ʼ��DS18B20��IO�� DQ ͬʱ���DS18B20�Ĵ���
 * @param       ��
 * @retval      0, ����
 *              1, ������/������
 */
uint8_t ds18b20_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    DS18B20_DQ_GPIO_CLK_ENABLE();   /* ����DQ����ʱ�� */

    gpio_init_struct.Pin = DS18B20_DQ_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;            /* ��©��� */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* ���� */
    HAL_GPIO_Init(DS18B20_DQ_GPIO_PORT, &gpio_init_struct); /* ��ʼ��DS18B20_DQ���� */
    /* DS18B20_DQ����ģʽ����,��©���,����, �����Ͳ���������IO������, ��©�����ʱ��(=1), Ҳ���Զ�ȡ�ⲿ�źŵĸߵ͵�ƽ */

    ds18b20_reset();
    return ds18b20_check();
}

/**
 * @brief       ��DS18B20�õ��¶�ֵ(���ȣ�0.1C)
 * @param       ��
 * @retval      �¶�ֵ ��-550~1250��
 *   @note      ���ص��¶�ֵ�Ŵ���10��.
 *              ʵ��ʹ�õ�ʱ��,Ҫ����10����ʵ���¶�.
 */
short ds18b20_get_temperature(void)
{
    uint8_t flag = 1;           /* Ĭ���¶�Ϊ���� */
    uint8_t TL, TH;
    short temp;
    
    ds18b20_start();            /*  ��ʼ�¶�ת�� */
    ds18b20_reset();						/* ��λDS18B20 */
    ds18b20_check();						/* �ȴ�DS18B20Ӧ�� */
    ds18b20_write_byte(0xcc);   /*  ����ROM */
    ds18b20_write_byte(0xbe);   /*  �������� */
    TL = ds18b20_read_byte();   /*  ��ȡ�¶ȵ�λֵ LSB */
    TH = ds18b20_read_byte();   /*  ��ȡ�¶ȸ�λֵ MSB */
		
    if (TH > 7)			/* �ж��¶����� */
    {
        TH = ~TH;
        TL = ~TL;
				TL+=1;
        flag = 0;   /* �¶�Ϊ�� */
    }

    temp = TH;      /* ��ø߰�λ */
    temp <<= 8;
    temp += TL;     /* ��õװ�λ */
    temp = (double)temp * 0.625;    /* ת�� */

    if (flag == 0)			/* ����¶�Ϊ���� */
    {
        temp = -temp;   /* ���¶�ת���ɸ��¶� */
    }
    
    return temp;
}
















