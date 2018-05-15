/**
  *********************************************************************************************************
  * @file    hal_vbat.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-11-03
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "hal_vbat.h"
#include "stm32l1xx_config.h"
#include "delay.h"

ADC_HandleTypeDef		VBAT_ADC_Handler;									//电池电压ADC句柄

/**********************************************************************************************************
 @Function			void VBAT_ADC_Init(void)
 @Description			电压ADC初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void VBAT_ADC_Init(void)
{
	ADC_ChannelConfTypeDef ADC_ChanelConfig;
	
	// ADC Function Configuration
	VBAT_ADC_Handler.Instance = ADC1;
	VBAT_ADC_Handler.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	VBAT_ADC_Handler.Init.Resolution = ADC_RESOLUTION_12B;
	VBAT_ADC_Handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	VBAT_ADC_Handler.Init.ScanConvMode = ADC_SCAN_DISABLE;
	VBAT_ADC_Handler.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	VBAT_ADC_Handler.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
	VBAT_ADC_Handler.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
	VBAT_ADC_Handler.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	VBAT_ADC_Handler.Init.ContinuousConvMode = DISABLE;
	VBAT_ADC_Handler.Init.NbrOfConversion = 1;
	VBAT_ADC_Handler.Init.DiscontinuousConvMode = DISABLE;
	VBAT_ADC_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	VBAT_ADC_Handler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	VBAT_ADC_Handler.Init.DMAContinuousRequests = DISABLE;
	if (HAL_ADC_Init(&VBAT_ADC_Handler) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
		return ;
	}
	
	// ADC Channel Configuration
	ADC_ChanelConfig.Channel = ADC_CHANNEL_9;
	ADC_ChanelConfig.Rank = ADC_REGULAR_RANK_1;
	ADC_ChanelConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;
	if (HAL_ADC_ConfigChannel(&VBAT_ADC_Handler, &ADC_ChanelConfig) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
		return ;
	}
}

/**********************************************************************************************************
 @Function			u16 VBAT_ADC_Read(u32 timeout)
 @Description			电源电压ADC读取
 @Input				读出等待超时时间
 @Return				电压值
**********************************************************************************************************/
u16 VBAT_ADC_Read(u32 timeout)
{
	u32 pwr_vol = 0;
	GPIO_InitTypeDef GPIO_Initure;
	
	VBAT_POWER_RCC_GPIO_CLK_ENABLE();
	VBAT_ADC_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = VBAT_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(VBAT_POWER_GPIOx, &GPIO_Initure);
	
	GPIO_Initure.Pin = VBAT_ADC_PIN;
	GPIO_Initure.Mode = GPIO_MODE_ANALOG;
	GPIO_Initure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(VBAT_ADC_GPIOx, &GPIO_Initure);
	
	VBAT_ADC_Init();													//电压ADC初始化
	
	VBATPOWER(ON);														//开启VBAT电源电压
	Delay_MS(5);
	
	HAL_ADC_Start(&VBAT_ADC_Handler);
	
	HAL_ADC_PollForConversion(&VBAT_ADC_Handler, timeout);
	
	pwr_vol = HAL_ADC_GetValue(&VBAT_ADC_Handler);
	
	HAL_ADC_Stop(&VBAT_ADC_Handler);
	
	VBATPOWER(OFF);													//关闭VBAT电源电压
	
	VBAT_POWER_RCC_GPIO_CLK_ENABLE();
	VBAT_ADC_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = VBAT_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_Initure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(VBAT_POWER_GPIOx, &GPIO_Initure);
	HAL_GPIO_WritePin(VBAT_POWER_GPIOx, VBAT_POWER_PIN, GPIO_PIN_SET);
	
	GPIO_Initure.Pin = VBAT_ADC_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_Initure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(VBAT_ADC_GPIOx, &GPIO_Initure);
	HAL_GPIO_WritePin(VBAT_ADC_GPIOx, VBAT_ADC_PIN, GPIO_PIN_SET);
	
	HAL_ADC_DeInit(&VBAT_ADC_Handler);
	
	return pwr_vol * 200 * 2.8 / 4096.0;
}

/********************************************** END OF FLEE **********************************************/
