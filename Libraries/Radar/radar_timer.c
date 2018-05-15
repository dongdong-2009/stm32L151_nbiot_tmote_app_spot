/**
  *********************************************************************************************************
  * @file    radar_timer.c
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

#include "radar_timer.h"

TIM_HandleTypeDef RADAR_TIM2_Handler;										//雷达定时器2句柄

/**********************************************************************************************************
 @Function			void Radar_TIM2_Init(u16 arr, u16 psc)
 @Description			雷达通用定时器2中断初始化
 @Input				arr：自动重装值
					psc：时钟预分频数
 @Other				定时器溢出时间计算方法 : Tout = ((arr+1)*(psc+1))/Ft us.
										Ft   = 定时器工作频率,单位:Mhz
					这里使用的是定时器2!(定时器2挂在APB1上, 时钟为HCLK)
 @Return				void
**********************************************************************************************************/
void Radar_TIM2_Init(u16 arr, u16 psc)
{
	RADAR_TIM2_Handler.Instance = TIM2;									//通用定时器2
	RADAR_TIM2_Handler.Init.Prescaler = psc;								//分频数
	RADAR_TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;					//向上计数器
	RADAR_TIM2_Handler.Init.Period = arr;									//自动装载值
	RADAR_TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;				//时钟分频因子
	HAL_TIM_Base_Init(&RADAR_TIM2_Handler);
	
	HAL_TIM_Base_Start_IT(&RADAR_TIM2_Handler);								//使能定时器2和定时器2更新中断 :　TIM_IT_UPDATE
}

/**********************************************************************************************************
 @Function			void Radar_TIM2_DeInit(void)
 @Description			雷达通用定时器2反初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_TIM2_DeInit(void)
{
	HAL_TIM_Base_DeInit(&RADAR_TIM2_Handler);
}

/********************************************** END OF FLEE **********************************************/
