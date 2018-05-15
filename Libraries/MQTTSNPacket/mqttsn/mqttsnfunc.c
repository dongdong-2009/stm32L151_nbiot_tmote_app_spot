/**
  *********************************************************************************************************
  * @file    mqttsnfunc.c
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

#include "mqttsnfunc.h"
#include "string.h"

/**********************************************************************************************************
 @Function			static unsigned int MQTTSN_GetNextMsgId(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_GetNextMsgId		: 获取下一个消息ID
 @Input				pClient				: MqttSN客户端实例
 @Return				MsgId				: 消息ID
**********************************************************************************************************/
static unsigned int MQTTSN_GetNextMsgId(MQTTSN_ClientsTypeDef* pClient)
{
	pClient->MsgId = (pClient->MsgId == MAX_MSG_ID) ? 1 : pClient->MsgId + 1;
	return pClient->MsgId;
}

/**********************************************************************************************************
 @Function			static void MQTTSN_NewMessageData(MQTTSN_MessageDataTypeDef* messageData, MQTTSNString* topicName, MQTTSN_MessageTypeDef* message)
 @Description			MQTTSN_NewMessageData	: 获取消息数据
 @Input				pClient				: MqttSN客户端实例
 @Return				MsgId				: 消息ID
**********************************************************************************************************/
static void MQTTSN_NewMessageData(MQTTSN_MessageDataTypeDef* messageData, MQTTSNString* topicName, MQTTSN_MessageTypeDef* message)
{
	messageData->topicName = topicName;
	messageData->message = message;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_DeliverMessage(MQTTSN_ClientsTypeDef* pClient, MQTTSN_topicid* topicId, MQTTSN_MessageTypeDef* message)
 @Description			MQTTSN_DeliverMessage	: 传入消息数据
 @Input				pClient				: MqttSN客户端实例
					topicId				: 消息主题
					message				: 消息
 @Return				MsgId				: 消息ID
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_DeliverMessage(MQTTSN_ClientsTypeDef* pClient, MQTTSN_topicid* topicId, MQTTSN_MessageTypeDef* message)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_ERROR;
	MQTTSNString topicName;
	
	/* find the right message handler - indexed by topic */
	for (int i = 0; i < MAX_MESSAGE_HANDLERS; i++) {
		/* select the right topic and put payload to the message register */
		if ((pClient->messageHandlers[i].topicid != 0) && (topicId->data.id == pClient->messageHandlers[i].topicid)) {
			if (pClient->messageHandlers[i].fp != NULL) {
				MQTTSN_MessageDataTypeDef messagedata;
				
				topicName.cstring = (char *)pClient->messageHandlers[i].topicFilter;
				MQTTSN_NewMessageData(&messagedata, &topicName, message);
				pClient->messageHandlers[i].fp(pClient, &messagedata);
				MQTTSNStatus = MQTTSN_OK;
			}
		}
	}
	
	if ((MQTTSNStatus == MQTTSN_ERROR) && (pClient->defaultMessageHandler != NULL)) {
		MQTTSN_MessageDataTypeDef messagedata;
		
		MQTTSN_NewMessageData(&messagedata, &topicName, message);
		pClient->defaultMessageHandler(pClient, &messagedata);
		MQTTSNStatus = MQTTSN_OK;
	}
	
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_ReadPacket(MQTTSN_ClientsTypeDef* pClient, int msgTypes)
 @Description			MQTTSN_ReadPacket		: MQTTSN获取数据包数据与消息类型
 @Input				pClient				: MqttSN客户端实例
					msgTypes				: MqttSN消息类型地址
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
 @attention			MQTTSN通用消息格式		: 消息头(2/4Byte) + 其他可变部分(nByte)
					MQTTSN消息头部			: 长度(1/3Byte) + 消息类型(1Byte)
					*消息长度要么是1字节要么是3字节,并且自身也会包含在内。
					*1个字节可表示256长度
					*当第一字节为0x01,那么长度为3字节表示,剩下的两个字节会标识真正的消息长度,最大长度为65535
					*大部分消息长度都是1个字节,除非特别提醒
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_ReadPacket(MQTTSN_ClientsTypeDef* pClient, int *msgTypes)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	int Rlength = 0;													//读取数据长度
	int Rleftlength = 0;												//剩余读取数据长度
	int len = 0;
	int rem_len = 0;
	unsigned char* bufptr;
	
	/* 1. read the header one byte. This has the packet length in it */
	MQTTSNStatus = pClient->SocketStack->Read(pClient->SocketStack, (char *)pClient->Recvbuf, 1, &Rlength, &Rleftlength);
	if ((MQTTSNStatus != MQTTSN_OK) || (Rlength != 1)) {
		goto exit;
	}
	
	len = Rlength;
	
	/* 2. read the remaining length. This is variable in itself */
	if (pClient->Recvbuf[0] == 0x01) {										//长度为3字节表示
		MQTTSNStatus = pClient->SocketStack->Read(pClient->SocketStack, (char *)pClient->Recvbuf + len, 2, &Rlength, &Rleftlength);
		if ((MQTTSNStatus != MQTTSN_OK) || (Rlength != 2)) {
			goto exit;
		}
		len += Rlength;
		bufptr = &pClient->Recvbuf[1];
		rem_len = readInt(&bufptr) - 3;
	}
	else {															//长度为1字节表示
		rem_len = pClient->Recvbuf[0] - 1;
	}
	
	if (rem_len > (pClient->Recvbuf_size - len)) {
		MQTTSNStatus = MQTTSN_BUFFER_OVERFLOW;
		goto exit;
	}
	
	/* 3. read the rest of the buffer using a callback to supply the rest of the data */
	if ((rem_len <= 0) || ((MQTTSNStatus = pClient->SocketStack->Read(pClient->SocketStack, (char *)pClient->Recvbuf + len, rem_len, &Rlength, &Rleftlength)) != MQTTSN_OK)) {
		goto exit;
	}
	
	*msgTypes = pClient->Recvbuf[len];
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_ConnectedAckEvent(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_ConnectAckEvent
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_ConnectAckEvent(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	//Todo
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_DisconnectEvent(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_DisconnectEvent
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_DisconnectEvent(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	//Todo
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_RegisterAckEvent(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_RegisterAckEvent
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_RegisterAckEvent(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	//Todo
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_SubscribeAckEvent(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_SubscribeAckEvent
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_SubscribeAckEvent(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	//Todo
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_PublishAckEvent(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_PublishAckEvent
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_PublishAckEvent(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	//Todo
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_PublishCompEvent(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_PublishCompEvent
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_PublishCompEvent(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	//Todo
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_PublishEvent(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_PublishEvent
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_PublishEvent(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	MQTTSN_topicid pubtopic;
	MQTTSN_MessageTypeDef message;
	int len = 0;
	int qos = -1;
	
	/* deserialize the publish packet and put parameter to register, payload to mqttsn client recvbuf */
	if (MQTTSNDeserialize_publish(&message.dup, 
							&qos, 
							&message.retained, 
							&message.msgid, 
							&pubtopic, 
							&message.payload, 
							&message.payloadlen, 
							pClient->Recvbuf, 
							pClient->Recvbuf_size) != 1) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* check QoS and make decision to send ack or rec packet */
	message.qos = (enum QoS)qos;
	
	/* puy payload to client messageHandler */
	if ((MQTTSNStatus = MQTTSN_DeliverMessage(pClient, &pubtopic, &message)) != MQTTSN_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* when QoS is not equal to 0 */
	if (message.qos != QOS0) {
		if (message.qos == QOS1) {
			/* Serialize PublishAck Command Buffer */
			if ((len = MQTTSNSerialize_puback(pClient->Sendbuf, pClient->Sendbuf_size, pubtopic.data.id, message.msgid, MQTTSN_RC_ACCEPTED)) <= 0) {
				MQTTSNStatus = MQTTSN_ERROR;
				goto exit;
			}
		}
		else if (message.qos == QOS2) {
			/* Serialize PublishRec Command Buffer */
			if ((len = MQTTSNSerialize_pubrec(pClient->Sendbuf, pClient->Sendbuf_size, message.msgid)) <= 0) {
				MQTTSNStatus = MQTTSN_ERROR;
				goto exit;
			}
		}
		
		/* Send PublishAck or PublishRec Command Buffer to MqttSN Server */
		if ((MQTTSNStatus = pClient->SocketStack->Write(pClient->SocketStack, (char *)pClient->Sendbuf, len)) != MQTTSN_OK) {
			MQTTSNStatus = MQTTSN_ERROR;
			goto exit;
		}
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			static MQTTSN_StatusTypeDef MQTTSN_PingRespEvent(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_PingRespEvent
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
static MQTTSN_StatusTypeDef MQTTSN_PingRespEvent(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	//Todo
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_RecvPacketProcess(MQTTSN_ClientsTypeDef* pClient, int *msgTypes)
 @Description			MQTTSN_RecvPacketProcess	: MQTTSN接收数据包处理
 @Input				pClient				: MqttSN客户端实例
					msgTypes				: MqttSN消息类型
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_RecvPacketProcess(MQTTSN_ClientsTypeDef* pClient, int *msgTypes)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	int packetType = -1;
	
	MQTTSNStatus = MQTTSN_ReadPacket(pClient, &packetType);					//获取MqttSN数据并提取消息类型
	if (MQTTSNStatus != MQTTSN_OK) {										//没有获取到数据
		MQTTSNStatus = MQTTSN_RECVPACKET_NONE;
		goto exit;
	}
	
	switch (packetType)													//根据消息类型执行不同处理
	{
	case MQTTSN_CONNACK:
		MQTTSNStatus = MQTTSN_ConnectAckEvent(pClient);
		break;
	
	case MQTTSN_DISCONNECT:
		MQTTSNStatus = MQTTSN_DisconnectEvent(pClient);
		break;
	
	case MQTTSN_REGACK:
		MQTTSNStatus = MQTTSN_RegisterAckEvent(pClient);
		break;
	
	case MQTTSN_SUBACK:
		MQTTSNStatus = MQTTSN_SubscribeAckEvent(pClient);
		break;
	
	case MQTTSN_PUBACK:
		MQTTSNStatus = MQTTSN_PublishAckEvent(pClient);
		break;
	
	case MQTTSN_PUBCOMP:
		MQTTSNStatus = MQTTSN_PublishCompEvent(pClient);
		break;
		
	case MQTTSN_PUBLISH:
		MQTTSNStatus = MQTTSN_PublishEvent(pClient);
		break;
	
	case MQTTSN_PINGRESP:
		MQTTSNStatus = MQTTSN_PingRespEvent(pClient);
		break;
	
	default:
		MQTTSNStatus = MQTTSN_ERROR;
		break;
	}
	
	if (MQTTSNStatus == MQTTSN_OK) {
		*msgTypes = packetType;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_WaitforRecvAck(MQTTSN_ClientsTypeDef* pClient, int msgType, Stm32_CalculagraphTypeDef* timerS)
 @Description			MQTTSN_WaitforRecvAck	: MQTTSN等待接收到相应应答消息类型
 @Input				pClient				: MqttSN客户端实例
					msgType				: MqttSN消息类型
					timer				: 超时计时器
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_WaitforRecvAck(MQTTSN_ClientsTypeDef* pClient, int msgType, Stm32_CalculagraphTypeDef* timerS)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	int packetType = -1;
	
	while (packetType != msgType) {
		if ((MQTTSNStatus = MQTTSN_RecvPacketProcess(pClient, &packetType)) == MQTTSN_ERROR) {	//接收数据处理
			goto exit;
		}
		if (Stm32_Calculagraph_IsExpiredSec(timerS) == true) {								//到达超时时间
			break;
		}
		
		MQTTSN_WaitforCallback(pClient);
	}
	
	if (packetType != msgType) {
		MQTTSNStatus = MQTTSN_CMD_TIMEOUT;
	}
	else {
		MQTTSNStatus = MQTTSN_OK;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Connect(MQTTSN_ClientsTypeDef* pClient, MQTTSNPacket_connectData* options)
 @Description			MQTTSN_Connect			: MQTTSN连接网关服务器
 @Input				pClient				: MqttSN客户端实例
					options				: MqttSN连接选项
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Connect(MQTTSN_ClientsTypeDef* pClient, MQTTSNPacket_connectData* options)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	MQTTSNPacket_connectData Default_options = MQTTSNPacket_connectData_initializer;
	Stm32_CalculagraphTypeDef Connect_timer_s;
	int len = 0;
	
	/* set default options if none were supplied */
	if (options == NULL) {
		options = &Default_options;
	}
	
	/* Configuration Calculagraph for Connect Timer */
	Stm32_Calculagraph_CountdownSec(&Connect_timer_s, pClient->Command_Timeout_Sec);
	
	/* Serialize Connect Command Buffer */
	if ((len = MQTTSNSerialize_connect(pClient->Sendbuf, pClient->Sendbuf_size, options)) <= 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Send Connect Command Buffer to MqttSN Server */
	if ((MQTTSNStatus = pClient->SocketStack->Write(pClient->SocketStack, (char *)pClient->Sendbuf, len)) != MQTTSN_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* This will be a blocking call, wait for the MQTTSN_CONNACK */
	if ((MQTTSNStatus = MQTTSN_WaitforRecvAck(pClient, MQTTSN_CONNACK, &Connect_timer_s)) == MQTTSN_OK) {
		int connack_rc = -1;
		if ((MQTTSNDeserialize_connack(&connack_rc, pClient->Recvbuf, pClient->Recvbuf_size) != 1) || (connack_rc != 0)) {
			MQTTSNStatus = MQTTSN_ERROR;
			goto exit;
		}
		else {
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			MQTTSNStatus = MQTTSN_OK;
		}
	}
	else {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_DisConnect(MQTTSN_ClientsTypeDef* pClient, int duration)
 @Description			MQTTSN_DisConnect		: MQTTSN断连网关服务器
 @Input				pClient				: MqttSN客户端实例
					duration				: MqttSN休眠时间
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_DisConnect(MQTTSN_ClientsTypeDef* pClient, int duration)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	Stm32_CalculagraphTypeDef Disconnect_timer_s;
	int len = 0;
	
	/* Don't send connect packet again if we are already connected */
	if ((pClient->SubState != MQTTSN_SUBSTATE_ACTIVE) || (duration < 0)) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Configuration Calculagraph for Disconnect Timer */
	Stm32_Calculagraph_CountdownSec(&Disconnect_timer_s, pClient->Command_Timeout_Sec);
	
	/* Serialize Disconnect with duration */
	if ((len = MQTTSNSerialize_disconnect(pClient->Sendbuf, pClient->Sendbuf_size, duration)) <= 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Send Disconnect Command Buffer to MqttSN Server */
	if ((MQTTSNStatus = pClient->SocketStack->Write(pClient->SocketStack, (char *)pClient->Sendbuf, len)) != MQTTSN_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* This will be a blocking call, wait for the MQTTSN_DISCONNECT */
	if ((MQTTSNStatus = MQTTSN_WaitforRecvAck(pClient, MQTTSN_DISCONNECT, &Disconnect_timer_s)) == MQTTSN_OK) {
		if (duration > 0) {
			pClient->SubState = MQTTSN_SUBSTATE_SLEEP;
		}
		else {
			pClient->SubState = MQTTSN_SUBSTATE_DISCONNECT;
		}
		MQTTSNStatus = MQTTSN_OK;
	}
	else {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Register(MQTTSN_ClientsTypeDef* pClient, const char* topicName, unsigned short* topicid)
 @Description			MQTTSN_Register		: MQTTSN根据TopicName动态申请TopicId
 @Input				pClient				: MqttSN客户端实例
					topicName				: MqttSN主题名称
					topicid				: 主题ID
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Register(MQTTSN_ClientsTypeDef* pClient, const char* topicName, unsigned short* topicid)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	Stm32_CalculagraphTypeDef Register_timer_s;
	int len = 0;
	
	MQTTSNString topicstr;
	unsigned short SendMsgId;
	unsigned short RecvMsgId;
	
	/* Don't send connect packet again if we are already connected */
	if ((pClient->SubState != MQTTSN_SUBSTATE_ACTIVE) || (topicName == NULL)) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Configuration Calculagraph for Register Timer */
	Stm32_Calculagraph_CountdownSec(&Register_timer_s, pClient->Command_Timeout_Sec);
	
	topicstr.cstring = (char *)topicName;
	topicstr.lenstring.len = strlen(topicName);
	SendMsgId = MQTTSN_GetNextMsgId(pClient);
	/* Serialize Register Command Buffer */
	if ((len = MQTTSNSerialize_register(pClient->Sendbuf, pClient->Sendbuf_size, 0, SendMsgId, &topicstr)) <= 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Send Register Command Buffer to MqttSN Server */
	if ((MQTTSNStatus = pClient->SocketStack->Write(pClient->SocketStack, (char *)pClient->Sendbuf, len)) != MQTTSN_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* This will be a blocking call, wait for the MQTTSN_REGACK */
	if ((MQTTSNStatus = MQTTSN_WaitforRecvAck(pClient, MQTTSN_REGACK, &Register_timer_s)) == MQTTSN_OK) {
		unsigned char returncode;
		MQTTSNDeserialize_regack(topicid, &RecvMsgId, &returncode, pClient->Recvbuf, pClient->Recvbuf_size);
		if (returncode != 0) {
			MQTTSNStatus = MQTTSN_ERROR;
			goto exit;
		}
		else {
			MQTTSNStatus = MQTTSN_OK;
		}
	}
	else {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Publish(MQTTSN_ClientsTypeDef* pClient, MQTTSN_topicid topic, MQTTSN_MessageTypeDef* message)
 @Description			MQTTSN_Publish			: MQTTSN发布一条消息
 @Input				pClient				: MqttSN客户端实例
					topic				: MqttSN主题Id
					message				: MqttSN消息
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Publish(MQTTSN_ClientsTypeDef* pClient, MQTTSN_topicid topic, MQTTSN_MessageTypeDef* message)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	Stm32_CalculagraphTypeDef Publish_timer_s;
	int len = 0;
	
	/* Don't send connect packet again if we are already connected */
	if ((pClient->SubState != MQTTSN_SUBSTATE_ACTIVE) || (message == NULL)) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Configuration Calculagraph for Publish Timer */
	Stm32_Calculagraph_CountdownSec(&Publish_timer_s, pClient->Command_Timeout_Sec);
	
	if (message->qos == QOS0) {
		message->msgid = 0;
	}
	else if ((message->qos == QOS1) || (message->qos == QOS2)) {
		message->msgid = MQTTSN_GetNextMsgId(pClient);
	}
	else {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Serialize Publish Command Buffer */
	if ((len = MQTTSNSerialize_publish(pClient->Sendbuf, 
								pClient->Sendbuf_size, 
								message->dup, 
								message->qos, 
								message->retained, 
								message->msgid, 
								topic, 
								message->payload, 
								message->payloadlen)) <= 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Send Publish Command Buffer to MqttSN Server */
	if ((MQTTSNStatus = pClient->SocketStack->Write(pClient->SocketStack, (char *)pClient->Sendbuf, len)) != MQTTSN_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* This will be a blocking call, wait for the MQTTSN_PUBACK or MQTTSN_PUBCOMP */
	if (message->qos == QOS1) {
		if ((MQTTSNStatus = MQTTSN_WaitforRecvAck(pClient, MQTTSN_PUBACK, &Publish_timer_s)) == MQTTSN_OK) {
			unsigned short packet_id, topic_id;
			unsigned char returncode;
			if (MQTTSNDeserialize_puback(&topic_id, &packet_id, &returncode, pClient->Recvbuf, pClient->Recvbuf_size) != 1 || returncode != MQTTSN_RC_ACCEPTED) {
				MQTTSNStatus = MQTTSN_ERROR;
				goto exit;
			}
			else {
				MQTTSNStatus = MQTTSN_OK;
			}
		}
		else {
			MQTTSNStatus = MQTTSN_ERROR;
			goto exit;
		}
	}
	else if (message->qos == QOS2) {
		if ((MQTTSNStatus = MQTTSN_WaitforRecvAck(pClient, MQTTSN_PUBCOMP, &Publish_timer_s)) == MQTTSN_OK) {
			unsigned short packet_id;
			unsigned char type;
			if (MQTTSNDeserialize_ack(&type, &packet_id, pClient->Recvbuf, pClient->Recvbuf_size) != 1) {
				MQTTSNStatus = MQTTSN_ERROR;
				goto exit;
			}
			else {
				MQTTSNStatus = MQTTSN_OK;
			}
		}
		else {
			MQTTSNStatus = MQTTSN_ERROR;
			goto exit;
		}
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Pingreq(MQTTSN_ClientsTypeDef* pClient, MQTTSNString* clientid)
 @Description			MQTTSN_Pingreq			: MQTTSN发送Pingreq包
 @Input				pClient				: MqttSN客户端实例
					clientid				: 1~23字符识别客户端ID
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Pingreq(MQTTSN_ClientsTypeDef* pClient, MQTTSNString* clientid)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	Stm32_CalculagraphTypeDef Pingreq_timer_s;
	int len = 0;
	
	/* Don't send connect packet or lost again if we are already connected */
	if ((pClient->SubState == MQTTSN_SUBSTATE_DISCONNECT) || (pClient->SubState == MQTTSN_SUBSTATE_LOST) || (clientid == NULL)) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Configuration Calculagraph for Pingreq Timer */
	Stm32_Calculagraph_CountdownSec(&Pingreq_timer_s, pClient->Command_Timeout_Sec);
	
	/* Serialize Pingreq Command Buffer */
	if ((len = MQTTSNSerialize_pingreq(pClient->Sendbuf, pClient->Sendbuf_size, *clientid)) <= 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Send Pingreq Command Buffer to MqttSN Server */
	if ((MQTTSNStatus = pClient->SocketStack->Write(pClient->SocketStack, (char *)pClient->Sendbuf, len)) != MQTTSN_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* This will be a blocking call, wait for the MQTTSN_PINGRESP */
	if ((MQTTSNStatus = MQTTSN_WaitforRecvAck(pClient, MQTTSN_PINGRESP, &Pingreq_timer_s)) == MQTTSN_OK) {
		MQTTSNStatus = MQTTSN_OK;
	}
	else {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Subscribe(MQTTSN_ClientsTypeDef* pClient, const char* topicFilter, enum QoS qos, messageHandlerFp messageHandler)
 @Description			MQTTSN_Subscribe		: MQTTSN订阅主题
 @Input				pClient				: MqttSN客户端实例
					topicFilter			: MqttSN主题名
					qos					: QOS Level
					messageHandler			: MqttSN消息处理回调函数
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Subscribe(MQTTSN_ClientsTypeDef* pClient, const char* topicFilter, enum QoS qos, messageHandlerFp messageHandler)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	Stm32_CalculagraphTypeDef Subscribe_timer_s;
	int len = 0;
	
	MQTTSN_topicid topic;
	unsigned short topicid;
	unsigned short SendMsgId;
	unsigned short RecvMsgId;
	
	/* Don't send connect packet again if we are already connected */
	if ((pClient->SubState != MQTTSN_SUBSTATE_ACTIVE) || (topicFilter == NULL)) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Configuration Calculagraph for Register Timer */
	Stm32_Calculagraph_CountdownSec(&Subscribe_timer_s, pClient->Command_Timeout_Sec);
	
	topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
	topic.data.long_.name = (char *)topicFilter;
	topic.data.long_.len = strlen(topic.data.long_.name);
	SendMsgId = MQTTSN_GetNextMsgId(pClient);
	/* Serialize Subscribe Command Buffer */
	if ((len = MQTTSNSerialize_subscribe(pClient->Sendbuf, pClient->Sendbuf_size, 0, qos, SendMsgId, &topic)) <= 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* Send Subscribe Command Buffer to MqttSN Server */
	if ((MQTTSNStatus = pClient->SocketStack->Write(pClient->SocketStack, (char *)pClient->Sendbuf, len)) != MQTTSN_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	/* we should check if the suback packet, now we do not check yet */
	if ((MQTTSNStatus = MQTTSN_WaitforRecvAck(pClient, MQTTSN_SUBACK, &Subscribe_timer_s)) == MQTTSN_OK) {
		int granted_qos = -1;
		unsigned char returncode;
		MQTTSNDeserialize_suback(&granted_qos, &topicid, &RecvMsgId, &returncode, pClient->Recvbuf, pClient->Recvbuf_size);
		if (granted_qos != 2 || returncode != 0) {
			MQTTSNStatus = MQTTSN_ERROR;
			goto exit;
		}
		else {
			MQTTSNStatus = MQTTSN_MESSAGE_HANDLERS_FULL;
			for (int i = 0; i < MAX_MESSAGE_HANDLERS; i++) {
				if ((pClient->messageHandlers[i].topicFilter == NULL) || (pClient->messageHandlers[i].topicid == topicid)) {
					pClient->messageHandlers[i].topicFilter = topicFilter;
					pClient->messageHandlers[i].topicid = topicid;
					pClient->messageHandlers[i].fp = messageHandler;
					MQTTSNStatus = MQTTSN_OK;
					break;
				}
			}
		}
	}
	else {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_RecvAck(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_RecvAck			: MQTTSN接收下行消息
 @Input				pClient				: MqttSN客户端实例
 @Return				MQTTSN_StatusTypeDef	: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_RecvAck(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	int packetType = -1;
	
	MQTTSNStatus = MQTTSN_RecvPacketProcess(pClient, &packetType);
	
	return MQTTSNStatus;
}

/********************************************** END OF FLEE **********************************************/
