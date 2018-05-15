/**
  *********************************************************************************************************
  * @file    hal_switch.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-28
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "hal_switch.h"

/**********************************************************************************************************
 @Function			u8 Mercury_Read(void)
 @Description			水银开关读取状态 PH0 (Device WorkMode : 1)
 @Input				void
 @Return				GPIO_PinState
**********************************************************************************************************/
u8 Mercury_Read(void)
{
	u8 ret = 0;
	
	GPIO_InitTypeDef GPIO_Initure;

	MERCURY_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = MERCURY_PIN;
	GPIO_Initure.Mode = GPIO_MODE_INPUT;
	GPIO_Initure.Pull = GPIO_PULLUP;										//上拉
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//高速
	HAL_GPIO_Init(MERCURY_GPIOx, &GPIO_Initure);								//初始化GPIO
	
	ret = MERCURY_READ();
	
	GPIO_Initure.Pull = GPIO_PULLDOWN;										//下拉
	HAL_GPIO_Init(MERCURY_GPIOx, &GPIO_Initure);								//初始化GPIO
	
	MERCURY_RCC_GPIO_CLK_DISABLE();

//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	MERCURY_RCC_GPIO_CLK_ENABLE();
//	
//	GPIO_Initure.Pin = GPIO_PIN_9;
//	GPIO_Initure.Mode = GPIO_MODE_INPUT;
//	GPIO_Initure.Pull = GPIO_PULLUP;										//上拉
//	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//高速
//	HAL_GPIO_Init(GPIOB, &GPIO_Initure);									//初始化GPIO
//	
//	GPIO_Initure.Pin = MERCURY_PIN;
//	GPIO_Initure.Mode = GPIO_MODE_INPUT;
//	GPIO_Initure.Pull = GPIO_PULLUP;										//上拉
//	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//高速
//	HAL_GPIO_Init(MERCURY_GPIOx, &GPIO_Initure);								//初始化GPIO
//	
//	ret1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
//	ret2 = MERCURY_READ();
//	ret = ret1 && ret2;
//	
//	GPIO_Initure.Pull = GPIO_PULLDOWN;										//下拉
//	HAL_GPIO_Init(GPIOB, &GPIO_Initure);									//初始化GPIO
//	HAL_GPIO_Init(MERCURY_GPIOx, &GPIO_Initure);								//初始化GPIO
//	
//	MERCURY_RCC_GPIO_CLK_DISABLE();
	
	return ret;
}

/********************************************** END OF FLEE **********************************************/
