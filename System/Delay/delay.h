#ifndef __DELAY_H
#define   __DELAY_H

#include <sys.h>

void Delay_Init(u32 SYSCLK);
void Delay_MS(u16 nms);
void Delay_US(u32 nus);

#endif
