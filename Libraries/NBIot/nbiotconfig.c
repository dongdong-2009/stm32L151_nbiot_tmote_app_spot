/**
  *********************************************************************************************************
  * @file    nbiotconfig.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-03
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "nbiotconfig.h"
#include "nbiotfunc.h"
#include "nbiottransport.h"
#include "string.h"

unsigned char NBIOT_SendBuf[NBIOT_BUFFER_SIZE];
unsigned char NBIOT_RecvBuf[NBIOT_BUFFER_SIZE];
unsigned char NBIOT_DataStack[NBIOT_DATASTACK_SIZE];

NBIOT_BaudrateTypeDef NBIOTBaudRate;

/**********************************************************************************************************
 @Function			void NBIOT_Client_Init(NBIOT_ClientsTypeDef* pClient, NBIOT_ATCmdTypeDef* ATCmdStack, NET_NBIOT_ClientsTypeDef* NetNbiotStack)
 @Description			NBIOT_Client_Init						: 初始化NBIOT客户端
 @Input				pClient								: NBIOT客户端实例
					ATCmdStack							: NBIOT AT 协议栈
					NetNbiotStack							: NET NBIOT 协议栈
 @Return				void
**********************************************************************************************************/
void NBIOT_Client_Init(NBIOT_ClientsTypeDef* pClient, NBIOT_ATCmdTypeDef* ATCmdStack, NET_NBIOT_ClientsTypeDef* NetNbiotStack)
{
	pClient->Sendbuf									= NBIOT_SendBuf;
	pClient->Recvbuf									= NBIOT_RecvBuf;
	pClient->Sendbuf_size								= sizeof(NBIOT_SendBuf);
	pClient->Recvbuf_size								= sizeof(NBIOT_RecvBuf);
	pClient->Sendlen									= 0;
	pClient->Recvlen									= 0;
	pClient->DataProcessStack							= NBIOT_DataStack;
	pClient->DataProcessStack_size						= sizeof(NBIOT_DataStack);
	
	pClient->Command_Timeout_Msec							= NBIOT_COMMAND_TIMEOUT_MSEC;
	pClient->Command_Failure_Cnt							= NBIOT_COMMAND_FAILURE_CNT;
	
	pClient->DictateRunCtl.dictateEnable					= false;
	pClient->DictateRunCtl.dictateTimeoutSec				= 0;
	pClient->DictateRunCtl.dictateRebootFailureCnt			= 0;
	pClient->DictateRunCtl.dictateModuleCheckFailureCnt		= 0;
	pClient->DictateRunCtl.dictateParameterConfigFailureCnt	= 0;
	pClient->DictateRunCtl.dictateSimICCIDCheckFailureCnt		= 0;
	pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt	= 0;
	pClient->DictateRunCtl.dictateAttachCheckFailureCnt		= 0;
	pClient->DictateRunCtl.dictateAttachExecuteFailureCnt		= 0;
	pClient->DictateRunCtl.dictateAttachInquireFailureCnt		= 0;
	pClient->DictateRunCtl.dictatePatameterCheckOutFailureCnt	= 0;
	pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt	= 0;
	pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt= 0;
	pClient->DictateRunCtl.dictateCDPServerCheckFailureCnt		= 0;
	pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt	= 0;
	pClient->DictateRunCtl.dictateSendDataFailureCnt			= 0;
	pClient->DictateRunCtl.dictateRecvDataFailureCnt			= 0;
	pClient->DictateRunCtl.dictateEvent					= HARDWARE_REBOOT;
	
	pClient->ATCmdStack									= ATCmdStack;
	pClient->NetNbiotStack								= NetNbiotStack;
	
	memset((void *)&pClient->Parameter, 0x0, sizeof(pClient->Parameter));
}

/********************************************** END OF FLEE **********************************************/
