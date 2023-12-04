/**
 ****************************************************************************************************
 * @file        ap3216c.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-20
 * @brief       AP3216C 驱动代码
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
#include "./BSP/AP3216C/ap3216c.h"
#include "./BSP/IIC/iic.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief       初始化AP3216C
 * @param       无
 * @retval      0,初始化成功
 *				1,初始化失败
 */
uint8_t ap3216c_init(void)
{
	uint8_t temp=0;
         				
		iic_init();													/* 初始化IIC */
    ap3216c_write_one_byte(0x00, 0X04);	/* 复位AP3216C */
    delay_ms(50);												/* AP33216C复位至少10ms */
    ap3216c_write_one_byte(0x00, 0X03);	/* 开启ALS、PS、IR */ 
	temp=ap3216c_read_one_byte(0X00);			/* 读取刚刚写进去的0X03 */
	if(temp == 0X03)
		return 0;						/* AP3216C正常 */
	else 
		return 1;						/* AP3216C失败 */
} 


/**
 * @brief       读取AP3216C的数据
 * 	@note				注意！如果同时打开ALS,IR、PS的话两次数据读取的时间间隔要大于112.5ms
 * @param       ir:  读取到的ir值
 * @param       ps:  读取到的ps值
 * @param       als: 读取到的als值
 * @retval      无
 */
void ap3216c_read_data(uint16_t* ir,uint16_t* ps,uint16_t* als)
{
	uint8_t i;
    uint8_t buf[6];

    for(i=0;i<6;i++)	
    {
        buf[i]=ap3216c_read_one_byte(0X0A + i);			/* 循环读取所有传感器数据*/
    }
	
    if(buf[0] & 0X80)
		*ir = 0;																				/* IR_OF位为1,则数据无效 */
	else 
		*ir = ((uint16_t)buf[1] << 2) | (buf[0] & 0X03);/* 读取IR传感器的数据 */ 
	
	*als = ((uint16_t)buf[3] << 8) | buf[2];					/* 读取ALS传感器的数据 */ 
	
    if(buf[4] & 0x40)
		*ps=0;    																			/* IR_OF位为1,则数据无效 */
	else 
		*ps=((uint16_t)(buf[5]&0X3F)<<4)|(buf[4]&0X0F);	/* 读取PS传感器的数据   */ 
}

/**
 * @brief       向AP3216C指定寄存器写一个字节  
 * @param       reg:  寄存器地址
 * @param       data: 要写入的数据
 * @retval      无
 */
void ap3216c_write_one_byte(uint8_t reg,uint8_t data)
{
	  INTX_DISABLE();										/* I2C通信的时候,关闭中断，防止干扰 */
	  iic_start();                    	/* 主机发送起始信号 */
    iic_send_byte(AP3216C_ADDR_W);  	/* 主机发送AP3216C的设备地址 */
    iic_wait_ack();                 	/* 主机等待AP3216C应答 */
    iic_send_byte(reg);             	/* 主机收到应答后，发送AP3216C寄存器地址 */
    iic_wait_ack();                 	/* 主机等待AP3216C的应答信号 */
    iic_send_byte(data);            	/* 主机发送要传输到寄存器的数据 */
    iic_wait_ack();                 	/* 主机等待AP3216C的应答信号 */
    iic_stop();                     	/* 主机发送停止信号，完成写的过程 */
}

/**
 * @brief       读取AP3216C指定寄存器数据  
 * @param       reg:  寄存器地址
 * @retval      读到的数据
 */
uint8_t ap3216c_read_one_byte(uint8_t reg)
{
	  uint8_t res;
		INTX_DISABLE();										/* I2C通信的时候,关闭中断，防止干扰 */
    iic_start();                    	/* 主机发送起始信号 */
    iic_send_byte(AP3216C_ADDR_W);  	/* 主机发送AP3216C的设备地址 */
    iic_wait_ack();                 	/* 主机等待AP3216C的应答 */
    iic_send_byte(reg);             	/* 主机收到应答信号后，发送AP3216C的寄存器地址 */
    iic_wait_ack();                 	/* 主机等待AP3216C的应答信号 */
    iic_start();                   		/* 主机发送重复起始信号 */
    iic_send_byte(AP3216C_ADDR_R);  	/* 主机发送AP3216C的设备地址 */
    iic_wait_ack();                 	/* 主机等待AP3216C的应答信号 */
    res=iic_read_byte(0);           	/* 主机读取AP3216C寄存器的数据 */
    iic_stop();                     	/* 主机发送停止信号，完成读的过程 */
		INTX_ENABLE();										/* I2C通信完成，打开中断 */
    return res;                     	/* 返回主机读取到的数据 */
}

