/**
  *********************************************************************************************************
  * @file    dnsconfig.c
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

#include "dnsconfig.h"
#include "dnsfunc.h"
#include "dnstransport.h"

unsigned char DNS_SendBuf[DNS_BUFFER_SIZE];
unsigned char DNS_RecvBuf[DNS_BUFFER_SIZE];

/**********************************************************************************************************
 @Function			void DNS_Client_Init(DNS_ClientsTypeDef* pClient, DNS_SocketNetTypeDef* NetSock)
 @Description			DNS_Client_Init						: 初始化DNS客户端
 @Input				pClient								: DNS客户端实例
					NetSock								: DNS Socket 协议栈
 @Return				void
**********************************************************************************************************/
void DNS_Client_Init(DNS_ClientsTypeDef* pClient, DNS_SocketNetTypeDef* NetSock)
{
	pClient->Sendbuf									= DNS_SendBuf;
	pClient->Recvbuf									= DNS_RecvBuf;
	pClient->Sendbuf_size								= sizeof(DNS_SendBuf);
	pClient->Recvbuf_size								= sizeof(DNS_RecvBuf);
	pClient->Sendlen									= 0;
	pClient->Recvlen									= 0;
	
	pClient->Command_Timeout_Sec							= DNS_COMMAND_TIMEOUT_SEC;
	pClient->Command_Failure_Cnt							= DNS_COMMAND_FAILURE_CNT;
	
	pClient->LetterCounter								= 0;
	
	
	
	
	
	pClient->SocketStack								= NetSock;
	
	
}










/********************************************** END OF FLEE **********************************************/
