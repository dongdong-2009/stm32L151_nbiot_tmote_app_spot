#ifndef __STM32L1xx_IT_H
#define   __STM32L1xx_IT_H

#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif

void TIM2_IRQHandler(void);

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
