#ifndef __NET_COAP_MESSAGE_OPERATE_H
#define   __NET_COAP_MESSAGE_OPERATE_H

#include "sys.h"

#define COAP_MSGTYPE_TYPE_SHORT_STATUS		0x37
#define COAP_MSGTYPE_TYPE_LONG_STATUS		0x3A
#define COAP_MSGTYPE_TYPE_INFO			0x35

typedef __packed struct
{
	u8	DataLen;
	u8	ProtocolType:3;
	u8	Reserved1:2;
	u8	ProtocolVersion:3;
	u8	Reserved2:4;
	u8	PacketType:4;
	u8	PacketNumber;
	u32	DeviceSN;
}COAP_PacketHeadTypeDef;													//数据包头8Byte

typedef __packed struct
{
	u32	DestSN;
	u8	Version;
	u8	Type;
}COAP_PacketMsgTypeDef;													//消息包头6Byte

typedef __packed struct
{
	COAP_PacketHeadTypeDef HeadPacket;
	COAP_PacketMsgTypeDef MsgPacket;
	u32	DateTime;
	u16	SpotCount;
	u8	SpotStatus;
}COAP_PacketShortTypeDef;												//短包21Byte

typedef __packed struct
{
	COAP_PacketHeadTypeDef HeadPacket;
	COAP_PacketMsgTypeDef MsgPacket;
	u32	DateTime;
	u16	SpotCount;
	u8	SpotStatus;
	u16	MagneticX;
	u16	MagneticY;
	u16	MagneticZ;
	u16	MagneticDiff;
	u8	RadarDistance;
	u8	RadarStrength;
	u8	RadarCoverCount;
	u8	RadarDiff;
}COAP_PacketLongTypeDef;													//长包33Byte

typedef __packed struct
{
	COAP_PacketHeadTypeDef HeadPacket;
	COAP_PacketMsgTypeDef MsgPacket;
	u8	InfoData[240];
}COAP_PacketInfoTypeDef;													//Info包254Byte

#define COAP_SEND_BUFFER_SIZE				256
#define COAP_RECV_BUFFER_SIZE				64
#define COAP_SEND_PARK_NUM				10
#define COAP_RECV_PARK_NUM				5

/* 发送数据队列 */
typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	
	struct SendParkTypeDef
	{
		unsigned char					Buffer[COAP_SEND_BUFFER_SIZE];
		unsigned short					Length;
	}Park[COAP_SEND_PARK_NUM];
}COAP_SwapSendDataTypeDef;

/* 接收数据队列 */
typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	
	struct RecvParkTypeDef
	{
		unsigned char					Buffer[COAP_RECV_BUFFER_SIZE];
		unsigned short					Length;
	}Park[COAP_RECV_PARK_NUM];
}COAP_SwapRecvDataTypeDef;


int NET_COAP_Message_Operate_Creat_Json_Work_Info(char* outBuffer);
int NET_COAP_Message_Operate_Creat_Json_Basic_Info(char* outBuffer);
int NET_COAP_Message_Operate_Creat_Json_Dynamic_Info(char* outBuffer);
int NET_COAP_Message_Operate_Creat_Json_Radar_Info(char* outBuffer);

void NET_Coap_Message_SendDataEnqueue(unsigned char* dataBuf, unsigned short dataLength);							//发送数据写入队列
void NET_Coap_Message_RecvDataEnqueue(unsigned char* dataBuf, unsigned short dataLength);							//接收数据写入队列
bool NET_Coap_Message_SendDataDequeue(unsigned char* dataBuf, unsigned short* dataLength);						//发送数据读出队列
bool NET_Coap_Message_RecvDataDequeue(unsigned char* dataBuf, unsigned short* dataLength);						//接收数据读出队列
bool NET_Coap_Message_SendDataOffSet(void);																//发送数据队列(队列头偏移1)
bool NET_Coap_Message_RecvDataOffSet(void);																//接收数据队列(队列头偏移1)
unsigned char NET_Coap_Message_SendDataRear(void);														//发送数据队尾值
unsigned char NET_Coap_Message_RecvDataRear(void);														//接收数据队尾值

#endif
