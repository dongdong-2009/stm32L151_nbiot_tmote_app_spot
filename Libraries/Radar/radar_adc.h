#ifndef __RADAR_ADC_H
#define   __RADAR_ADC_H

#include "sys.h"

#define RADAR_ADCx							ADC1
#define RADAR_ADCx_CHANNEL_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define RADAR_ADCx_CHANNEL_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOA_CLK_DISABLE()
#define RADAR_ADCx_CLK_ENABLE()				__HAL_RCC_ADC1_CLK_ENABLE()
#define RADAR_ADCx_CLK_DISABLE()				__HAL_RCC_ADC1_CLK_DISABLE()

/* 定义ADC通道引脚 */
#define RADAR_ADCx_CHANNEL_GPIO_PORT			GPIOA
#define RADAR_ADCx_CHANNEL_PIN				GPIO_PIN_1

extern __IO uint16_t RADAR_ADC_ConvertedValue;

extern ADC_HandleTypeDef						RADAR_ADC_Handler;				//RADAR ADC句柄
extern DMA_HandleTypeDef 					RADAR_ADC_DMA_Handler;			//RADAR ADC DMA句柄

void Radar_ADC_Init(void);												//雷达ADC初始化
void Radar_ADC_DeInit(void);												//雷达ADC复位

#endif
