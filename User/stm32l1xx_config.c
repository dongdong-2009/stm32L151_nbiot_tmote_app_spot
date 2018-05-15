/**
  *********************************************************************************************************
  * @file    stm32l1xx_config.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-11-01
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "stm32l1xx_config.h"
#include "platform_config.h"
#include "platform_map.h"
#include "hal_rtc.h"
#include "hal_iic.h"
#include "hal_qmc5883l.h"
#include "radio_hal_rf.h"
#include "radio_rf_app.h"
#include "radio_hal.h"
#include "radar_adc.h"
#include "radar_dac.h"
#include "radar_dma.h"
#include "tmesh_xtea.h"
#include "delay.h"

RCC_RESET_FLAG_TypeDef SoftResetFlag = RCC_RESET_FLAG_NONE;

signed int TimeToInitModule = 0;
unsigned short TimeToReinitModuleCnt = 0;

extern __IO uint32_t uwTick;

/**********************************************************************************************************
 @Function			RCC_RESET_FLAG_TypeDef RCC_ResetFlag_GetStatus(void)
 @Description			RCC_ResetFlag_GetStatus	: 获取复位标志位
 @Input				void
 @Return				RCC_RESET_FLAG_TypeDef	: 标志位
**********************************************************************************************************/
RCC_RESET_FLAG_TypeDef RCC_ResetFlag_GetStatus(void)
{
	RCC_RESET_FLAG_TypeDef ResetStatus = RCC_RESET_FLAG_NONE;
	
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_IWDGRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_SFTRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_PORRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_PINRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_LPWRRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_OBLRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_WWDGRST;
	}
	
	return ResetStatus;
}

/**********************************************************************************************************
 @Function			void PowerCtrlIO_Init(void)
 @Description			低功耗控制IO初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void LowPowerCtrlIO_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	
	GPIO_Initure.Pin = GPIOA_PIN;
	GPIO_Initure.Mode = GPIO_MODE_ANALOG;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOB_PIN;
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOC_PIN;
	HAL_GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOD_PIN;
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOE_PIN;
	HAL_GPIO_Init(GPIOE, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOH_PIN;
	HAL_GPIO_Init(GPIOH, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLDOWN;
	GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
}

/**********************************************************************************************************
 @Function			static void ModelPower_Init(void)
 @Description			Model Power 初始化 PA8 : 0 导通 1 : 截止
 @Input				void
 @Return				void
**********************************************************************************************************/
static void ModelPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	MODEL_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = MODEL_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//高速
	HAL_GPIO_Init(MODEL_POWER_GPIOx, &GPIO_Initure);										//初始化GPIO
	
	MODELPOWER(OFF);																//初始化关闭电源
}

/**********************************************************************************************************
 @Function			static void RadarPower_Init(void)
 @Description			Radar Power 初始化 PB3 : 1 导通 0 : 截止
 @Input				void
 @Return				void
**********************************************************************************************************/
static void RadarPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	RADAR_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = RADAR_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//高速
	HAL_GPIO_Init(RADAR_POWER_GPIOx, &GPIO_Initure);										//初始化GPIO
	
	RADARPOWER(OFF);
}

/**********************************************************************************************************
 @Function			static void NBIOTPower_Init(void)
 @Description			NBIOT Power 初始化 PA0 : 1 导通 0 : 截止
 @Input				void
 @Return				void
**********************************************************************************************************/
static void NBIOTPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	NBIOT_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = NBIOT_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//高速
	HAL_GPIO_Init(NBIOT_POWER_GPIOx, &GPIO_Initure);										//初始化GPIO
	
	NBIOTPOWER(OFF);
}

/**********************************************************************************************************
 @Function			static void VbatPower_Init(void)
 @Description			VBAT Power 初始化 PA12 : 0 导通 1 : 截止
 @Input				void
 @Return				void
**********************************************************************************************************/
static void VbatPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	VBAT_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = VBAT_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//高速
	HAL_GPIO_Init(VBAT_POWER_GPIOx, &GPIO_Initure);										//初始化GPIO
	
	VBATPOWER(OFF);
}

/**********************************************************************************************************
 @Function			void ModulePowerReset_Init(void)
 @Description			模块复位电源控制
 @Input				void
 @Return				void
**********************************************************************************************************/
void ModulePowerReset_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	ModelPower_Init();																//模块电源初始化
	RadarPower_Init();																//雷达电源初始化
	NBIOTPower_Init();																//NBIOT电源初始化
	VbatPower_Init();																//电压检测电源初始化
	
	MODELPOWER(OFF);																//关闭模块总电源
	RADARPOWER(OFF);																//关闭雷达电源
	NBIOTPOWER(OFF);																//关闭NBIOT电源
	VBATPOWER(OFF);																//关闭电源电压检测电源
	
	IIC_SCL_RCC_GPIO_CLK_ENABLE();
	IIC_SDA_RCC_GPIO_CLK_ENABLE();
	SPIx_NSS_GPIO_CLK_ENABLE();
	SPIx_SCK_GPIO_CLK_ENABLE();
	SPIx_MOSI_GPIO_CLK_ENABLE();
	SPIx_MISO_GPIO_CLK_ENABLE();
	RF_SDN_GPIO_CLK_ENABLE();
	RF_nIRQ_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = IIC_SCL_PIN;													//IIC_SCL
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(IIC_SCL_GPIOx, &GPIO_Initure);
	
	GPIO_Initure.Pin = IIC_SDA_PIN;													//IIC_SDA
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(IIC_SDA_GPIOx, &GPIO_Initure);
	
	GPIO_Initure.Pin = SPIx_NSS_PIN;													//SPI_NSS
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(SPIx_NSS_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin = SPIx_SCK_PIN;													//SPI_SCK
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin = SPIx_MOSI_PIN;													//SPI_MOSI
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin = SPIx_MISO_PIN;													//SPI_MISO
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin = RF_SDN_PIN;													//RF_SDN
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(RF_SDN_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin = RF_nIRQ_PIN;													//RF_nIRQ
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;											//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;												//HIGH
	HAL_GPIO_Init(RF_nIRQ_GPIO_PORT, &GPIO_Initure);
	
	HAL_GPIO_WritePin(IIC_SCL_GPIOx, IIC_SCL_PIN, GPIO_PIN_RESET);							//IIC_SCL
	HAL_GPIO_WritePin(IIC_SDA_GPIOx, IIC_SDA_PIN, GPIO_PIN_RESET);							//IIC_SDA
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT, SPIx_NSS_PIN, GPIO_PIN_RESET);						//SPI_NSS
	HAL_GPIO_WritePin(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN, GPIO_PIN_RESET);						//SPI_SCK
	HAL_GPIO_WritePin(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN, GPIO_PIN_RESET);					//SPI_MOSI
	HAL_GPIO_WritePin(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN, GPIO_PIN_RESET);					//SPI_MISO
	HAL_GPIO_WritePin(RF_SDN_GPIO_PORT, RF_SDN_PIN, GPIO_PIN_RESET);							//RF_SDN
	HAL_GPIO_WritePin(RF_nIRQ_GPIO_PORT, RF_nIRQ_PIN, GPIO_PIN_RESET);						//RF_nIRQ
	Delay_MS(3000);																//断电3秒
}

/**********************************************************************************************************
 @Function			void PowerCtrlIO_Init(void)
 @Description			电源控制IO初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void PowerCtrlIO_Init(void)
{
	ModelPower_Init();																//模块电源初始化
	RadarPower_Init();																//雷达电源初始化
	NBIOTPower_Init();																//NBIOT电源初始化
	VbatPower_Init();																//电压检测电源初始化
	
	MODELPOWER(ON);																//开启模块总电源
	RADARPOWER(OFF);																//关闭雷达电源
	NBIOTPOWER(OFF);																//关闭NBIOT电源
	VBATPOWER(OFF);																//关闭电源电压检测电源
}

/**********************************************************************************************************
 @Function			void LowPowerEnterStop(void)
 @Description			进入低功耗stop模式
 @Input				void
 @Return				void
**********************************************************************************************************/
void LowPowerEnterStop(void)
{
	RADARPOWER(OFF);
	
	Radar_DMA_DeInit();
	Radar_ADC_DeInit();
	Radar_DAC_DeInit();
	
	__HAL_RCC_PWR_CLK_ENABLE();
	SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
	
	HAL_RTCEx_DeactivateWakeUpTimer(&RTC_Handler);
	HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler, 0x800, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
	
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	
	uwTick += 1000;
	Stm32_IncSecondTick();
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == SYSTEMCLOCKMSI)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);
	Delay_Init(4194);
#elif (SYSTEMCLOCK == SYSTEMCLOCKHSI)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);
	Delay_Init(32000);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
}

/**********************************************************************************************************
 @Function			void LowPowerBeforeSleepInit(void)
 @Description			进入低功耗stop模式之前初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void LowPowerBeforeSleepInit(void)
{
#ifdef RADIO_SI4438
	if (Radio_Rf_is_Sleep() == 0) {
		Radio_Rf_Sleep();
		Radio_Rf_Interface_Deinit();
	}
#endif
	QMC5883L_Drdy_DeInit();
}

/**********************************************************************************************************
 @Function			void LowPowerAfterSleepInit(void)
 @Description			进入低功耗stop模式之后初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void LowPowerAfterSleepInit(void)
{
	if (Stm32_GetSecondTick() >= (TimeToInitModule + TIME_TO_MODULE_INIT)) {
#ifdef RADIO_SI4438
		Radio_Rf_Interrupt_Deinit();
		Radio_Hal_DeassertNsel();
#endif
		MODELPOWER(OFF);
		QMC5883L_Discharge();
		MODELPOWER(ON);
		QMC5883L_Init();
#ifdef RADIO_SI4438
		Radio_Rf_Init();
#endif
		TimeToInitModule = Stm32_GetSecondTick();
	}
	QMC5883L_ClearInsideData();
	QMC5883L_Drdy_Init();
}

/**********************************************************************************************************
 @Function			void ReInitModule(void)
 @Description			reinit the qmc5883 and other module
 @Input				void
 @Return				void
**********************************************************************************************************/
void ReInitModule(void)
{
	TimeToInitModule = 0 - TIME_TO_MODULE_INIT;
	TimeToReinitModuleCnt++;
}

/**********************************************************************************************************
 @Function			unsigned short ReInitModuleGetCnt(void)
 @Description			ReInitModuleGetCnt
 @Input				void
 @Return				void
**********************************************************************************************************/
unsigned short ReInitModuleGetCnt(void)
{
	return TimeToReinitModuleCnt;
}

/********************************************** END OF FLEE **********************************************/
