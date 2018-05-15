/**
  *********************************************************************************************************
  * @file    stm32l1xx_hal_msp.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-11-01
  * @brief   回调函数
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "sys.h"
#include "usart.h"
#include "hal_rtc.h"
#include "hal_vbat.h"
#include "hal_vptat.h"
#include "hal_temperature.h"
#include "radar_adc.h"
#include "radar_dac.h"
#include "radio_hal_rf.h"

/**********************************************************************************************************
 @Function			void HAL_MspInit(void)
 @Description			HAL_MspInit
 @Input				void
 @Return				void
**********************************************************************************************************/
void HAL_MspInit(void)
{
	
}

/**********************************************************************************************************
 @Function			void HAL_MspDeInit(void)
 @Description			HAL_MspDeInit
 @Input				void
 @Return				void
**********************************************************************************************************/
void HAL_MspDeInit(void)
{
	
}


/**********************************************************************************************************
 @Function			void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
 @Description			定时器底层驱动, 开启时钟, 设置中断优先级
					此函数会被HAL_TIM_Base_Init()函数调用
 @Input				htim : TIM句柄
 @Return				void
**********************************************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_ENABLE();										//使能TIM2时钟
		HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);								//设置中断优先级, 强占优先级1, 次优先级0
		HAL_NVIC_EnableIRQ(TIM2_IRQn);									//开启TIM2中断
	}
}

/**********************************************************************************************************
 @Function			void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
 @Description			定时器底层驱动, 关闭时钟
					此函数会被HAL_TIM_Base_DeInit()函数调用
 @Input				htim : TIM句柄
 @Return				void
**********************************************************************************************************/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_DISABLE();										//失能TIM2时钟
		HAL_NVIC_DisableIRQ(TIM2_IRQn);									//关闭TIM2中断
	}
}


/**********************************************************************************************************
 @Function			void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
 @Description			ADC底层驱动
					此函数会被HAL_ADC_Init()函数调用
 @Input				hadc : ADC句柄
 @Return				void
**********************************************************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (hadc == &RADAR_ADC_Handler) {										//雷达ADC
		/* ADC外设时钟使能 */
		RADAR_ADCx_CLK_ENABLE();
		
		/* 雷达AD转换通道引脚时钟使能 */
		RADAR_ADCx_CHANNEL_GPIO_CLK_ENABLE();
		
		/* 雷达DMA时钟使能 */
		__HAL_RCC_DMA1_CLK_ENABLE();
		
		/* 雷达ADC引脚配置 */
		GPIO_Initure.Pin = RADAR_ADCx_CHANNEL_PIN;
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(RADAR_ADCx_CHANNEL_GPIO_PORT, &GPIO_Initure);
		
		/* Peripheral DMA init*/
		RADAR_ADC_DMA_Handler.Instance					= DMA1_Channel1;
		RADAR_ADC_DMA_Handler.Init.Direction				= DMA_PERIPH_TO_MEMORY;
		RADAR_ADC_DMA_Handler.Init.PeriphInc				= DMA_PINC_DISABLE;
		RADAR_ADC_DMA_Handler.Init.MemInc					= DMA_MINC_ENABLE;
		RADAR_ADC_DMA_Handler.Init.PeriphDataAlignment		= DMA_PDATAALIGN_HALFWORD;
		RADAR_ADC_DMA_Handler.Init.MemDataAlignment			= DMA_MDATAALIGN_HALFWORD;
		RADAR_ADC_DMA_Handler.Init.Mode					= DMA_CIRCULAR;
		RADAR_ADC_DMA_Handler.Init.Priority				= DMA_PRIORITY_LOW;
		HAL_DMA_Init(&RADAR_ADC_DMA_Handler);
		
		__HAL_LINKDMA(hadc, DMA_Handle, RADAR_ADC_DMA_Handler);
	}
	
	if (hadc == &VBAT_ADC_Handler) {										//电池电压ADC
		/* Enable ADC */
		VBAT_ADC_RCC_ADC1_CLK_ENABLE();
		
		/* Enable GPIO */
		VBAT_ADC_RCC_GPIO_CLK_ENABLE();
		
		/* ADC Pin Configuration */
		GPIO_Initure.Pin = VBAT_ADC_PIN;
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(VBAT_ADC_GPIOx, &GPIO_Initure);
	}
	
	if (hadc == &VPTAT_ADC_Handler) {										//VPTAT电压ADC
		/* Enable ADC */
		VPTAT_ADC_RCC_ADC1_CLK_ENABLE();
		
		/* Enable GPIO */
		VPTAT_ADC_RCC_GPIO_CLK_ENABLE();
		
		/* ADC Pin Configuration */
		GPIO_Initure.Pin = VPTAT_ADC_PIN;
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(VPTAT_ADC_GPIOx, &GPIO_Initure);
	}
	
	if (hadc == &TEMPERATURE_ADC_Handler) {									//内部温度传感器ADC
		/* Enable ADC */
		TEMPERATURE_ADC_RCC_ADC1_CLK_ENABLE();
	}
}

/**********************************************************************************************************
 @Function			void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
 @Description			ADC底层驱动
					此函数会被HAL_ADC_DeInit()函数调用
 @Input				hadc : ADC句柄
 @Return				void
**********************************************************************************************************/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
	if (hadc == &RADAR_ADC_Handler) {										//雷达ADC
		/* Disable ADC */
		RADAR_ADCx_CLK_DISABLE();
		
		/* Deinitialize ADC GPIO */
		HAL_GPIO_DeInit(RADAR_ADCx_CHANNEL_GPIO_PORT, RADAR_ADCx_CHANNEL_PIN);
		
		/* Deinitialize ADC DMA */
		HAL_DMA_DeInit(hadc->DMA_Handle);
	}
	
	if (hadc == &VBAT_ADC_Handler) {										//电池电压ADC
		/* Disable ADC */
		VBAT_ADC_RCC_ADC1_CLK_DISABLE();
		
		/* Deinitialize ADC GPIO */
		HAL_GPIO_DeInit(VBAT_ADC_GPIOx, VBAT_ADC_PIN);
		
		/* Deinitialize ADC DMA */
		HAL_DMA_DeInit(hadc->DMA_Handle);
	}
	
	if (hadc == &VPTAT_ADC_Handler) {										//VPTAT电压ADC
		/* Disable ADC */
		VPTAT_ADC_RCC_ADC1_CLK_DISABLE();
		
		/* Deinitialize ADC GPIO */
		HAL_GPIO_DeInit(VPTAT_ADC_GPIOx, VPTAT_ADC_PIN);
		
		/* Deinitialize ADC DMA */
		HAL_DMA_DeInit(hadc->DMA_Handle);
	}
	
	if (hadc == &TEMPERATURE_ADC_Handler) {									//内部温度传感器ADC
		/* Disable ADC */
		TEMPERATURE_ADC_RCC_ADC1_CLK_DISABLE();
	}
}


/**********************************************************************************************************
 @Function			void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
 @Description			DAC底层驱动
					此函数会被HAL_DAC_Init()函数调用
 @Input				hdac : DAC句柄
 @Return				void
**********************************************************************************************************/
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (hdac == &RADAR_DAC_Handler) {										//雷达DAC
		/* DAC外设时钟使能 */
		RADAR_DACx_CLK_ENABLE();
		
		/* DAC通道引脚端口时钟使能 */
		RADAR_DACx_CHANNEL_GPIO_CLK_ENABLE();
		
		/* DAC通道引脚配置 */
		GPIO_Initure.Pin = RADAR_DACx_CHANNEL_PIN;
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(RADAR_DACx_CHANNEL_GPIO_PORT, &GPIO_Initure);
	}
}

/**********************************************************************************************************
 @Function			void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
 @Description			DAC底层驱动
					此函数会被HAL_DAC_DeInit()函数调用
 @Input				hdac : DAC句柄
 @Return				void
**********************************************************************************************************/
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{
	if (hdac == &RADAR_DAC_Handler) {										//雷达DAC
		/* 复位DAC */
		RADAR_DACx_FORCE_RESET();
		RADAR_DACx_RELEASE_RESET();
		
		/* 禁用DAC外设时钟 */
		RADAR_DACx_CLK_DISABLE();
		
		/* 反初始化DAC通道引脚 */
		HAL_GPIO_DeInit(RADAR_DACx_CHANNEL_GPIO_PORT, RADAR_DACx_CHANNEL_PIN);
		
		/* DMA控制器反初始化 */
		HAL_DMA_DeInit(hdac->DMA_Handle2);
	}
}


/**********************************************************************************************************
 @Function			void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
 @Description			RTC底层驱动, 时钟配置
					此函数会被HAL_RTC_Init()函数调用
 @Input				hrtc : RTC句柄
 @Return				void
**********************************************************************************************************/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
	if (hrtc->Instance == RTC) {
		RCC_OscInitTypeDef RCC_OscInitStruct;
		RCC_PeriphCLKInitTypeDef PeriphCLKInitStruct;
		
		__HAL_RCC_PWR_CLK_ENABLE();										//使能电源时钟PWR
		HAL_PWR_EnableBkUpAccess();										//取消备份区域写保护
		
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;				//LSE配置
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;						//DISABLE PLL
		RCC_OscInitStruct.LSEState = RCC_LSE_ON;							//RTC使用LSE
		HAL_RCC_OscConfig(&RCC_OscInitStruct);
		
		PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;			//外设为RTC
		PeriphCLKInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;			//RTC时钟源为LSE
		HAL_RCCEx_PeriphCLKConfig(&PeriphCLKInitStruct);
		
		__HAL_RCC_RTC_ENABLE();											//RTC时钟使能
		
		HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
	}
}

/**********************************************************************************************************
 @Function			void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
 @Description			RTC底层驱动, 时钟关闭
					此函数会被HAL_RTC_DeInit()函数调用
 @Input				hrtc : RTC句柄
 @Return				void
**********************************************************************************************************/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
	if (hrtc->Instance == RTC) {
		__HAL_RCC_PWR_CLK_DISABLE();										//失能电源时钟PWR
		__HAL_RCC_RTC_DISABLE();											//RTC时钟失能
		HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
	}
}


/**********************************************************************************************************
 @Function			void HAL_UART_MspInit(UART_HandleTypeDef *huart)
 @Description			UART底层初始化, 时钟使能, 引脚配置, 中断配置
					此函数会被HAL_UART_Init()调用
					USART1_TX : PA9
					USART1_RX : PA10
					USART2_TX : PA2
					USART2_RX : PA3
 @Input				huart:串口句柄
 @Return				void
**********************************************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (huart->Instance == USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();										//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();									//使能USART1时钟
		
		GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10;							//PA9|PA10
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;								//复用推挽输出
		GPIO_Initure.Pull = GPIO_PULLUP;									//上拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;							//高速
		GPIO_Initure.Alternate = GPIO_AF7_USART1;							//复用为串口1
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);								//初始化PA9|PA10
		
		HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);							//抢占优先级1, 子优先级0
		HAL_NVIC_EnableIRQ(USART1_IRQn);									//使能USART1中断通道
	}
	
	if (huart->Instance == USART2)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();										//使能GPIOA时钟
		__HAL_RCC_USART2_CLK_ENABLE();									//使能USART2时钟
		
		GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3;							//PA2|PA3
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;								//复用推挽输出
		GPIO_Initure.Pull = GPIO_PULLUP;									//上拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;							//高速
		GPIO_Initure.Alternate = GPIO_AF7_USART2;							//复用为串口2
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);								//初始化PA2|PA3
		
		HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);							//抢占优先级1, 子优先级1
		HAL_NVIC_EnableIRQ(USART2_IRQn);									//使能USART2中断通道
	}
}

/**********************************************************************************************************
 @Function			void HAL_UART_MspInit(UART_HandleTypeDef *huart)
 @Description			UART底层恢复初始化, 时钟失能, 引脚复位, 中断关闭
					此函数会被HAL_UART_DeInit()调用
					USART1_TX : PA9
					USART1_RX : PA10
					USART2_TX : PA2
					USART2_RX : PA3
 @Input				huart:串口句柄
 @Return				void
**********************************************************************************************************/
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (huart->Instance == USART1)
	{
		__HAL_RCC_USART1_CLK_DISABLE();									//失能USART1时钟
		
		GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10;							//PA9|PA10
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);								//初始化PA9|PA10
		
		HAL_NVIC_DisableIRQ(USART1_IRQn);									//失能USART1中断通道
	}
	
	if (huart->Instance == USART2)
	{
		__HAL_RCC_USART2_CLK_DISABLE();									//失能USART2时钟
		
		GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3;							//PA2|PA3
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);								//初始化PA2|PA3
		
		HAL_NVIC_DisableIRQ(USART2_IRQn);									//失能USART2中断通道
	}
}


/**********************************************************************************************************
 @Function			void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
 @Description			SPI底层驱动, 时钟配置
					此函数会被HAL_SPI_Init()函数调用
 @Input				hspi : SPI句柄
 @Return				void
**********************************************************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (hspi->Instance == SPIx)
	{
		SPIx_NSS_GPIO_CLK_ENABLE();
		SPIx_SCK_GPIO_CLK_ENABLE();
		SPIx_MOSI_GPIO_CLK_ENABLE();
		SPIx_MISO_GPIO_CLK_ENABLE();
		
		SPIx_RCC_CLK_ENABLE();
		
		/* SPI1_NSS */
		GPIO_Initure.Pin		= SPIx_NSS_PIN;
		GPIO_Initure.Mode		= GPIO_MODE_OUTPUT_PP;
		GPIO_Initure.Speed		= GPIO_SPEED_HIGH;
		HAL_GPIO_Init(SPIx_NSS_GPIO_PORT, &GPIO_Initure);
		
		/* SPI1_SCK */
		GPIO_Initure.Pin		= SPIx_SCK_PIN;
		GPIO_Initure.Mode		= GPIO_MODE_AF_PP;
		GPIO_Initure.Pull		= GPIO_PULLDOWN;
		GPIO_Initure.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_Initure.Alternate	= SPIx_SCK_AF;
		HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_Initure);
		
		/* SPI1_MOSI */
		GPIO_Initure.Pin		= SPIx_MOSI_PIN;
		GPIO_Initure.Alternate	= SPIx_MOSI_AF;
		HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_Initure);
		
		/* SPI1_MISO */
		GPIO_Initure.Pin		= SPIx_MISO_PIN;
		GPIO_Initure.Alternate	= SPIx_MISO_AF;
		HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_Initure);
	}
}

/**********************************************************************************************************
 @Function			void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
 @Description			SPI底层驱动, 时钟关闭
					此函数会被HAL_SPI_DeInit()函数调用
 @Input				hspi : SPI句柄
 @Return				void
**********************************************************************************************************/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
	if (hspi->Instance == SPIx)
	{
		SPIx_FORCE_RESET();
		SPIx_RELEASE_RESET();
		
		HAL_GPIO_DeInit(SPIx_NSS_GPIO_PORT, SPIx_NSS_PIN);
		HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
		HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
		HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
	}
}

/********************************************** END OF FLEE **********************************************/
