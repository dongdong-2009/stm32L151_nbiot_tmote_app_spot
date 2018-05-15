/**
  *********************************************************************************************************
  * @file    stm32l1xx_it.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-23
  * @brief   中断函数
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "stm32l1xx_it.h"
#include "usart.h"
#include "hal_rtc.h"
#include "radar_api.h"
#include "radar_adc.h"
#include "radar_dac.h"
#include "radar_timer.h"
#include "radio_hal_rf.h"
#include "nbiotconfig.h"

/**********************************************************************************************************
 @Function			void TIM2_IRQHandler(void)
 @Description			定时器2中断服务函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&RADAR_TIM2_Handler);								//调用HAL库TIM2中断处理公共函数
}

/**********************************************************************************************************
 @Function			void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
 @Description			定时器中断处理回调函数
					在HAL库中所有定时器中断服务函数都会调用此函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		if (RADAR_DAC_Handler.Instance) {
			Radar_GetSample_Time();										//雷达采样节拍
		}
	}
}


/**********************************************************************************************************
 @Function			void USART1_IRQHandler(void)
 @Description			串口1中断服务程序
 @Input				void
 @Return				void
**********************************************************************************************************/
void USART1_IRQHandler(void)
{
//	u32 timeout = 0;
	
	HAL_UART_IRQHandler(&UART1_Handler);									//调用HAL库中断处理公用函数
	
//	timeout = 0;
//	while (HAL_UART_GetState(&UART1_Handler) != HAL_UART_STATE_READY) {			//等待就绪
//		timeout++;													//超时处理
//		if (timeout > HAL_MAX_DELAY) break;
//	}
//	
//	timeout = 0;
//	/* 一次处理完成之后, 重新开启中断并设置RxXferCount为1 */
//	while (HAL_UART_Receive_IT(&UART1_Handler, (u8 *)USART1_aRxBuffer, USART1_RXBUFFERSIZE) != HAL_OK) {
//		timeout++;													//超时处理
//		if (timeout > HAL_MAX_DELAY) break;
//	}
	
	if (UART1_Handler.RxXferCount == 0) {
		UART1_Handler.pRxBuffPtr = (u8 *)USART1_aRxBuffer;
		UART1_Handler.RxXferCount = USART1_RXBUFFERSIZE;
		UART1_Handler.RxXferSize = USART1_RXBUFFERSIZE;
	}
	if (UART1_Handler.State == HAL_UART_STATE_BUSY_TX) {
		UART1_Handler.State = HAL_UART_STATE_BUSY_TX_RX;
	}
	else {
		UART1_Handler.State = HAL_UART_STATE_BUSY_RX;
	}
}

/**********************************************************************************************************
 @Function			void USART2_IRQHandler(void)
 @Description			串口2中断服务程序
 @Input				void
 @Return				void
**********************************************************************************************************/
void USART2_IRQHandler(void)
{
//	u32 timeout = 0;
	
	HAL_UART_IRQHandler(&UART2_Handler);									//调用HAL库中断处理公用函数
	
//	timeout = 0;
//	while (HAL_UART_GetState(&UART2_Handler) != HAL_UART_STATE_READY) {			//等待就绪
//		timeout++;													//超时处理
//		if (timeout > HAL_MAX_DELAY) break;
//	}
//	
//	timeout = 0;
//	/* 一次处理完成之后, 重新开启中断并设置RxXferCount为1 */
//	while (HAL_UART_Receive_IT(&UART2_Handler, (u8 *)USART2_aRxBuffer, USART2_RXBUFFERSIZE) != HAL_OK) {
//		timeout++;													//超时处理
//		if (timeout > HAL_MAX_DELAY) break;
//	}
	
	if (UART2_Handler.RxXferCount == 0) {
		UART2_Handler.pRxBuffPtr = (u8 *)USART2_aRxBuffer;
		UART2_Handler.RxXferCount = USART2_RXBUFFERSIZE;
		UART2_Handler.RxXferSize = USART2_RXBUFFERSIZE;
	}
	if (UART2_Handler.State == HAL_UART_STATE_BUSY_TX) {
		UART2_Handler.State = HAL_UART_STATE_BUSY_TX_RX;
	}
	else {
		UART2_Handler.State = HAL_UART_STATE_BUSY_RX;
	}
}

/**********************************************************************************************************
 @Function			void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 @Description			串口接收中断处理回调函数
					在HAL库中所有串口接收中断服务函数都会调用此函数
 @Input				huart : UART句柄
 @Return				void
**********************************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)										//如果是串口1
	{
		if ((USART1_RX_STA & 0x8000) == 0) {								//接收未完成
			if (USART1_RX_STA & 0x4000) {									//接收到了0x0D
				if (USART1_aRxBuffer[0] != 0x0a) {							//还未接收到\r\n
					USART1_RX_STA &= 0xBFFF;
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = USART1_aRxBuffer[0];
					USART1_RX_STA++;
					if ((USART1_RX_STA & 0X3FFF) > (USART1_REC_LEN-1)) USART1_RX_STA = 0;		//接收数据错误, 重新开始接收
				}
				else {												//接收完成了
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = USART1_aRxBuffer[0];
					USART1_RX_STA++;
					if ((USART1_RX_STA & 0X3FFF) > (USART1_REC_LEN-1)) USART1_RX_STA = 0;		//接收数据错误, 重新开始接收
					USART1_RX_STA |= 0x8000;
					
					/* NBIOT波特率计算 */
					if (NBIOTBaudRate.EnBaudRateState != false) {
						NBIOTBaudRate.NBIOTBaudRateNow.EndMs = HAL_GetTick();
						if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {
							NBIOTBaudRate.NBIOTBaudRateNow.EndMs++;
						}
						NBIOTBaudRate.NBIOTBaudRateNow.EndClock = SysTick->VAL;
					}
				}
			}
			else {													//还没收到0x0D
				if (USART1_aRxBuffer[0] == 0x0d) {
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = USART1_aRxBuffer[0];
					USART1_RX_STA++;
					if ((USART1_RX_STA & 0X3FFF) > (USART1_REC_LEN-1)) USART1_RX_STA = 0;		//接收数据错误, 重新开始接收
					USART1_RX_STA |= 0x4000;
				}
				else {
					/* NBIOT波特率计算 */
					if (NBIOTBaudRate.EnBaudRateState != false) {
						if ((USART1_RX_STA & 0X3FFF) == 0) {
							NBIOTBaudRate.NBIOTBaudRateNow.StartMs = HAL_GetTick();
							if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {
								NBIOTBaudRate.NBIOTBaudRateNow.StartMs++;
							}
							NBIOTBaudRate.NBIOTBaudRateNow.StartClock = SysTick->VAL;
						}
						else if ((USART1_RX_STA & 0X3FFF) == BAUDRATE_CAL_MIDDLE_NUM) {
							NBIOTBaudRate.NBIOTBaudRateNow.MiddleMs = HAL_GetTick();
							if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {
								NBIOTBaudRate.NBIOTBaudRateNow.MiddleMs++;
							}
							NBIOTBaudRate.NBIOTBaudRateNow.MiddleClock = SysTick->VAL;
							NBIOTBaudRate.NBIOTBaudRateNow.MiddleNum = BAUDRATE_CAL_MIDDLE_NUM;
						}
					}
					
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = USART1_aRxBuffer[0];
					USART1_RX_STA++;
					if ((USART1_RX_STA & 0X3FFF) > (USART1_REC_LEN-1)) USART1_RX_STA = 0;		//接收数据错误, 重新开始接收
				}
			}
		}
	}
	
	if (huart->Instance == USART2)										//如果是串口2
	{
		if ((USART2_RX_STA & 0x8000) == 0) {								//接收未完成
			if (USART2_RX_STA & 0x4000) {									//接收到了0x0D
				if (USART2_aRxBuffer[0] != 0x0a) {							//还未接收到\r\n
					USART2_RX_STA &= 0xBFFF;
					USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = USART2_aRxBuffer[0];
					USART2_RX_STA++;
					if ((USART2_RX_STA & 0X3FFF) > (USART2_REC_LEN-1)) USART2_RX_STA = 0;		//接收数据错误, 重新开始接收
				}
				else {												//接收完成了
					USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = USART2_aRxBuffer[0];
					USART2_RX_STA++;
					if ((USART2_RX_STA & 0X3FFF) > (USART2_REC_LEN-1)) USART2_RX_STA = 0;		//接收数据错误, 重新开始接收
					USART2_RX_STA |= 0x8000;
				}
			}
			else {													//还没收到0x0D
				if (USART2_aRxBuffer[0] == 0x0d) {
					USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = USART2_aRxBuffer[0];
					USART2_RX_STA++;
					if ((USART2_RX_STA & 0X3FFF) > (USART2_REC_LEN-1)) USART2_RX_STA = 0;		//接收数据错误, 重新开始接收
					USART2_RX_STA |= 0x4000;
				}
				else {
					USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = USART2_aRxBuffer[0];
					USART2_RX_STA++;
					if ((USART2_RX_STA & 0X3FFF) > (USART2_REC_LEN-1)) USART2_RX_STA = 0;		//接收数据错误, 重新开始接收
				}
			}
		}
	}
}


/**********************************************************************************************************
 @Function			void EXTI15_10_IRQHandler(void)
 @Description			This function handles EXTI15_10 exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(RF_nIRQ_PIN);									//调用中断处理公共函数
}

/**********************************************************************************************************
 @Function			void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 @Description			外部中断线中断处理回调函数
					在HAL库中所有外部中断服务函数都会调用此函数
 @Input				GPIO_Pin : 中断线引脚
 @Return				void
**********************************************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == RF_nIRQ_PIN) {										//RF_nIRQ_PIN 中断
		Radio_Rf_ISR();
	}
}


/**********************************************************************************************************
 @Function			void RTC_Alarm_IRQHandler(void)
 @Description			RTC闹钟中断服务函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void RTC_Alarm_IRQHandler(void)
{
	HAL_RTC_AlarmIRQHandler(&RTC_Handler);									//调用RTC闹钟中断处理公共函数
}

/**********************************************************************************************************
 @Function			void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
 @Description			RTC闹钟中断处理回调函数
 @Input				hrtc : RTC句柄
 @Return				void
**********************************************************************************************************/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	if (hrtc == &RTC_Handler) {
		
	}
}


/**********************************************************************************************************
 @Function			void RTC_WKUP_IRQHandler(void)
 @Description			RTC WAKE UP中断服务函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void RTC_WKUP_IRQHandler(void)
{
	HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler);							//调用RTC WAKE UP中断处理公共函数
}

/**********************************************************************************************************
 @Function			void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
 @Description			RTC WAKE UP中断处理回调函数
 @Input				hrtc : RTC句柄
 @Return				void
**********************************************************************************************************/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	if (hrtc == &RTC_Handler) {
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	}
}


/**********************************************************************************************************
 @Function			void NMI_Handler(void)
 @Description			This function handles NMI exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void NMI_Handler(void)
{
}

/**********************************************************************************************************
 @Function			void HardFault_Handler(void)
 @Description			This function handles Hard Fault exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while(1) {
	}
}

/**********************************************************************************************************
 @Function			void MemManage_Handler(void)
 @Description			This function handles Memory Manage exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while(1) {
	}
}

/**********************************************************************************************************
 @Function			void BusFault_Handler(void)
 @Description			This function handles Bus Fault exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while(1) {
	}
}

/**********************************************************************************************************
 @Function			void UsageFault_Handler(void)
 @Description			This function handles Usage Fault exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while(1) {
	}
}

/**********************************************************************************************************
 @Function			void SVC_Handler(void)
 @Description			This function handles SVCall exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void SVC_Handler(void)
{
}

/**********************************************************************************************************
 @Function			void DebugMon_Handler(void)
 @Description			This function handles Debug Monitor exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void DebugMon_Handler(void)
{
}

/**********************************************************************************************************
 @Function			void PendSV_Handler(void)
 @Description			This function handles PendSVC exception.
 @Input				void
 @Return				void
**********************************************************************************************************/
void PendSV_Handler(void)
{
}


/**********************************************************************************************************
 @Function			void SysTick_Handler(void)
 @Description			This function handles SysTick Handler.
 @Input				void
 @Return				void
**********************************************************************************************************/
void SysTick_Handler(void)
{
	HAL_SYSTICK_IRQHandler();
}

/**********************************************************************************************************
 @Function			void HAL_SYSTICK_Callback(void)
 @Description			系统嘀嗒定时器中断处理回调函数
					在HAL库中所有系统嘀嗒定时器中断服务函数都会调用此函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void HAL_SYSTICK_Callback(void)
{
	__IO uint32_t SystickCtrlVal = SysTick->CTRL;							//清除COUNTFLAG
	(void) SystickCtrlVal;												//丢弃SystickCtrlVal
	
	HAL_IncTick();
	Stm32_IncSecondTick();
	
	SystemSoftResetTime++;												//软重启计数器累加
	if (SystemSoftResetTime >= 100000) {									//100秒计数器溢出重启
		Stm32_System_Software_Reboot();
	}
}

/********************************************** END OF FLEE **********************************************/
