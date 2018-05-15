/**
  *********************************************************************************************************
  * @file    hal_temperature.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-01-06
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "hal_temperature.h"
#include "delay.h"

ADC_HandleTypeDef		TEMPERATURE_ADC_Handler;								//内部温度传感器电压ADC句柄

/**********************************************************************************************************
 @Function			void TEMPERATURE_ADC_Init(void)
 @Description			电压ADC初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void TEMPERATURE_ADC_Init(void)
{
	ADC_ChannelConfTypeDef ADC_ChanelConfig;
	
	// ADC Function Configuration
	TEMPERATURE_ADC_Handler.Instance = ADC1;
	TEMPERATURE_ADC_Handler.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	TEMPERATURE_ADC_Handler.Init.Resolution = ADC_RESOLUTION_12B;
	TEMPERATURE_ADC_Handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	TEMPERATURE_ADC_Handler.Init.ScanConvMode = ADC_SCAN_DISABLE;
	TEMPERATURE_ADC_Handler.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	TEMPERATURE_ADC_Handler.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
	TEMPERATURE_ADC_Handler.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
	TEMPERATURE_ADC_Handler.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	TEMPERATURE_ADC_Handler.Init.ContinuousConvMode = DISABLE;
	TEMPERATURE_ADC_Handler.Init.NbrOfConversion = 1;
	TEMPERATURE_ADC_Handler.Init.DiscontinuousConvMode = DISABLE;
	TEMPERATURE_ADC_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	TEMPERATURE_ADC_Handler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	TEMPERATURE_ADC_Handler.Init.DMAContinuousRequests = DISABLE;
	if (HAL_ADC_Init(&TEMPERATURE_ADC_Handler) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
		return ;
	}
	
	// ADC Channel Configuration
	ADC_ChanelConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	ADC_ChanelConfig.Rank = ADC_REGULAR_RANK_1;
	ADC_ChanelConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;
	if (HAL_ADC_ConfigChannel(&TEMPERATURE_ADC_Handler, &ADC_ChanelConfig) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
		return ;
	}
}

/**********************************************************************************************************
 @Function			short TEMPERATURE_ADC_Read(u32 timeout)
 @Description			内部温度传感器温度ADC读取
 @Input				读出等待超时时间
 @Return				温度值
**********************************************************************************************************/
short TEMPERATURE_ADC_Read(u32 timeout)
{
	u32 pwr_vol = 0;
	short temperate = 0;
	short tempMax = -100;
	short temp2nd = -100;
	char i;
	
	TEMPERATURE_ADC_RCC_ADC1_CLK_ENABLE();
	
	TEMPERATURE_ADC_Init();												//电压ADC初始化
	
	for (i = 0; i < 3; i++) {
		HAL_ADC_Start(&TEMPERATURE_ADC_Handler);
		
		HAL_ADC_PollForConversion(&TEMPERATURE_ADC_Handler, timeout);
		
		pwr_vol = HAL_ADC_GetValue(&TEMPERATURE_ADC_Handler);
		
		temperate = LL_ADC_CALC_TEMPERATURE(2800, pwr_vol);
		
		if (tempMax <= temperate) {
			temp2nd = tempMax;
			tempMax = temperate;
		}
		else if (temp2nd < temperate) {
			temp2nd = temperate;
		}
		
		HAL_ADC_Stop(&TEMPERATURE_ADC_Handler);
		
		Delay_MS(10);
	}
	
	if (HAL_ADC_DeInit(&TEMPERATURE_ADC_Handler) != HAL_OK) {
		__NOP();
	}
	
	return temp2nd;
}

/********************************************** END OF FLEE **********************************************/
