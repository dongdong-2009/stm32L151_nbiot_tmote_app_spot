#ifndef __NET_NBIOT_APP_H
#define   __NET_NBIOT_APP_H

#include "sys.h"

#define NETCoapNeedSendCode_initializer		{ 0, 0, 1, 1, 1, 0 }
#define NETMqttSNNeedSendCode_initializer	{ 0, 0, 1, 1, 1, 0 }

typedef struct
{
	char						ShortStatus;
	char						LongStatus;
	char						WorkInfo;
	char						BasicInfo;
	char						DynamicInfo;
	char						RadarInfo;
}NETCoapNeedSendCodeTypeDef;

extern NETCoapNeedSendCodeTypeDef NETCoapNeedSendCode;

typedef struct
{
	char						StatusBasic;
	char						StatusExtend;
	char						InfoWork;
	char						InfoBasic;
	char						InfoDynamic;
	char						InfoRadar;
}NETMqttSNNeedSendCodeTypeDef;

extern NETMqttSNNeedSendCodeTypeDef NETMqttSNNeedSendCode;


void NET_NBIOT_Initialization(void);										//NET初始化
void NET_NBIOT_DataProcessing(void);										//NET数据处理
void NET_NBIOT_TaskProcessing(void);										//NET工作处理
void NET_NBIOT_App_Task(void);											//NET处理

#endif
