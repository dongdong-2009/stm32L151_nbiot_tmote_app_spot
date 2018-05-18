/**
  *********************************************************************************************************
  * @file    dnstransport.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-05-15
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "dnstransport.h"
#include "nbiotfunc.h"
#include "string.h"

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNS_Transport_Open(DNS_SocketNetTypeDef* NetSock)
 @Description			DNS_Transport_Open		: 创建一个Socket连接
 @Input				NetSock				: SocketNet Struct*
 @Return				DNS_StatusTypeDef		: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNS_Transport_Open(DNS_SocketNetTypeDef* NetSock)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	DNS_StatusTypeDef DNSStatus = DNS_OK;
	int socketHandler = 0;
	
	NBStatus = NBIOT_Neul_NBxx_CreateUDPSocket(NetSock->NBIotStack, NetSock->LocalPort, 1, &socketHandler);
	if (NBStatus != NBIOT_OK) {
		NetSock->SocketHandler = -1;
		DNSStatus = DNS_ERROR;
	}
	else {
		NetSock->SocketHandler = socketHandler;
		DNSStatus = DNS_OK;
	}
	
	return DNSStatus;
}

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNS_Transport_Close(DNS_SocketNetTypeDef* NetSock)
 @Description			DNS_Transport_Close		: 关闭一个Socket连接
 @Input				NetSock				: SocketNet Struct*
 @Return				DNS_StatusTypeDef		: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNS_Transport_Close(DNS_SocketNetTypeDef* NetSock)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	DNS_StatusTypeDef DNSStatus = DNS_OK;
	
	NBStatus = NBIOT_Neul_NBxx_CloseUDPSocket(NetSock->NBIotStack, NetSock->SocketHandler);
	if (NBStatus != NBIOT_OK) {
		DNSStatus = DNS_ERROR;
	}
	else {
		NetSock->SocketHandler = -1;
		DNSStatus = DNS_OK;
	}
	
	return DNSStatus;
}

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNS_Transport_Write(DNS_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen)
 @Description			DNS_Transport_Write		: Socket连接发送一条负载数据
 @Input				NetSock				: SocketNet Struct*
					buf					: 负载数据
					sendlen 				: 数据长度
 @Return				DNS_StatusTypeDef		: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNS_Transport_Write(DNS_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	DNS_StatusTypeDef DNSStatus = DNS_OK;
	
	if (NetSock->SocketHandler < 0) {
		DNSStatus = DNS_ERROR;
		goto exit;
	}
	
	memcpy(NetSock->NBIotStack->Sendbuf, buf, sendlen);
	NetSock->NBIotStack->Sendlen = sendlen;
	NBStatus = NBIOT_Neul_NBxx_SendUDPPayload(NetSock->NBIotStack, NetSock->SocketHandler, (char *)NetSock->ServerHost, NetSock->ServerPort);
	if (NBStatus != NBIOT_OK) {
		DNSStatus = DNS_ERROR;
	}
	else {
		DNSStatus = DNS_OK;
	}
	
exit:
	return DNSStatus;
}

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNS_Transport_Read(DNS_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft)
 @Description			DNS_Transport_Read		: Socket连接读取一条负载数据
 @Input				NetSock				: SocketNet Struct*
					buf					: 读取数据缓存
					maxrlen				: 最大读取数据大小
					rlen					: 读取数据长度地址
					rleft				: 剩余读取数据长度地址
 @Return				DNS_StatusTypeDef		: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNS_Transport_Read(DNS_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	DNS_StatusTypeDef DNSStatus = DNS_OK;
	char addr[16];
	u16 port = 0;
	
	if (NetSock->SocketHandler < 0) {
		DNSStatus = DNS_ERROR;
		goto exit;
	}
	
	memset((void *)addr, 0x0, sizeof(addr));
	NBStatus = NBIOT_Neul_NBxx_ReadUDPPayload(NetSock->NBIotStack, NetSock->SocketHandler, maxrlen, rleft, addr, &port);
	if (NBStatus != NBIOT_OK) {
		DNSStatus = DNS_ERROR;
	}
	else {
		if ((strncmp(addr, (char *)NetSock->ServerHost, 16) == 0) && (port == NetSock->ServerPort)) {
			memcpy(buf, NetSock->NBIotStack->Recvbuf, NetSock->NBIotStack->Recvlen);
			*rlen = NetSock->NBIotStack->Recvlen;
			DNSStatus = DNS_OK;
		}
		else {
			DNSStatus = DNS_ERROR;
		}
	}
	
exit:
	return DNSStatus;
}

/**********************************************************************************************************
 @Function			DNS_StatusTypeDef DNS_Transport_Init(DNS_SocketNetTypeDef* NetSock, NBIOT_ClientsTypeDef* NBIotStack, u16 Localport, const char *Serverhost, u16 Serverport)
 @Description			DNS_Transport_Init		: Initiative初始化DNS数据传输接口
 @Input				NetSock				: SocketNet Struct*
					NBIotStack			: NBIot协议栈
					Localport				: 本地端口
					Serverhost			: 服务器地址
					Serverport			: 服务器端口
 @Return				DNS_StatusTypeDef		: DNS处理状态
**********************************************************************************************************/
DNS_StatusTypeDef DNS_Transport_Init(DNS_SocketNetTypeDef* NetSock, NBIOT_ClientsTypeDef* NBIotStack, u16 Localport, const char *Serverhost, u16 Serverport)
{
	DNS_StatusTypeDef DNSStatus = DNS_OK;
	
	NetSock->SocketHandler = -1;
	NetSock->LocalPort = Localport;
	memset((void*)NetSock->ServerHost, 0, sizeof(NetSock->ServerHost));
	memcpy(NetSock->ServerHost, Serverhost, strlen(Serverhost));
	NetSock->ServerPort = Serverport;
	
	NetSock->NBIotStack = NBIotStack;
	NetSock->Open = DNS_Transport_Open;
	NetSock->Close = DNS_Transport_Close;
	NetSock->Write = DNS_Transport_Write;
	NetSock->Read = DNS_Transport_Read;
	
	return DNSStatus;
}

/********************************************** END OF FLEE **********************************************/
