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

#include "radar_dac.h"

DAC_HandleTypeDef RADAR_DAC_Handler;										//Radar DAC句柄

/**********************************************************************************************************
 @Function			void Radar_DAC_Init(void)
 @Description			雷达DAC初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_DAC_Init(void)
{
	DAC_ChannelConfTypeDef DAC_ChanelConfig;
	
	/* DAC初始化 */
	RADAR_DAC_Handler.Instance = RADAR_DACx;
	HAL_DAC_Init(&RADAR_DAC_Handler);
	
	/* DAC通道输出配置 */
	DAC_ChanelConfig.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
	DAC_ChanelConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	HAL_DAC_ConfigChannel(&RADAR_DAC_Handler, &DAC_ChanelConfig, RADAR_DACx_CHANNEL);
}

/**********************************************************************************************************
 @Function			void Radar_DAC_DeInit(void)
 @Description			雷达DAC复位
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_DAC_DeInit(void)
{
	HAL_DAC_DeInit(&RADAR_DAC_Handler);
}

/********************************************** END OF FLEE **********************************************/
