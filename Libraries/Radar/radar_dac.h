#ifndef __RADAR_DAC_H
#define   __RADAR_DAC_H

#include "sys.h"

#define RADAR_DACx							DAC
#define RADAR_DACx_CHANNEL_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define RADAR_DACx_CHANNEL_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOA_CLK_DISABLE()
#define RADAR_DACx_CLK_ENABLE()				__HAL_RCC_DAC_CLK_ENABLE()
#define RADAR_DACx_CLK_DISABLE()				__HAL_RCC_DAC_CLK_DISABLE()
#define RADAR_DACx_FORCE_RESET()				__HAL_RCC_DAC_FORCE_RESET()
#define RADAR_DACx_RELEASE_RESET()				__HAL_RCC_DAC_RELEASE_RESET()

/* 定义DAC通道引脚 */
#define RADAR_DACx_CHANNEL_GPIO_PORT			GPIOA
#define RADAR_DACx_CHANNEL_PIN				GPIO_PIN_4

#define RADAR_DACx_DMAx_CHANNELn				DMA1_Channel2
#define RADAR_DACx_DMAx_CHANNELn_IRQn			DMA1_Channel2_IRQn
#define RADAR_DACx_DMAx_CHANNELn_IRQHANDLER		DMA1_Channel2_IRQHandler

/* 定义DAC通道 */
#define RADAR_DACx_CHANNEL					DAC_CHANNEL_1

extern DAC_HandleTypeDef						RADAR_DAC_Handler;				//Radar DAC句柄

void Radar_DAC_Init(void);												//雷达DAC初始化
void Radar_DAC_DeInit(void);												//雷达DAC复位

#endif
