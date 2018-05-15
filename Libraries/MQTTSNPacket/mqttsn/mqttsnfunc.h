#ifndef __MQTTSN_FUNC_H
#define   __MQTTSN_FUNC_H

#include "sys.h"
#include "mqttsnconfig.h"

MQTTSN_StatusTypeDef MQTTSN_ReadPacket(MQTTSN_ClientsTypeDef* pClient, int *msgTypes);								//获取数据包数据与消息类型
MQTTSN_StatusTypeDef MQTTSN_RecvPacketProcess(MQTTSN_ClientsTypeDef* pClient, int *msgTypes);							//接收数据包处理
MQTTSN_StatusTypeDef MQTTSN_WaitforRecvAck(MQTTSN_ClientsTypeDef* pClient, int msgType, Stm32_CalculagraphTypeDef* timerS);	//等待接收到相应应答消息类型

/* Application Programming Interface */
MQTTSN_StatusTypeDef MQTTSN_Connect(MQTTSN_ClientsTypeDef* pClient, MQTTSNPacket_connectData* options);

MQTTSN_StatusTypeDef MQTTSN_DisConnect(MQTTSN_ClientsTypeDef* pClient, int duration);

MQTTSN_StatusTypeDef MQTTSN_Register(MQTTSN_ClientsTypeDef* pClient, const char* topicName, unsigned short* topicid);

MQTTSN_StatusTypeDef MQTTSN_Publish(MQTTSN_ClientsTypeDef* pClient, MQTTSN_topicid topic, MQTTSN_MessageTypeDef* message);

MQTTSN_StatusTypeDef MQTTSN_Pingreq(MQTTSN_ClientsTypeDef* pClient, MQTTSNString* clientid);

MQTTSN_StatusTypeDef MQTTSN_Subscribe(MQTTSN_ClientsTypeDef* pClient, const char* topicFilter, enum QoS qos, messageHandlerFp messageHandler);

MQTTSN_StatusTypeDef MQTTSN_RecvAck(MQTTSN_ClientsTypeDef* pClient);

#endif
