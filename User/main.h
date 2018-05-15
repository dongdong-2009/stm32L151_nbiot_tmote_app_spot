#ifndef __MAIN_H
#define   __MAIN_H

#include "stm32l1xx_hal.h"
#include "stdbool.h"

void MainMajorCycle(void);												//MqttSN等待数据接收中处理程序
void MainHandleRoutine(void);												//日常处理

void MainRollingEnteredUpWork(void);										//已进入正放工作状态
void MainRollingEnteringUpWork(void);										//将进入正放工作状态
void MainRollingUpwardsActived(void);										//正放工作中
void MainRollingUpwardsSleep(void);										//正放休眠中

void MainRollingEnteredDownWork(void);										//已进入倒放工作状态
void MainRollingEnteredDownSleep(void);										//已进入倒放休眠状态
void MainRollingEnteredDownSleepKeepActived(void);							//将进入倒放休眠状态前保持工作
void MainRollingEnteringDownSleep(void);									//将进入倒放休眠
void MainRollingDownSleep(void);											//倒放休眠中

#endif

