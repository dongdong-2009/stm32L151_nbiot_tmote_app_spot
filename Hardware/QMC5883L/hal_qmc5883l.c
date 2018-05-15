/**
  *********************************************************************************************************
  * @file    hal_qmc5883l.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-24
  * @brief   HAL QMC5883L Device
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "hal_qmc5883l.h"
#include "hal_iic.h"
#include "tmesh_algorithm.h"
#include "platform_config.h"
#include "platform_map.h"
#include "stm32l1xx_config.h"
#include "delay.h"
#include <stdlib.h>

/**********************************************************************************************************
 @Function			static void QMC5883L_WriteByte(u8 ucRegAddr, u8 ucRegData)
 @Description			QMC5883L写入1Byte数据到寄存器( 内部调用 )
 @Input				ucRegAddr : 写入地址
					ucRegData : 写入数据
 @Return				void
**********************************************************************************************************/
static void QMC5883L_WriteByte(u8 ucRegAddr, u8 ucRegData)
{
	IIC_Start();														//发送起始信号
	
	IIC_Send_Byte(QMC5883L_SLAVE_ADDRESS_W);								//发送I2C从机地址写
	IIC_Wait_Ack();													//等待应答
	
	IIC_Send_Byte(ucRegAddr);											//发送待写入数据地址
	IIC_Wait_Ack();													//等待应答
	
	IIC_Send_Byte(ucRegData);											//发送需写入数据
	IIC_Wait_Ack();													//等待应答
	
	IIC_Stop();														//发送停止信号
}

/**********************************************************************************************************
 @Function			static u8 QMC5883L_ReadByte(u8 ucRegAddr)
 @Description			QMC5883L读取1Byte数据( 内部调用 )
 @Input				ucRegAddr : 读取数据地址
 @Return				ucRegAddr : 读取数据
**********************************************************************************************************/
static u8 QMC5883L_ReadByte(u8 ucRegAddr)
{
	u8 ucData;
	
	IIC_Start();														//发送起始信号
	
	IIC_Send_Byte(QMC5883L_SLAVE_ADDRESS_W);								//发送I2C从机地址写
	IIC_Wait_Ack();													//等待应答
	
	IIC_Send_Byte(ucRegAddr);											//发送待读取数据地址
	IIC_Wait_Ack();													//等待应答
	
	IIC_Start();														//发送起始信号
	
	IIC_Send_Byte(QMC5883L_SLAVE_ADDRESS_R);								//发送I2C从机地址读
	IIC_Wait_Ack();													//等待应答
	
	ucData = IIC_Read_Byte(0);											//读取数据
	
	IIC_Stop();														//发送停止信号
	
	return ucData;
}

/**********************************************************************************************************
 @Function			void QMC5883L_Drdy_Init(void)
 @Description			QMC5883L引脚配置PA11高电平读取
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Drdy_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	QMC_DRDY_RCC_GPIO_CLK_ENABLE();										//使能QMC DRDY GPIO 时钟
	
	GPIO_Initure.Pin = QMC_DRDY_PIN;										//PA11
	GPIO_Initure.Mode = GPIO_MODE_INPUT;									//输入
	GPIO_Initure.Pull = GPIO_PULLDOWN;										//下拉
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//高速
	HAL_GPIO_Init(QMC_DRDY_GPIOx, &GPIO_Initure);
}

/**********************************************************************************************************
 @Function			void QMC5883L_Drdy_DeInit(void)
 @Description			QMC5883L引脚配置PA11反初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Drdy_DeInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	QMC_DRDY_RCC_GPIO_CLK_ENABLE();										//使能QMC DRDY GPIO 时钟
	
	GPIO_Initure.Pin = QMC_DRDY_PIN;										//PA11
	GPIO_Initure.Mode = GPIO_MODE_INPUT;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(QMC_DRDY_GPIOx, &GPIO_Initure);
}

/**********************************************************************************************************
 @Function			void QMC5883L_Drdy_Exti_Init(void)
 @Description			QMC5883L引脚配置PA11高电平读取(中断)
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Drdy_Exti_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	QMC_DRDY_RCC_GPIO_CLK_ENABLE();										//使能QMC DRDY GPIO 时钟
	
	GPIO_Initure.Pin = QMC_DRDY_PIN;										//PA11
	GPIO_Initure.Mode = GPIO_MODE_IT_RISING;								//中断上升沿触发
	GPIO_Initure.Pull = GPIO_PULLDOWN;										//下拉
	HAL_GPIO_Init(QMC_DRDY_GPIOx, &GPIO_Initure);
	
	HAL_NVIC_SetPriority(QMC_DRDY_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(QMC_DRDY_IRQn);
}

/**********************************************************************************************************
 @Function			void QMC5883L_Discharge(void)
 @Description			QMC5883L复位
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Discharge(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	IIC_SCL_RCC_GPIO_CLK_ENABLE();
	IIC_SDA_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = IIC_SCL_PIN;													//IIC_SCL
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(IIC_SCL_GPIOx, &GPIO_Initure);
	
	GPIO_Initure.Pin = IIC_SDA_PIN;													//IIC_SDA
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(IIC_SDA_GPIOx, &GPIO_Initure);
	
	HAL_GPIO_WritePin(IIC_SCL_GPIOx, IIC_SCL_PIN, GPIO_PIN_RESET);							//IIC_SCL
	HAL_GPIO_WritePin(IIC_SDA_GPIOx, IIC_SDA_PIN, GPIO_PIN_RESET);							//IIC_SDA
	Delay_MS(3000);
	HAL_GPIO_WritePin(IIC_SCL_GPIOx, IIC_SCL_PIN, GPIO_PIN_SET);							//IIC_SCL
	HAL_GPIO_WritePin(IIC_SDA_GPIOx, IIC_SDA_PIN, GPIO_PIN_SET);							//IIC_SDA
}

/**********************************************************************************************************
 @Function			void QMC5883L_Init(void)
 @Description			QMC5883L初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Init(void)
{
	IIC_Init();														//IIC初始化
	
	QMC5883L_WriteByte(QMC5883L_CR2, QMC_SOFT_REST);							//复位QMC5883L
	
	QMC5883L_WriteByte(0x0B, 0x01);
	QMC5883L_WriteByte(0x20, 0x40);
	QMC5883L_WriteByte(0x21, 0x01);
	
	/* 64滤波, 8高斯范围, 50Hz输出, 初始化为StandBy */
	QMC5883L_WriteByte(QMC5883L_CR1, QMC_OSR_64 | QMC_RANGE_8G | QMC_RATES_200HZ | QMC_MODE_STANDBY);
	/* 引脚中断使能, 数据读取完指针自动偏转失能 */
	QMC5883L_WriteByte(QMC5883L_CR2, QMC_INT_ENABLE | QMC_POINT_ROLL_DISABLE);
	
#if QMC_DRDY_EXIT
	QMC5883L_Drdy_Exti_Init();											//QMC5883L引脚配置PA11高电平读取(中断)
#else
	QMC5883L_Drdy_Init();												//QMC5883L引脚配置PA11高电平读取(非中断)
#endif
}

/**********************************************************************************************************
 @Function			unsigned char QMC5883L_ReadData_Simplify(void)
 @Description			QMC5883L读取数据
 @Input				void
 @Return				1		: success
					0		: fail
**********************************************************************************************************/
unsigned char QMC5883L_ReadData_Simplify(void)
{
	u8 ucReadBuf[QMC_REG_MAG];
	u32 qmctimeover = 0;
	u8 ret = 0;
	
	while (1) {
		if (QMC_DRDY_READ() == 1) {
			for (int index = 0; index < QMC_REG_MAG; index++) {
				ucReadBuf[index] = QMC5883L_ReadByte(QMC_DATA_OUT_X_L + index);
			}
			qmctimeover = 0;
			break;
		}
		else {
			qmctimeover++;
			if (qmctimeover >= 10000) {									//超时处理
				break;
			}
		}
		
		Delay_US(10);
	}
	
	Qmc5883lData.X_Now = 0;
	Qmc5883lData.Y_Now = 0;
	Qmc5883lData.Z_Now = 0;
	
	if (qmctimeover == 0) {
		Qmc5883lData.X_Now = (int16_t)(ucReadBuf[1] << 8) | ucReadBuf[0];
		Qmc5883lData.Y_Now = (int16_t)(ucReadBuf[3] << 8) | ucReadBuf[2];
		Qmc5883lData.Z_Now = (int16_t)(ucReadBuf[5] << 8) | ucReadBuf[4];
		ret = 1;
	}
	
	return ret;
}

/**********************************************************************************************************
 @Function			unsigned char QMC5883L_ReadData_Extend(void)
 @Description			QMC5883L读取数据
 @Input				void
 @Return				1		: success
					0		: fail
**********************************************************************************************************/
unsigned char QMC5883L_ReadData_Extend(void)
{
	u8 ucReadBuf[QMC_SAMPLE_TIMES][QMC_REG_MAG];
	int16_t magdata_x[QMC_SAMPLE_TIMES], magdata_y[QMC_SAMPLE_TIMES], magdata_z[QMC_SAMPLE_TIMES];
	u32 qmctimeover = 0;
	u8 index = 0;
	u8 sample_times = 0;
	u8 ret = 0;
	
	while (1) {
		if (QMC_DRDY_READ() == 1) {
			for (index = 0; index < QMC_REG_MAG; index++) {
				ucReadBuf[sample_times][index] = QMC5883L_ReadByte(QMC_DATA_OUT_X_L + index);
			}
			sample_times++;
			qmctimeover = 0;
		}
		else {
			qmctimeover++;
			if (qmctimeover >= 1000) {									//超时处理
				break;
			}
		}
		
		if (sample_times >= QMC_SAMPLE_TIMES) {
			for (index = 0; index < QMC_SAMPLE_TIMES; index++) {
				magdata_x[index] = (int16_t)(ucReadBuf[index][1] << 8) | ucReadBuf[index][0];
				magdata_y[index] = (int16_t)(ucReadBuf[index][3] << 8) | ucReadBuf[index][2];
				magdata_z[index] = (int16_t)(ucReadBuf[index][5] << 8) | ucReadBuf[index][4];
			}
			
			for (index = 1; index < QMC_SAMPLE_TIMES; index++) {
				if (abs(magdata_x[0] - magdata_x[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
				else if (abs(magdata_y[0] - magdata_y[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
				else if (abs(magdata_z[0] - magdata_z[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
			}
			break;
		}
		Delay_US(100);
	}
	
	Qmc5883lData.X_Now = 0;
	Qmc5883lData.Y_Now = 0;
	Qmc5883lData.Z_Now = 0;
	
	if (sample_times >= QMC_SAMPLE_TIMES) {
		for (index = 0; index < QMC_SAMPLE_TIMES; index++) {
			Qmc5883lData.X_Now += (((int16_t)(ucReadBuf[index][1] << 8)|ucReadBuf[index][0])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
			Qmc5883lData.Y_Now += (((int16_t)(ucReadBuf[index][3] << 8)|ucReadBuf[index][2])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
			Qmc5883lData.Z_Now += (((int16_t)(ucReadBuf[index][5] << 8)|ucReadBuf[index][4])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
		}
		ret = 1;
	}
	
	return ret;
}

/**********************************************************************************************************
 @Function			void QMC5883L_ReadData_Single(short* x, short* y, short* z)
 @Description			QMC5883L读取数据
 @Input				x, y, z
 @Return				void
**********************************************************************************************************/
void QMC5883L_ReadData_Single(short* x, short* y, short* z)
{
	u8 ucReadBuf[QMC_SAMPLE_TIMES][QMC_REG_MAG];
	int16_t magdata_x[QMC_SAMPLE_TIMES], magdata_y[QMC_SAMPLE_TIMES], magdata_z[QMC_SAMPLE_TIMES];
	u8 index = 0;
	u8 sample_times = 0;
	u32 qmctimeover = 0;
	static short x_old, y_old, z_old;
	static char same_cnt = 0;
	
	QMC5883L_Mode_Selection(QMC_MODE_CONTINOUS);
	
	while ((qmctimeover++) < 10000) {
		if (QMC_DRDY_READ() == 1) {
			for (index = 0; index < QMC_REG_MAG; index++) {
				ucReadBuf[sample_times][index] = QMC5883L_ReadByte(QMC_DATA_OUT_X_L + index);
			}
			sample_times++;
		}
		
		if (sample_times >= QMC_SAMPLE_TIMES) {
			for (index = 0; index < QMC_SAMPLE_TIMES; index++) {
				magdata_x[index] = (int16_t)(ucReadBuf[index][1] << 8) | ucReadBuf[index][0];
				magdata_y[index] = (int16_t)(ucReadBuf[index][3] << 8) | ucReadBuf[index][2];
				magdata_z[index] = (int16_t)(ucReadBuf[index][5] << 8) | ucReadBuf[index][4];
			}
			
			for (index = 1; index < QMC_SAMPLE_TIMES; index++) {
				if (abs(magdata_x[0] - magdata_x[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
				else if (abs(magdata_y[0] - magdata_y[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
				else if (abs(magdata_z[0] - magdata_z[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
			}
			if (sample_times != 0) {
				break;
			}
		}
		
		Delay_US(10);
	}
	
	if (sample_times >= QMC_SAMPLE_TIMES) {
		*x = 0; *y = 0; *z = 0;
		
		for (index = 0; index < QMC_SAMPLE_TIMES; index++) {
			*x += (((int16_t)(ucReadBuf[index][1] << 8)|ucReadBuf[index][0])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
			*y += (((int16_t)(ucReadBuf[index][3] << 8)|ucReadBuf[index][2])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
			*z += (((int16_t)(ucReadBuf[index][5] << 8)|ucReadBuf[index][4])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
		}
		
		if ((*x == x_old) && (*y == y_old) && (*z == z_old)) {
			same_cnt++;
		}
		
		x_old = *x; y_old = *y; z_old = *z;
		
		if (same_cnt > 60) {
			same_cnt = 0;
			ReInitModule();
		}
	}
	else {
		ReInitModule();
	}
	
	QMC5883L_Mode_Selection(QMC_MODE_STANDBY);
}

/**********************************************************************************************************
 @Function			void QMC5883L_ClearInsideData(void)
 @Description			QMC5883L清除内部待读取数据缓存
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_ClearInsideData(void)
{
	u8 index = 0;
	
	if (QMC_DRDY_READ() == 1) {
		for (index = 0; index < QMC_REG_MAG; index++) {
			QMC5883L_ReadByte(QMC_DATA_OUT_X_L + index);
		}
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Mode_Selection(u8 mode)
 @Description			QMC5883L工作模式选择
 @Input				mode : QMC_MODE_STANDBY/QMC_MODE_CONTINOUS
 @Return				void
**********************************************************************************************************/
void QMC5883L_Mode_Selection(u8 mode)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR1);
	if (mode == QMC_MODE_STANDBY) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xFC) | QMC_MODE_STANDBY);
	}
	else if (mode == QMC_MODE_CONTINOUS) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xFC) | QMC_MODE_CONTINOUS);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Rates_Selection(u8 rates)
 @Description			QMC5883L数据输出速率选择
 @Input				rates : QMC_RATES_10HZ/QMC_RATES_50HZ/QMC_RATES_100HZ/QMC_RATES_200HZ
 @Return				void
**********************************************************************************************************/
void QMC5883L_Rates_Selection(u8 rates)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR1);
	if (rates == QMC_RATES_10HZ) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xF3) | QMC_RATES_10HZ);
	}
	else if (rates == QMC_RATES_50HZ) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xF3) | QMC_RATES_50HZ);
	}
	else if (rates == QMC_RATES_100HZ) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xF3) | QMC_RATES_100HZ);
	}
	else if (rates == QMC_RATES_200HZ) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xF3) | QMC_RATES_200HZ);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Rates_Selection_Freq(u8 Freq)
 @Description			QMC5883L数据输出速率选择
 @Input				0, 1, 2, 3 : QMC_RATES_10HZ/QMC_RATES_50HZ/QMC_RATES_100HZ/QMC_RATES_200HZ
 @Return				void
**********************************************************************************************************/
void QMC5883L_Rates_Selection_Freq(u8 Freq)
{
	if (Freq == 0) {
		QMC5883L_Rates_Selection(QMC_RATES_10HZ);
	}
	else if (Freq == 1) {
		QMC5883L_Rates_Selection(QMC_RATES_50HZ);
	}
	else if (Freq == 2) {
		QMC5883L_Rates_Selection(QMC_RATES_100HZ);
	}
	else if (Freq == 3) {
		QMC5883L_Rates_Selection(QMC_RATES_200HZ);
	}
	else {
		QMC5883L_Rates_Selection(QMC_RATES_50HZ);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Range_Selection(u8 range)
 @Description			QMC5883L测量范围选择
 @Input				range : QMC_RANGE_2G/QMC_RANGE_8G
 @Return				void
**********************************************************************************************************/
void QMC5883L_Range_Selection(u8 range)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR1);
	if (range == QMC_RANGE_2G) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xCF) | QMC_RANGE_2G);
	}
	else if (range == QMC_RANGE_8G) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xCF) | QMC_RANGE_8G);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Osr_Selection(u8 osr)
 @Description			QMC5883L内置滤波器带宽选择
 @Input				osr : QMC_OSR_512/QMC_OSR_256/QMC_OSR_128/QMC_OSR_64
 @Return				void
**********************************************************************************************************/
void QMC5883L_Osr_Selection(u8 osr)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR1);
	if (osr == QMC_OSR_512) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0x3F) | QMC_OSR_512);
	}
	else if (osr == QMC_OSR_256) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0x3F) | QMC_OSR_256);
	}
	else if (osr == QMC_OSR_128) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0x3F) | QMC_OSR_128);
	}
	else if (osr == QMC_OSR_64) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0x3F) | QMC_OSR_64);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Interrupt_Selection(u8 interrupt)
 @Description			QMC5883L引脚中断选择
 @Input				interrupt : QMC_INT_ENABLE/QMC_INT_DISABLE
 @Return				void
**********************************************************************************************************/
void QMC5883L_Interrupt_Selection(u8 interrupt)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR2);
	if (interrupt == QMC_INT_ENABLE) {
		QMC5883L_WriteByte(QMC5883L_CR2, (reg & 0xFE) | QMC_INT_ENABLE);
	}
	else if (interrupt == QMC_INT_DISABLE) {
		QMC5883L_WriteByte(QMC5883L_CR2, (reg & 0xFE) | QMC_INT_DISABLE);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Softwart_Reset(void)
 @Description			QMC5883L复位
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Softwart_Reset(void)
{
	QMC5883L_WriteByte(QMC5883L_CR2, QMC_SOFT_REST);							//复位QMC5883L
	
	QMC5883L_WriteByte(0x0B, 0x01);
	QMC5883L_WriteByte(0x20, 0x40);
	QMC5883L_WriteByte(0x21, 0x01);
	
	/* 64滤波, 8高斯范围, 10Hz输出, 初始化为StandBy */
	QMC5883L_WriteByte(QMC5883L_CR1, QMC_OSR_64 | QMC_RANGE_8G | QMC_RATES_10HZ | QMC_MODE_STANDBY);
	/* 引脚中断使能, 数据读取完指针自动偏转失能 */
	QMC5883L_WriteByte(QMC5883L_CR2, QMC_INT_ENABLE | QMC_POINT_ROLL_DISABLE);
}

/********************************************** END OF FLEE **********************************************/
