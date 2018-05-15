/**
  *********************************************************************************************************
  * @file    radar_adc.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-31
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "radar_adc.h"

__IO uint16_t RADAR_ADC_ConvertedValue = 10;

ADC_HandleTypeDef RADAR_ADC_Handler;										//RADAR ADC句柄
DMA_HandleTypeDef RADAR_ADC_DMA_Handler;									//RADAR ADC DMA句柄

/**********************************************************************************************************
 @Function			void Radar_ADC_Init(void)
 @Description			雷达ADC初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_ADC_Init(void)
{
	ADC_ChannelConfTypeDef ADC_ChanelConfig;
	
	RADAR_ADC_Handler.Instance = RADAR_ADCx;
	RADAR_ADC_Handler.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
	RADAR_ADC_Handler.Init.Resolution = ADC_RESOLUTION12b;
	RADAR_ADC_Handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	RADAR_ADC_Handler.Init.ScanConvMode = ADC_SCAN_DISABLE;
	RADAR_ADC_Handler.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	RADAR_ADC_Handler.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
	RADAR_ADC_Handler.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
	RADAR_ADC_Handler.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	RADAR_ADC_Handler.Init.ContinuousConvMode = ENABLE;
	RADAR_ADC_Handler.Init.NbrOfConversion = 1;
	RADAR_ADC_Handler.Init.DiscontinuousConvMode = DISABLE;
	RADAR_ADC_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	RADAR_ADC_Handler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	RADAR_ADC_Handler.Init.DMAContinuousRequests = ENABLE;
	HAL_ADC_Init(&RADAR_ADC_Handler);
	
	ADC_ChanelConfig.Channel = ADC_CHANNEL_1;
	ADC_ChanelConfig.Rank = ADC_REGULAR_RANK_1;
	ADC_ChanelConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
	HAL_ADC_ConfigChannel(&RADAR_ADC_Handler, &ADC_ChanelConfig);
	
	/* 启动AD转换并使能DMA传输和中断 */
	HAL_ADC_Start_DMA(&RADAR_ADC_Handler, (uint32_t*)&RADAR_ADC_ConvertedValue, 1);
}

/**********************************************************************************************************
 @Function			void Radar_ADC_DeInit(void)
 @Description			雷达ADC复位
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_ADC_DeInit(void)
{
	HAL_ADC_DeInit(&RADAR_ADC_Handler);
}

/********************************************** END OF FLEE **********************************************/
