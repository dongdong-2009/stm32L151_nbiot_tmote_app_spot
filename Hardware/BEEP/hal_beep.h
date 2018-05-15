#ifndef __HAL_BEEP_H
#define   __HAL_BEEP_H

#include "sys.h"

#define BEEP_GPIOx						GPIOB
#define BEEP_PIN						GPIO_PIN_8
#define BEEP_RCC_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define BEEP_RCC_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOB_CLK_DISABLE()

#define BEEP(a)						BEEP_Ctrl(a);

void BEEP_Ctrl(u8 beepEnable);											//蜂鸣器控制
void BEEP_CtrlRepeat(u16 nCount, u16 nMs);									//蜂鸣器控制重复响
void BEEP_CtrlRepeat_Extend(u16 nCount, u16 speak_nMs, u16 shut_nMs);				//蜂鸣器控制重复响

#endif
