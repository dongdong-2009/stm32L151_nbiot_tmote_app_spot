#ifndef __HAL_SWITCH_H
#define   __HAL_SWITCH_H

#include "sys.h"

#define MERCURY_GPIOx					GPIOH
#define MERCURY_PIN						GPIO_PIN_0
#define MERCURY_RCC_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOH_CLK_ENABLE()
#define MERCURY_RCC_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOH_CLK_DISABLE()
#define MERCURY_READ()					HAL_GPIO_ReadPin(MERCURY_GPIOx, MERCURY_PIN)

u8 Mercury_Read(void);													//水银开关读取状态

#endif
