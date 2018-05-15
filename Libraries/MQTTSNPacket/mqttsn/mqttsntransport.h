#ifndef __MQTTSN_TRANSPORT_H
#define   __MQTTSN_TRANSPORT_H

#include "sys.h"
#include "mqttsnconfig.h"

MQTTSN_StatusTypeDef MQTTSN_Transport_Open(MQTTSN_SocketNetTypeDef* NetSock);											//创建一个Socket连接
MQTTSN_StatusTypeDef MQTTSN_Transport_Close(MQTTSN_SocketNetTypeDef* NetSock);											//关闭一个Socket连接
MQTTSN_StatusTypeDef MQTTSN_Transport_Write(MQTTSN_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen);					//发送一条负载数据
MQTTSN_StatusTypeDef MQTTSN_Transport_Read(MQTTSN_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft);		//读取一条负载数据

MQTTSN_StatusTypeDef MQTTSN_Transport_Init(MQTTSN_SocketNetTypeDef* , NBIOT_ClientsTypeDef* , u16 , const char *, u16 );		//初始化

#endif
