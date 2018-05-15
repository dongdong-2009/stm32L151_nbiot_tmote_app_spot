/**
  *********************************************************************************************************
  * @file    usart.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-23
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "usart.h"

u8 USART1_RX_BUF[USART1_REC_LEN];    										//USART1接收缓冲,最大USART1_REC_LEN个字节
u8 USART2_RX_BUF[USART2_REC_LEN];    										//USART2接收缓冲,最大USART1_REC_LEN个字节

u16 USART1_RX_STA = 0;													//USART1接收状态标记 bit15,接收完成标志 bit14,接收到0x0d(\n)
u16 USART2_RX_STA = 0;													//USART2接收状态标记 bit15,接收完成标志 bit14,接收到0x0d(\n)

u8 USART1_aRxBuffer[USART1_RXBUFFERSIZE];									//HAL库使用的串口接收缓冲
u8 USART2_aRxBuffer[USART2_RXBUFFERSIZE];									//HAL库使用的串口接收缓冲

UART_HandleTypeDef UART1_Handler;											//UART1句柄
UART_HandleTypeDef UART2_Handler;											//UART2句柄

/**********************************************************************************************************
 @Function			void Uart1_Init(u32 bound)
 @Description			串口1初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Uart1_Init(u32 bound)
{
	UART1_Handler.Instance = USART1;										//USART1
	UART1_Handler.Init.BaudRate = bound;									//波特率
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;						//字长为8位数据格式
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;							//一个停止位
	UART1_Handler.Init.Parity = UART_PARITY_NONE;							//无奇偶校验位
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;						//无硬件流控
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;								//收发模式
	UART1_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UART1_Handler) != HAL_OK) {							//HAL_UART_Init()会使能UART1
		while(1);
	}
	
	/* 该函数会开启接收中断: 标志位UART_IT_RXNE, 并且设置接收缓冲以及接收缓冲接收最大数据量 */
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)USART1_aRxBuffer, USART1_RXBUFFERSIZE);
}

/**********************************************************************************************************
 @Function			void Uart2_Init(u32 bound)
 @Description			串口2初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Uart2_Init(u32 bound)
{
	UART2_Handler.Instance = USART2;										//USART2
	UART2_Handler.Init.BaudRate = bound;									//波特率
	UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B;						//字长为8位数据格式
	UART2_Handler.Init.StopBits = UART_STOPBITS_1;							//一个停止位
	UART2_Handler.Init.Parity = UART_PARITY_NONE;							//无奇偶校验位
	UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;						//无硬件流控
	UART2_Handler.Init.Mode = UART_MODE_TX_RX;								//收发模式
	UART1_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UART2_Handler) != HAL_OK) {							//HAL_UART_Init()会使能UART2
		while (1);
	}
	
	/* 该函数会开启接收中断: 标志位UART_IT_RXNE, 并且设置接收缓冲以及接收缓冲接收最大数据量 */
	HAL_UART_Receive_IT(&UART2_Handler, (u8 *)USART2_aRxBuffer, USART2_RXBUFFERSIZE);
}

/**********************************************************************************************************
 @Function			void Uart1_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
 @Description			使能/失能 发送/接收 串口中断
 @Input				xRxEnable : 1 ENABLE
							  0 DISABLE
					xTxEnable : 1 ENBALE
							: 0 DISABLE
 @Return				void
**********************************************************************************************************/
void Uart1_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
{
	ENTER_CRITICAL_SECTION();
	
	if (xRxEnable) {													//使能串口接收中断
		__HAL_USART_ENABLE_IT(&UART1_Handler, UART_IT_RXNE);
	}
	else {															//失能串口接收中断
		__HAL_USART_DISABLE_IT(&UART1_Handler, UART_IT_RXNE);
	}
	
	if (xTxEnable) {													//使能串口发送中断
		__HAL_USART_ENABLE_IT(&UART1_Handler, UART_IT_TXE);
	}
	else {															//失能串口发送中断
		__HAL_USART_DISABLE_IT(&UART1_Handler, UART_IT_TXE);
	}
	
	EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			void Uart2_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
 @Description			使能/失能 发送/接收 串口中断
 @Input				xRxEnable : 1 ENABLE
							  0 DISABLE
					xTxEnable : 1 ENBALE
							: 0 DISABLE
 @Return				void
**********************************************************************************************************/
void Uart2_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
{
	ENTER_CRITICAL_SECTION();
	
	if (xRxEnable) {													//使能串口接收中断
		__HAL_USART_ENABLE_IT(&UART2_Handler, UART_IT_RXNE);
	}
	else {															//失能串口接收中断
		__HAL_USART_DISABLE_IT(&UART2_Handler, UART_IT_RXNE);
	}
	
	if (xTxEnable) {													//使能串口发送中断
		__HAL_USART_ENABLE_IT(&UART2_Handler, UART_IT_TXE);
	}
	else {															//失能串口发送中断
		__HAL_USART_DISABLE_IT(&UART2_Handler, UART_IT_TXE);
	}
	
	EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			void Uart_EnterCriticalSection(void)
 @Description			进入关键操作,关闭中断,等待中断结束返回
 @Input				void
 @Return				void
**********************************************************************************************************/
void Uart_EnterCriticalSection(void)
{
	__disable_irq();
}

/**********************************************************************************************************
 @Function			void Uart_ExitCriticalSection(void)
 @Description			退出关键操作,开启中断
 @Input				void
 @Return				void
**********************************************************************************************************/
void Uart_ExitCriticalSection(void)
{
	__enable_irq();
}

#if 1
#pragma import(__use_no_semihosting)

struct __FILE
{
	int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
	x = x;
}

int fputc(int ch, FILE *f)
{
	while ((PRINTF_USART->SR & 0X40) == 0);
	PRINTF_USART->DR = (u8) ch;
	return ch;
}
#endif 

/********************************************** END OF FLEE **********************************************/
