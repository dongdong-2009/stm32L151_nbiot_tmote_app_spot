#ifndef __HAL_VPTAT_ADC_H
#define   __HAL_VPTAT_ADC_H

#include "sys.h"

#define VPTAT_ADC_GPIOx					GPIOB
#define VPTAT_ADC_PIN					GPIO_PIN_15
#define VPTAT_ADC_RCC_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()
#define VPTAT_ADC_RCC_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOB_CLK_DISABLE()
#define VPTAT_ADC_RCC_ADC1_CLK_ENABLE()		__HAL_RCC_ADC1_CLK_ENABLE()
#define VPTAT_ADC_RCC_ADC1_CLK_DISABLE()	__HAL_RCC_ADC1_CLK_DISABLE()

extern ADC_HandleTypeDef		VPTAT_ADC_Handler;								//VPTAT电压ADC句柄

void VPTAT_ADC_Init(void);												//VPTAT电压ADC初始化
u16  VPTAT_ADC_Read(u32 timeout);											//VPTAT电压ADC读取

#endif
