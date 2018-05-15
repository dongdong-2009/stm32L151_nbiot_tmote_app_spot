#ifndef __DNS_TRANSPORT_H
#define   __DNS_TRANSPORT_H

#include "sys.h"
#include "dnsconfig.h"

DNS_StatusTypeDef DNS_Transport_Open(DNS_SocketNetTypeDef* NetSock);													//创建一个Socket连接
DNS_StatusTypeDef DNS_Transport_Close(DNS_SocketNetTypeDef* NetSock);													//关闭一个Socket连接
DNS_StatusTypeDef DNS_Transport_Write(DNS_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen);							//发送一条负载数据
DNS_StatusTypeDef DNS_Transport_Read(DNS_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft);			//读取一条负载数据

DNS_StatusTypeDef DNS_Transport_Init(DNS_SocketNetTypeDef* NetSock, NBIOT_ClientsTypeDef* NBIotStack, u16 , const char *, u16 );	//初始化

#endif
