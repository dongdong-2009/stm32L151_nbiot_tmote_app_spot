/**
  *********************************************************************************************************
  * @file    mqttsnconfig.c
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

#include "main.h"
#include "mqttsnconfig.h"
#include "mqttsnfunc.h"
#include "mqttsntransport.h"

unsigned char MQTTSN_SendBuf[MQTTSN_BUFFER_SIZE];
unsigned char MQTTSN_RecvBuf[MQTTSN_BUFFER_SIZE];
unsigned char MQTTSN_DataStack[MQTTSN_DATASTACK_SIZE];

/**********************************************************************************************************
 @Function			void MQTTSN_WaitforCallback(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_WaitforCallback	: MQTTSN数据等待接收回调函数
 @Input				pClient				: MqttSN客户端实例
 @Return				void
 @attention			MQTTSN等待相应应答消息到来时间较长,此回调函数提供在等待时运行函数
**********************************************************************************************************/
void MQTTSN_WaitforCallback(MQTTSN_ClientsTypeDef* pClient)
{
	MainMajorCycle();
}

/**********************************************************************************************************
 @Function			void MQTTSN_Client_Init(MQTTSN_ClientsTypeDef* pClient, MQTTSN_SocketNetTypeDef* NetSock)
 @Description			MQTTSN_Client_Init						: 初始化MQTTSN客户端
 @Input				pClient								: MQTTSN客户端实例
					NetSock								: MQTTSN Socket 协议栈
 @Return				void
**********************************************************************************************************/
void MQTTSN_Client_Init(MQTTSN_ClientsTypeDef* pClient, MQTTSN_SocketNetTypeDef* NetSock)
{
	pClient->Sendbuf									= MQTTSN_SendBuf;
	pClient->Recvbuf									= MQTTSN_RecvBuf;
	pClient->Sendbuf_size								= sizeof(MQTTSN_SendBuf);
	pClient->Recvbuf_size								= sizeof(MQTTSN_RecvBuf);
	pClient->Sendlen									= 0;
	pClient->Recvlen									= 0;
	pClient->DataProcessStack							= MQTTSN_DataStack;
	pClient->DataProcessStack_size						= sizeof(MQTTSN_DataStack);
	
	pClient->Command_Timeout_Sec							= MQTTSN_COMMAND_TIMEOUT_SEC;
	pClient->Command_Failure_Cnt							= MQTTSN_COMMAND_FAILURE_CNT;
	
	pClient->DictateRunCtl.dictateEnable					= false;
	pClient->DictateRunCtl.dictateTimeoutSec				= 0;
	pClient->DictateRunCtl.dictateInitFailureCnt				= 0;
	pClient->DictateRunCtl.dictateDisconnectFailureCnt		= 0;
	pClient->DictateRunCtl.dictateActiveFailureCnt			= 0;
	pClient->DictateRunCtl.dictateSleepFailureCnt			= 0;
	pClient->DictateRunCtl.dictateAweakFailureCnt			= 0;
	pClient->DictateRunCtl.dictateLostFailureCnt				= 0;
	pClient->DictateRunCtl.dictateSubscribeStatus			= false;
	
	pClient->MessageSendCtl.messageStatusBasic				= false;
	pClient->MessageSendCtl.messageStatusExtend				= false;
	pClient->MessageSendCtl.messageInfoWork					= false;
	pClient->MessageSendCtl.messageInfoBasic				= false;
	pClient->MessageSendCtl.messageInfoDynamic				= false;
	pClient->MessageSendCtl.messageInfoRadar				= false;
	
	pClient->MsgId										= 1;
	pClient->defaultMessageHandler						= NULL;
	pClient->SubState									= MQTTSN_SUBSTATE_INIT;
	pClient->SocketStack								= NetSock;
	
	for (int i = 0; i < MAX_MESSAGE_HANDLERS; i++) {
		pClient->messageHandlers[i].topicFilter				= NULL;
		pClient->messageHandlers[i].topicid				= 0;
		pClient->messageHandlers[i].fp					= NULL;
	}
}

/********************************************** END OF FLEE **********************************************/
