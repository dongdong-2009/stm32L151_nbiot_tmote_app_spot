#ifndef __NBIOT_TRANSPORT_H
#define   __NBIOT_TRANSPORT_H

#include "sys.h"
#include "nbiotconfig.h"

NBIOT_StatusTypeDef NBIOT_Transport_SendATCmd(NBIOT_ATCmdTypeDef* ATCmd);											//发送AT指令等待应答
NBIOT_StatusTypeDef NBIOT_Transport_RecvATCmd(NBIOT_ATCmdTypeDef* ATCmd);											//接收AT指令应答
NBIOT_StatusTypeDef NBIOT_Transport_Init(NBIOT_ATCmdTypeDef* ATCmd);												//初始化NBIOT数据传输接口

#endif
