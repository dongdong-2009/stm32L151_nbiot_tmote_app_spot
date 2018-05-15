#ifndef __ROLLING_OVER_H
#define   __ROLLING_OVER_H

#include "sys.h"

#define ROLLINGOVER_LOG_RF_PRINT											//定义开启RF输出信息

#define ROLLINGOVER_ENTER_SLEEP_TIME		900								//等待900秒进入休眠
#define ROLLINGOVER_WAIT_SLEEP_TIME		15								//保持15秒处理

void RollingOverPollExecution(void);										//翻转检测处理
bool RollingOverCheckToActive(void);										//翻转检测
void RollingOverMercuryBreak(void);										//水银开关断开工作
void RollingOverMercuryClose(void);										//水银开关闭合休眠
void RollingOverCheckToInitSensorBackground(void);							//检查是否需要初始化传感器背景

#endif
