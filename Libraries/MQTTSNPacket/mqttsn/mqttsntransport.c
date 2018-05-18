/**
  *********************************************************************************************************
  * @file    mqttsntransport.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-02-06
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "mqttsntransport.h"
#include "nbiotfunc.h"
#include "string.h"

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Open(MQTTSN_SocketNetTypeDef* NetSock)
 @Description			MQTTSN_Transport_Open	: 创建一个Socket连接
 @Input				NetSock				: SocketNet Struct*
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Open(MQTTSN_SocketNetTypeDef* NetSock)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	int socketHandler = 0;
	
	NBStatus = NBIOT_Neul_NBxx_CreateUDPSocket(NetSock->NBIotStack, NetSock->LocalPort, 1, &socketHandler);
	if (NBStatus != NBIOT_OK) {
		NetSock->SocketHandler = -1;
		MQTTSNStatus = MQTTSN_ERROR;
	}
	else {
		NetSock->SocketHandler = socketHandler;
		MQTTSNStatus = MQTTSN_OK;
	}
	
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Close(MQTTSN_SocketNetTypeDef* NetSock)
 @Description			MQTTSN_Transport_Close	: 关闭一个Socket连接
 @Input				NetSock				: SocketNet Struct*
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Close(MQTTSN_SocketNetTypeDef* NetSock)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	
	NBStatus = NBIOT_Neul_NBxx_CloseUDPSocket(NetSock->NBIotStack, NetSock->SocketHandler);
	if (NBStatus != NBIOT_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
	}
	else {
		NetSock->SocketHandler = -1;
		MQTTSNStatus = MQTTSN_OK;
	}
	
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Write(MQTTSN_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen)
 @Description			MQTTSN_Transport_Write	: Socket连接发送一条负载数据
 @Input				NetSock				: SocketNet Struct*
					buf					: 负载数据
					sendlen 				: 数据长度
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Write(MQTTSN_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	
	if (NetSock->SocketHandler < 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	memcpy(NetSock->NBIotStack->Sendbuf, buf, sendlen);
	NetSock->NBIotStack->Sendlen = sendlen;
	NBStatus = NBIOT_Neul_NBxx_SendUDPPayload(NetSock->NBIotStack, NetSock->SocketHandler, (char *)NetSock->ServerHost, NetSock->ServerPort);
	if (NBStatus != NBIOT_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
	}
	else {
		MQTTSNStatus = MQTTSN_OK;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Read(MQTTSN_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft)
 @Description			MQTTSN_Transport_Read	: Socket连接读取一条负载数据
 @Input				NetSock				: SocketNet Struct*
					buf					: 读取数据缓存
					maxrlen				: 最大读取数据大小
					rlen					: 读取数据长度地址
					rleft				: 剩余读取数据长度地址
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Read(MQTTSN_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	char addr[16];
	u16 port = 0;
	
	if (NetSock->SocketHandler < 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	memset((void *)addr, 0x0, sizeof(addr));
	NBStatus = NBIOT_Neul_NBxx_ReadUDPPayload(NetSock->NBIotStack, NetSock->SocketHandler, maxrlen, rleft, addr, &port);
	if (NBStatus != NBIOT_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
	}
	else {
		if ((strncmp(addr, (char *)NetSock->ServerHost, 16) == 0) && (port == NetSock->ServerPort)) {
			memcpy(buf, NetSock->NBIotStack->Recvbuf, NetSock->NBIotStack->Recvlen);
			*rlen = NetSock->NBIotStack->Recvlen;
			MQTTSNStatus = MQTTSN_OK;
		}
		else {
			MQTTSNStatus = MQTTSN_ERROR;
		}
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Init(MQTTSN_SocketNetTypeDef* NetSock, NBIOT_ClientsTypeDef* NBIotStack, u16 Localport, const char *Serverhost, u16 Serverport)
 @Description			MQTTSN_Transport_Init	: Initiative初始化MqttSN数据传输接口
 @Input				NetSock				: SocketNet Struct*
					NBIotStack			: NBIot协议栈
					Localport				: 本地端口
					Serverhost			: 服务器地址
					Serverport			: 服务器端口
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Init(MQTTSN_SocketNetTypeDef* NetSock, NBIOT_ClientsTypeDef* NBIotStack, u16 Localport, const char *Serverhost, u16 Serverport)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	
	NetSock->SocketHandler = -1;
	NetSock->LocalPort = Localport;
	memset((void*)NetSock->ServerHost, 0, sizeof(NetSock->ServerHost));
	memcpy(NetSock->ServerHost, Serverhost, strlen(Serverhost));
	NetSock->ServerPort = Serverport;
	
	NetSock->NBIotStack = NBIotStack;
	NetSock->Open = MQTTSN_Transport_Open;
	NetSock->Close = MQTTSN_Transport_Close;
	NetSock->Write = MQTTSN_Transport_Write;
	NetSock->Read = MQTTSN_Transport_Read;
	
	return MQTTSNStatus;
}

/********************************************** END OF FLEE **********************************************/
