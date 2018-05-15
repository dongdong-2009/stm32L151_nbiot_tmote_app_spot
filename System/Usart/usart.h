#ifndef __USART_H
#define   __USART_H

#include "sys.h"
#include "stdio.h"
#include "string.h"

#define PRINTF_USART			USART2									//Printf使用串口

#define ENTER_CRITICAL_SECTION()	Uart_EnterCriticalSection();
#define EXIT_CRITICAL_SECTION()	Uart_ExitCriticalSection();

#define USART1_REC_LEN			1024										//串口1最大接收字节数
#define USART2_REC_LEN			1024										//串口2最大接收字节数

#define USART1_RXBUFFERSIZE   	1 										//HAL库使用的串口接收缓存大小
#define USART2_RXBUFFERSIZE   	1 										//HAL库使用的串口接收缓存大小

extern u8 USART1_RX_BUF[USART1_REC_LEN];    									//USART1接收缓冲,最大USART1_REC_LEN个字节
extern u8 USART2_RX_BUF[USART2_REC_LEN];    									//USART2接收缓冲,最大USART1_REC_LEN个字节

extern u16 USART1_RX_STA;												//USART1接收状态标记 bit15,接收完成标志 bit14,接收到0x0d(\n)
extern u16 USART2_RX_STA;												//USART2接收状态标记 bit15,接收完成标志 bit14,接收到0x0d(\n)

extern u8 USART1_aRxBuffer[USART1_RXBUFFERSIZE];								//HAL库使用的串口接收缓冲
extern u8 USART2_aRxBuffer[USART2_RXBUFFERSIZE];								//HAL库使用的串口接收缓冲

extern UART_HandleTypeDef UART1_Handler;									//UART1句柄
extern UART_HandleTypeDef UART2_Handler;									//UART2句柄


void Uart1_Init(u32 bound);												//串口1初始化
void Uart2_Init(u32 bound);												//串口2初始化
void Uart1_PortSerialEnable(u8 xRxEnable, u8 xTxEnable);						//使能/失能 发送/接收 串口中断
void Uart2_PortSerialEnable(u8 xRxEnable, u8 xTxEnable);						//使能/失能 发送/接收 串口中断

void Uart_EnterCriticalSection(void);										//进入关键操作,关闭中断,等待中断结束返回
void Uart_ExitCriticalSection(void);										//退出关键操作,开启中断

#endif
