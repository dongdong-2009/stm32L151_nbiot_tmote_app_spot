#ifndef __HAL_IWDG_H
#define   __HAL_IWDG_H

#include "sys.h"

void IWDG_Init(u8 prer, u16 rlr);											//初始化独立看门狗
void IWDG_Feed(void);													//喂独立看门狗

#endif
