#ifndef __NET_MQTTSN_MESSAGE_OPERATE_H
#define   __NET_MQTTSN_MESSAGE_OPERATE_H

#include "sys.h"

#define NET_MESSAGE_GET_MAGICNUM(Byte)		{Byte[0]='T';Byte[1]='C';Byte[2]='L';Byte[3]='D';}
#define MESSAGE_VERSION					'1'

typedef enum
{
	MSG_JSON							= '1',
	MSG_FIRMWARE						= '3'
}NET_Message_MsgType_TypeDef;

typedef enum
{
	ENCRYPT_NONE						= '0',
	ENCRYPT_AES128						= '1',
	ENCRYPT_GZIP						= '2'
}NET_Message_EncryptMode_TypeDef;

typedef enum
{
	LOGIN_REQ							= '0',
	LOGIN_RSP							= '1',
	LOGOUT_REQ						= '2',
	LOGOUT_RSP						= '3',
	
	KEEPALIVE_REQ						= '4',
	KEEPALIVE_RSP						= '5',
	SYSINFO_REQ						= '6',
	SYSINFO_RSP						= '7',
	CONFIG_SET						= 'S',
	CONFIG_SET_RSP						= 's',
	CONFIG_GET						= 'G',
	CONFIG_GET_RSP						= 'g',
	
	MODIFYUSER_REQ						= 'c',
	MODIFYUSER_RSP						= 'd',
	MODIFYPASSWORD_REQ					= 'e',
	MODIFYPASSWORD_RSP					= 'f',
	
	CLOUD_VERSION_REQ					= 'g',
	CLOUD_VERSION_RSP					= 'i',
	CLOUD_UPGRADE_START_REQ				= 'j',
	CLOUD_UPGRADE_START_RSP				= 'k',
	CLOUD_UPGRADE_STOP_REQ				= 'l',
	CLOUD_UPGRADE_STOP_RSP				= 'm',
	CLOUD_UPGRADE_PROGRESS				= 'n',
	CLOUD_UPGRADE_DATA					= 'U',
	CLOUD_UPGRADE_DATA_RSP				= 'u',
	
	MSGID_POST						= 'S',
	MSGID_PUT							= 'P'
}NET_Message_CtrlMsg_TypeDef;

typedef __packed struct
{
	unsigned char						MagicNum[4];
	unsigned char						MsgType;
	unsigned char						Version;
	unsigned char						EncryptMode;
	unsigned char						MsgId;
}NET_Message_TcldHeadTypeDef;

typedef __packed struct
{
	NET_Message_TcldHeadTypeDef			MsgHead;
	char								pData[1];
}NET_Message_TcldMsgTypeDef;

#define MQTTSN_STATUS_BASIC_PARK_NUM		10
#define MQTTSN_STATUS_EXTEND_PARK_NUM		10
#define MQTTSN_INFO_WORK_PARK_NUM			3
#define MQTTSN_INFO_BASIC_PARK_NUM			3
#define MQTTSN_INFO_DYNAMIC_PARK_NUM		3
#define MQTTSN_INFO_RADAR_PARK_NUM			3

/* MqttSN Status Basic */
typedef __packed struct
{
	unsigned int						DeviceSN;
	unsigned char						Status;
	unsigned int						Count;
	unsigned int						DateTime;
}MQTTSN_StatusBasicTypeDef;

typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	MQTTSN_StatusBasicTypeDef			StatusBasic[MQTTSN_STATUS_BASIC_PARK_NUM];
}MQTTSN_SwapStatusBasicTypeDef;

/* MqttSN Status Extend */
typedef __packed struct
{
	unsigned int						DeviceSN;
	unsigned char						Status;
	unsigned int						Count;
	unsigned int						DateTime;
	int								MagX;
	int								MagY;
	int								MagZ;
	int								MagDiff;
	int								Distance;
	int								Strength;
	int								CoverCount;
	int								RadarDiff;
}MQTTSN_StatusExtendTypeDef;

typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	MQTTSN_StatusExtendTypeDef			StatusExtend[MQTTSN_STATUS_EXTEND_PARK_NUM];
}MQTTSN_SwapStatusExtendTypeDef;

/* MqttSN Info Work */
typedef __packed struct
{
	unsigned int						DeviceSN;
	unsigned char						Sensitivity;
	unsigned char						WorkMode;
	unsigned char						RfChannel;
}MQTTSN_InfoWorkTypeDef;

typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	MQTTSN_InfoWorkTypeDef				InfoWork[MQTTSN_INFO_WORK_PARK_NUM];
}MQTTSN_SwapInfoWorkTypeDef;

/* MqttSN Info Basic */
typedef __packed struct
{
	unsigned int						DeviceSN;
	unsigned char						DeviceType;
}MQTTSN_InfoBasicTypeDef;

typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	MQTTSN_InfoBasicTypeDef				InfoBasic[MQTTSN_INFO_BASIC_PARK_NUM];
}MQTTSN_SwapInfoBasicTypeDef;

/* MqttSN Info Dynamic */
typedef __packed struct
{
	unsigned int						DeviceSN;
	unsigned int						Runtime;
	int								NBIotRssi;
	unsigned short						DeviceBatt;
	unsigned char						RadarLib;
	unsigned int						RadarCount;
	short							DeviceTemperature;
	unsigned char						RadarDbgMode;
	unsigned char						NBiotPSMEnable;
	unsigned char						AlgoLib;
	unsigned short						ReInitModuleCount;
	unsigned int						NBIotBootCount;
	unsigned int						NBIotSentCount;
	unsigned int 						NBIotRecvCount;
	unsigned char						MagMode;
	unsigned char						NbiotHeart;
	unsigned short						DistanceRange;
}MQTTSN_InfoDynamicTypeDef;

typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	MQTTSN_InfoDynamicTypeDef			InfoDynamic[MQTTSN_INFO_DYNAMIC_PARK_NUM];
}MQTTSN_SwapInfoDynamicTypeDef;

/* MqttSN Info Radar */
typedef __packed struct
{
	unsigned int						DeviceSN;
}MQTTSN_InfoRadarTypeDef;

typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	MQTTSN_InfoRadarTypeDef				InfoRadar[MQTTSN_INFO_RADAR_PARK_NUM];
}MQTTSN_SwapInfoRadarTypeDef;


int NET_Message_Operate_Creat_Json_MoteStatus_Basic(char* outBuffer);
int NET_Message_Operate_Creat_Json_MoteStatus_Extend(char* outBuffer);
int NET_Message_Operate_Creat_Json_MoteInfo_Work(char* outBuffer);
int NET_Message_Operate_Creat_Json_MoteInfo_Basic(char* outBuffer);
int NET_Message_Operate_Creat_Json_MoteInfo_Dynamic(char* outBuffer);
int NET_Message_Operate_Creat_Json_MoteInfo_Radar(char* outBuffer);

bool NET_MqttSN_Message_StatusBasicisFull(void);
bool NET_MqttSN_Message_StatusExtendisFull(void);
bool NET_MqttSN_Message_InfoWorkisFull(void);
bool NET_MqttSN_Message_InfoBasicisFull(void);
bool NET_MqttSN_Message_InfoDynamicisFull(void);
bool NET_MqttSN_Message_InfoRadarisFull(void);

bool NET_MqttSN_Message_StatusBasicisEmpty(void);
bool NET_MqttSN_Message_StatusExtendisEmpty(void);
bool NET_MqttSN_Message_InfoWorkisEmpty(void);
bool NET_MqttSN_Message_InfoBasicisEmpty(void);
bool NET_MqttSN_Message_InfoDynamicisEmpty(void);
bool NET_MqttSN_Message_InfoRadarisEmpty(void);

void NET_MqttSN_Message_StatusBasicEnqueue(MQTTSN_StatusBasicTypeDef dataBuf);
void NET_MqttSN_Message_StatusExtendEnqueue(MQTTSN_StatusExtendTypeDef dataBuf);
void NET_MqttSN_Message_InfoWorkEnqueue(MQTTSN_InfoWorkTypeDef dataBuf);
void NET_MqttSN_Message_InfoBasicEnqueue(MQTTSN_InfoBasicTypeDef dataBuf);
void NET_MqttSN_Message_InfoDynamicEnqueue(MQTTSN_InfoDynamicTypeDef dataBuf);
void NET_MqttSN_Message_InfoRadarEnqueue(MQTTSN_InfoRadarTypeDef dataBuf);

bool NET_MqttSN_Message_StatusBasicDequeue(MQTTSN_StatusBasicTypeDef* dataBuf);
bool NET_MqttSN_Message_StatusExtendDequeue(MQTTSN_StatusExtendTypeDef* dataBuf);
bool NET_MqttSN_Message_InfoWorkDequeue(MQTTSN_InfoWorkTypeDef* dataBuf);
bool NET_MqttSN_Message_InfoBasicDequeue(MQTTSN_InfoBasicTypeDef* dataBuf);
bool NET_MqttSN_Message_InfoDynamicDequeue(MQTTSN_InfoDynamicTypeDef* dataBuf);
bool NET_MqttSN_Message_InfoRadarDequeue(MQTTSN_InfoRadarTypeDef* dataBuf);

bool NET_MqttSN_Message_StatusBasicOffSet(void);
bool NET_MqttSN_Message_StatusExtendOffSet(void);
bool NET_MqttSN_Message_InfoWorkOffSet(void);
bool NET_MqttSN_Message_InfoBasicOffSet(void);
bool NET_MqttSN_Message_InfoDynamicOffSet(void);
bool NET_MqttSN_Message_InfoRadarOffSet(void);

unsigned char NET_MqttSN_Message_StatusBasicRear(void);
unsigned char NET_MqttSN_Message_StatusExtendRear(void);
unsigned char NET_MqttSN_Message_InfoWorkRear(void);
unsigned char NET_MqttSN_Message_InfoBasicRear(void);
unsigned char NET_MqttSN_Message_InfoDynamicRear(void);
unsigned char NET_MqttSN_Message_InfoRadarRear(void);

#endif
