#ifndef __NET_MQTTSN_APP_H
#define   __NET_MQTTSN_APP_H

#include "sys.h"
#include "mqttsnconfig.h"
#include "mqttsnfunc.h"
#include "mqttsntransport.h"
#include "nbiotconfig.h"
#include "nbiotfunc.h"
#include "nbiottransport.h"
#include "net_mqttsn_message_operate.h"
#include "platform_map.h"

#define MQTTSN_DEBUG_LOG_RF_PRINT											//定义开启RF输出DEBUG信息

#define MQTTSN_CLIENT_ID					TCFG_EEPROM_Get_MAC_SN_String()
#define MQTTSN_SUBSCRIBE_ID				TCFG_EEPROM_Get_MAC_SN_String()

#define MQTTSN_NBIOT_BAND_CHINA_TELECOM		ChinaTelecom
#define MQTTSN_NBIOT_BAND_CHINA_MOBILE		ChinaMobile
#define MQTTSN_NBIOT_BAND_CHINA_UNICOM		ChinaUnicom
#define MQTTSN_NBIOT_BAND				MQTTSN_NBIOT_BAND_CHINA_MOBILE

#define TOPICID_MOTESTATUS				27921							//27921 -->mvb/51/1/status
#define TOPICID_MOTEINFO					27922							//27922 -->mvb/51/1/info

#define TOPICID_PARKINGDEVICEMVB_A_STATUS	28921							//28921 -->ParkingDeviceMvb/A/status
#define TOPICID_PARKINGDEVICEMVB_A_MOTEINFO	28922							//28921 -->ParkingDeviceMvb/A/moteinfo
#define TOPICID_PARKINGDEVICEMVB_A_WORKINFO	28923							//28921 -->ParkingDeviceMvb/A/workinfo
#define TOPICID_PARKINGDEVICEMVB_A_RADARDBG	28924							//28921 -->ParkingDeviceMvb/A/radardbg

#define TOPICID_TESTING_MVBKK				29921							//29921 -->/testing/mvbkk/

#define TNET_MQTTSN_ACTIVE_DURATION		100
#define TNET_MQTTSN_SLEEP_DURATION			14400
#define TNET_MQTTSN_PINGREG_DURATION		7200
#define TNET_MQTTSN_LOST_DURATION			300

/* MQTTSN ObjectPacket */
typedef enum
{
	OBJECT_TYPE_TMOTES_STATUS_BASIC_PUT	= 0x00,							//BASIC		STATUS
	OBJECT_TYPE_TMOTES_STATUS_EXTEND_PUT	= 0x01,							//EXTEND		STATUS
	OBJECT_TYPE_TMOTES_INFO_WORK_PUT		= 0x02,							//WORK		INFO
	OBJECT_TYPE_TMOTES_INFO_BASIC_PUT		= 0x03,							//BASIC		INFO
	OBJECT_TYPE_TMOTES_INFO_DYNAMIC_PUT	= 0x04,							//DYNAMIC		INFO
	OBJECT_TYPE_TMOTES_INFO_RADAR_PUT		= 0x05							//RADAR		INFO
}NET_MQTTSN_ObjectPacketTypeDef;

MQTTSN_StatusTypeDef messageHandlerFunction(MQTTSN_ClientsTypeDef* pClient, MQTTSN_MessageDataTypeDef* messageHandler);			//MQTTSN接收处理
MQTTSN_StatusTypeDef NET_MQTTSN_SendPayloadPacket(MQTTSN_ClientsTypeDef* pClient, NET_MQTTSN_ObjectPacketTypeDef ObjectPacket);	//MQTTSN发送负载包

void NET_MQTTSN_APP_PollExecution(MQTTSN_ClientsTypeDef* pClient);				//MqttSN逻辑处理
void NET_MQTTSN_NBIOT_Event_StopMode(MQTTSN_ClientsTypeDef* pClient);				//停止模式
void NET_MQTTSN_NBIOT_Event_HardwareReboot(MQTTSN_ClientsTypeDef* pClient);		//硬件重启
void NET_MQTTSN_NBIOT_Event_ModuleCheck(MQTTSN_ClientsTypeDef* pClient);			//模块检测
void NET_MQTTSN_NBIOT_Event_ParameterConfig(MQTTSN_ClientsTypeDef* pClient);		//参数配置
void NET_MQTTSN_NBIOT_Event_SimICCIDCheck(MQTTSN_ClientsTypeDef* pClient);			//Sim卡检测
void NET_MQTTSN_NBIOT_Event_MiscEquipConfig(MQTTSN_ClientsTypeDef* pClient);		//其他配置
void NET_MQTTSN_NBIOT_Event_AttachCheck(MQTTSN_ClientsTypeDef* pClient);			//注网检查
void NET_MQTTSN_NBIOT_Event_AttachExecute(MQTTSN_ClientsTypeDef* pClient);			//注网进行
void NET_MQTTSN_NBIOT_Event_AttachInquire(MQTTSN_ClientsTypeDef* pClient);			//注网查询
void NET_MQTTSN_NBIOT_Event_PatameterCheckOut(MQTTSN_ClientsTypeDef* pClient);		//参数检出

void NET_MQTTSN_APP_ProcessExecution(MQTTSN_ClientsTypeDef* pClient);				//MqttSN协议逻辑处理
void NET_MQTTSN_Event_Init(MQTTSN_ClientsTypeDef* pClient);						//INIT
void NET_MQTTSN_Event_Disconnect(MQTTSN_ClientsTypeDef* pClient);				//DISCONNECT
void NET_MQTTSN_Event_Active(MQTTSN_ClientsTypeDef* pClient);					//ACTIVE
void NET_MQTTSN_Event_Sleep(MQTTSN_ClientsTypeDef* pClient);					//SLEEP
void NET_MQTTSN_Event_Aweak(MQTTSN_ClientsTypeDef* pClient);					//AWAKE
void NET_MQTTSN_Event_Lost(MQTTSN_ClientsTypeDef* pClient);						//LOST

#endif
