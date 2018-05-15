#ifndef __HAL_IIC_H
#define   __HAL_IIC_H

#include "sys.h"

#define IIC_SCL_GPIOx				GPIOB
#define IIC_SDA_GPIOx				GPIOB
#define IIC_SCL_PIN					GPIO_PIN_6
#define IIC_SDA_PIN					GPIO_PIN_7
#define IIC_SCL_RCC_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define IIC_SCL_RCC_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOB_CLK_DISABLE()
#define IIC_SDA_RCC_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define IIC_SDA_RCC_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOB_CLK_DISABLE()

/* 高低电平 */
#define HIGH		1
#define LOW		0

/* IO方向设置 */
#define SDA_IN()	{IIC_SDA_GPIOx->MODER&=~(3<<(7*2));IIC_SDA_GPIOx->MODER|=0<<7*2;}				//PB7输入模式
#define SDA_OUT()	{IIC_SDA_GPIOx->MODER&=~(3<<(7*2));IIC_SDA_GPIOx->MODER|=1<<7*2;}				//PB7输出模式
/* IO操作 */
#define IIC_SCL(n)	(n?HAL_GPIO_WritePin(IIC_SCL_GPIOx, IIC_SCL_PIN, GPIO_PIN_SET):HAL_GPIO_WritePin(IIC_SCL_GPIOx, IIC_SCL_PIN, GPIO_PIN_RESET))	//SCL
#define IIC_SDA(n)	(n?HAL_GPIO_WritePin(IIC_SDA_GPIOx, IIC_SDA_PIN, GPIO_PIN_SET):HAL_GPIO_WritePin(IIC_SDA_GPIOx, IIC_SDA_PIN, GPIO_PIN_RESET))	//SDA
#define READ_SDA	HAL_GPIO_ReadPin(IIC_SDA_GPIOx, IIC_SDA_PIN)									//输入SDA

void IIC_Init(void);													//IIC初始化
void IIC_Start(void);													//产生IIC起始信号
void IIC_Stop(void);													//产生IIC停止信号
u8   IIC_Wait_Ack(void);													//等待ACK信号到来
void IIC_Ack(void);														//产生ACK应答
void IIC_NAck(void);													//不产生ACK应答
void IIC_Send_Byte(u8 txd);												//IIC发送一个字节
u8   IIC_Read_Byte(unsigned char ack);										//IIC读取一个字节

#endif
