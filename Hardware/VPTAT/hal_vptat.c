/**
  *********************************************************************************************************
  * @file    hal_vptat.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-04-02
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "hal_vptat.h"
#include "delay.h"

ADC_HandleTypeDef		VPTAT_ADC_Handler;									//VPTAT电压ADC句柄

/**********************************************************************************************************
 @Function			void VPTAT_ADC_Init(void)
 @Description			VPTAT电压ADC初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void VPTAT_ADC_Init(void)
{
	ADC_ChannelConfTypeDef ADC_ChanelConfig;
	
	// ADC Function Configuration
	VPTAT_ADC_Handler.Instance = ADC1;
	VPTAT_ADC_Handler.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	VPTAT_ADC_Handler.Init.Resolution = ADC_RESOLUTION_12B;
	VPTAT_ADC_Handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	VPTAT_ADC_Handler.Init.ScanConvMode = ADC_SCAN_DISABLE;
	VPTAT_ADC_Handler.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	VPTAT_ADC_Handler.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
	VPTAT_ADC_Handler.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
	VPTAT_ADC_Handler.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
	VPTAT_ADC_Handler.Init.ContinuousConvMode = DISABLE;
	VPTAT_ADC_Handler.Init.NbrOfConversion = 1;
	VPTAT_ADC_Handler.Init.DiscontinuousConvMode = DISABLE;
	VPTAT_ADC_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	VPTAT_ADC_Handler.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	VPTAT_ADC_Handler.Init.DMAContinuousRequests = DISABLE;
	if (HAL_ADC_Init(&VPTAT_ADC_Handler) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
		return ;
	}
	
	// ADC Channel Configuration
	ADC_ChanelConfig.Channel = ADC_CHANNEL_21;
	ADC_ChanelConfig.Rank = ADC_REGULAR_RANK_1;
	ADC_ChanelConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;
	if (HAL_ADC_ConfigChannel(&VPTAT_ADC_Handler, &ADC_ChanelConfig) != HAL_OK) {
		//_Error_Handler(__FILE__, __LINE__);
		return ;
	}
}

/**********************************************************************************************************
 @Function			u16 VPTAT_ADC_Read(u32 timeout)
 @Description			VPTAT电压ADC读取
 @Input				读出等待超时时间
 @Return				电压值
**********************************************************************************************************/
u16 VPTAT_ADC_Read(u32 timeout)
{
	u32 pwr_vol = 0;
	
	VPTAT_ADC_Init();													//VPTAT电压ADC初始化
	
	Delay_MS(5);
	
	HAL_ADC_Start(&VPTAT_ADC_Handler);
	
	HAL_ADC_PollForConversion(&VPTAT_ADC_Handler, timeout);
	
	pwr_vol = HAL_ADC_GetValue(&VPTAT_ADC_Handler);
	
	HAL_ADC_Stop(&VPTAT_ADC_Handler);
	
	HAL_ADC_DeInit(&VPTAT_ADC_Handler);
	
	return pwr_vol * 100 * 2.8 / 4096.0;
}

/********************************************** END OF FLEE **********************************************/
