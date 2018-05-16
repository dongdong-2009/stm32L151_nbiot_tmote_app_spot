/**
  *********************************************************************************************************
  * @file    net_nbiot_app.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-04-26
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "net_nbiot_app.h"
#include "net_coap_app.h"
#include "net_mqttsn_app.h"
#include "stm32l1xx_config.h"
#include "platform_config.h"
#include "platform_map.h"
#include "inspectmessageoperate.h"
#include "hal_rtc.h"
#include "string.h"

NETCoapNeedSendCodeTypeDef	NETCoapNeedSendCode = NETCoapNeedSendCode_initializer;
NETMqttSNNeedSendCodeTypeDef	NETMqttSNNeedSendCode = NETMqttSNNeedSendCode_initializer;

/**********************************************************************************************************
 @Function			void NET_NBIOT_Initialization(void)
 @Description			NET_NBIOT_Initialization						: NET初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void NET_NBIOT_Initialization(void)
{
	NBIOT_Transport_Init(&NbiotATCmdHandler);												//NBIOT数据传输接口初始化
	NBIOT_Client_Init(&NbiotClientHandler, &NbiotATCmdHandler);									//NBIOT客户端初始化
	
	MQTTSN_Transport_Init(&MqttSNSocketNetHandler, &NbiotClientHandler, 4000, "106.14.142.169", 1884);	//MqttSN数据传输接口初始化
	MQTTSN_Client_Init(&MqttSNClientHandler, &MqttSNSocketNetHandler);							//MQTTSN客户端初始化
}

/**********************************************************************************************************
 @Function			void NET_NBIOT_DataProcessing(void)
 @Description			NET_NBIOT_DataProcessing						: NET数据处理
 @Input				void
 @Return				void
**********************************************************************************************************/
void NET_NBIOT_DataProcessing(void)
{
#if NETPROTOCAL == NETCOAP
	
	u32 len = 0;
	SpotStatusTypedef SpotStatusData;
	
	/* 检查是否有数据需要发送 */
	if (Inspect_Message_SpotStatusisEmpty() == false) {
		NETCoapNeedSendCode.LongStatus = 1;
	}
	
	/* COAP SHORT STATUS DATA ENQUEUE */
	if (NETCoapNeedSendCode.ShortStatus) {
		Inspect_Message_SpotStatusDequeue(&SpotStatusData);
		CoapShortStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapShortStructure.HeadPacket.DataLen				= 0x00;
		CoapShortStructure.HeadPacket.ProtocolType			= 0x00;
		CoapShortStructure.HeadPacket.Reserved1				= 0x00;
		CoapShortStructure.HeadPacket.ProtocolVersion		= 0x00;
		CoapShortStructure.HeadPacket.Reserved2				= 0x00;
		CoapShortStructure.HeadPacket.PacketType			= 0x05;
		CoapShortStructure.HeadPacket.PacketNumber			= 0x00;
		CoapShortStructure.MsgPacket.DestSN				= 0x00;
		CoapShortStructure.MsgPacket.Version				= 0x01;
		CoapShortStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_SHORT_STATUS;
		CoapShortStructure.DateTime						= SpotStatusData.unixTime;
		CoapShortStructure.SpotStatus						= SpotStatusData.spot_status;
		CoapShortStructure.SpotCount						= SpotStatusData.spot_count;
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapShortStructure, sizeof(CoapShortStructure));
		NETCoapNeedSendCode.ShortStatus = 0;
		Inspect_Message_SpotStatusOffSet();
		TCFG_Utility_Add_Coap_SentCount();
	}
	/* COAP LONG STATUS DATA ENQUEUE */
	else if (NETCoapNeedSendCode.LongStatus) {
		Inspect_Message_SpotStatusDequeue(&SpotStatusData);
		CoapLongStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapLongStructure.HeadPacket.DataLen				= 0x00;
		CoapLongStructure.HeadPacket.ProtocolType			= 0x00;
		CoapLongStructure.HeadPacket.Reserved1				= 0x00;
		CoapLongStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapLongStructure.HeadPacket.Reserved2				= 0x00;
		CoapLongStructure.HeadPacket.PacketType				= 0x05;
		CoapLongStructure.HeadPacket.PacketNumber			= 0x00;
		CoapLongStructure.MsgPacket.DestSN					= 0x00;
		CoapLongStructure.MsgPacket.Version				= 0x01;
		CoapLongStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_LONG_STATUS;
		CoapLongStructure.DateTime						= SpotStatusData.unixTime;
		CoapLongStructure.SpotStatus						= SpotStatusData.spot_status;
		CoapLongStructure.SpotCount						= SpotStatusData.spot_count;
		CoapLongStructure.MagneticX						= SpotStatusData.qmc5883lData.X_Now;
		CoapLongStructure.MagneticY						= SpotStatusData.qmc5883lData.Y_Now;
		CoapLongStructure.MagneticZ						= SpotStatusData.qmc5883lData.Z_Now;
		CoapLongStructure.MagneticDiff					= SpotStatusData.qmc5883lDiff.BackVal_Diff;
		CoapLongStructure.RadarDistance					= SpotStatusData.radarData.DisVal;
		CoapLongStructure.RadarStrength					= SpotStatusData.radarData.MagVal;
		CoapLongStructure.RadarCoverCount					= SpotStatusData.radarData.Diff_v2;
		CoapLongStructure.RadarDiff						= SpotStatusData.radarData.Diff;
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
		NETCoapNeedSendCode.LongStatus = 0;
		Inspect_Message_SpotStatusOffSet();
		TCFG_Utility_Add_Coap_SentCount();
	}
	/* COAP WORK INFO DATA ENQUEUE */
	else if (NETCoapNeedSendCode.WorkInfo) {
		memset((void*)&CoapInfoStructure.InfoData, 0, sizeof(CoapInfoStructure.InfoData));
		CoapInfoStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapInfoStructure.HeadPacket.DataLen				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolType			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved1				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved2				= 0x00;
		CoapInfoStructure.HeadPacket.PacketType				= 0x05;
		CoapInfoStructure.HeadPacket.PacketNumber			= 0x00;
		CoapInfoStructure.MsgPacket.DestSN					= 0x00;
		CoapInfoStructure.MsgPacket.Version				= 0x01;
		CoapInfoStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_INFO;
		len = NET_COAP_Message_Operate_Creat_Json_Work_Info((char *)&CoapInfoStructure.InfoData);
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapInfoStructure, sizeof(CoapInfoStructure) - sizeof(CoapInfoStructure.InfoData) + len);
		NETCoapNeedSendCode.WorkInfo = 0;
		TCFG_Utility_Add_Coap_SentCount();
	}
	/* COAP BASIC INFO DATA ENQUEUE */
	else if (NETCoapNeedSendCode.BasicInfo) {
		memset((void*)&CoapInfoStructure.InfoData, 0, sizeof(CoapInfoStructure.InfoData));
		CoapInfoStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapInfoStructure.HeadPacket.DataLen				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolType			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved1				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved2				= 0x00;
		CoapInfoStructure.HeadPacket.PacketType				= 0x05;
		CoapInfoStructure.HeadPacket.PacketNumber			= 0x00;
		CoapInfoStructure.MsgPacket.DestSN					= 0x00;
		CoapInfoStructure.MsgPacket.Version				= 0x01;
		CoapInfoStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_INFO;
		len = NET_COAP_Message_Operate_Creat_Json_Basic_Info((char *)&CoapInfoStructure.InfoData);
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapInfoStructure, sizeof(CoapInfoStructure) - sizeof(CoapInfoStructure.InfoData) + len);
		NETCoapNeedSendCode.BasicInfo = 0;
		TCFG_Utility_Add_Coap_SentCount();
	}
	/* COAP DYNAMIC INFO DATA ENQUEUE */
	else if (NETCoapNeedSendCode.DynamicInfo) {
		memset((void*)&CoapInfoStructure.InfoData, 0, sizeof(CoapInfoStructure.InfoData));
		CoapInfoStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapInfoStructure.HeadPacket.DataLen				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolType			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved1				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved2				= 0x00;
		CoapInfoStructure.HeadPacket.PacketType				= 0x05;
		CoapInfoStructure.HeadPacket.PacketNumber			= 0x00;
		CoapInfoStructure.MsgPacket.DestSN					= 0x00;
		CoapInfoStructure.MsgPacket.Version				= 0x01;
		CoapInfoStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_INFO;
		len = NET_COAP_Message_Operate_Creat_Json_Dynamic_Info((char *)&CoapInfoStructure.InfoData);
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapInfoStructure, sizeof(CoapInfoStructure) - sizeof(CoapInfoStructure.InfoData) + len);
		NETCoapNeedSendCode.DynamicInfo = 0;
		TCFG_Utility_Add_Coap_SentCount();
	}
	/* COAP RADAR INFO DATA ENQUEUE */
	else if (NETCoapNeedSendCode.RadarInfo) {
		memset((void*)&CoapInfoStructure.InfoData, 0, sizeof(CoapInfoStructure.InfoData));
		CoapInfoStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapInfoStructure.HeadPacket.DataLen				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolType			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved1				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved2				= 0x00;
		CoapInfoStructure.HeadPacket.PacketType				= 0x05;
		CoapInfoStructure.HeadPacket.PacketNumber			= 0x00;
		CoapInfoStructure.MsgPacket.DestSN					= 0x00;
		CoapInfoStructure.MsgPacket.Version				= 0x01;
		CoapInfoStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_INFO;
		len = NET_COAP_Message_Operate_Creat_Json_Radar_Info((char *)&CoapInfoStructure.InfoData);
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapInfoStructure, sizeof(CoapInfoStructure) - sizeof(CoapInfoStructure.InfoData) + len);
		NETCoapNeedSendCode.RadarInfo = 0;
		TCFG_Utility_Add_Coap_SentCount();
	}
#elif NETPROTOCAL == NETMQTTSN
	
	SpotStatusTypedef SpotStatusData;
	
	/* 检查是否有数据需要发送 */
	if (Inspect_Message_SpotStatusisEmpty() == false) {
		NETMqttSNNeedSendCode.StatusExtend = 1;
	}
	
	/* MQTTSN STATUS BASIC DATA ENQUEUE */
	if (NETMqttSNNeedSendCode.StatusBasic) {
		Inspect_Message_SpotStatusDequeue(&SpotStatusData);
		MqttSNStatusBasicStructure.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		MqttSNStatusBasicStructure.Status					= SpotStatusData.spot_status;
		MqttSNStatusBasicStructure.Count					= SpotStatusData.spot_count;
		MqttSNStatusBasicStructure.DateTime				= SpotStatusData.unixTime;
		NET_MqttSN_Message_StatusBasicEnqueue(MqttSNStatusBasicStructure);
		NETMqttSNNeedSendCode.StatusBasic = 0;
		Inspect_Message_SpotStatusOffSet();
		TCFG_Utility_Add_MqttSN_SentCount();
	}
	/* MQTTSN STATUS EXTEND DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.StatusExtend) {
		Inspect_Message_SpotStatusDequeue(&SpotStatusData);
		MqttSNStatusExtendStructure.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		MqttSNStatusExtendStructure.Status					= SpotStatusData.spot_status;
		MqttSNStatusExtendStructure.Count					= SpotStatusData.spot_count;
		MqttSNStatusExtendStructure.DateTime				= SpotStatusData.unixTime;
		MqttSNStatusExtendStructure.MagX					= SpotStatusData.qmc5883lData.X_Now;
		MqttSNStatusExtendStructure.MagY					= SpotStatusData.qmc5883lData.Y_Now;
		MqttSNStatusExtendStructure.MagZ					= SpotStatusData.qmc5883lData.Z_Now;
		MqttSNStatusExtendStructure.MagDiff				= SpotStatusData.qmc5883lDiff.BackVal_Diff;
		MqttSNStatusExtendStructure.Distance				= SpotStatusData.radarData.DisVal;
		MqttSNStatusExtendStructure.Strength				= SpotStatusData.radarData.MagVal;
		MqttSNStatusExtendStructure.CoverCount				= SpotStatusData.radarData.Diff_v2;
		MqttSNStatusExtendStructure.RadarDiff				= SpotStatusData.radarData.Diff;
		NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
		NETMqttSNNeedSendCode.StatusExtend = 0;
		Inspect_Message_SpotStatusOffSet();
		TCFG_Utility_Add_MqttSN_SentCount();
	}
	/* MQTTSN INFO WORK DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.InfoWork) {
		MqttSNInfoWorkStructure.DeviceSN					= TCFG_EEPROM_Get_MAC_SN();
		MqttSNInfoWorkStructure.Sensitivity				= TCFG_EEPROM_GetSavedSensitivity();
		MqttSNInfoWorkStructure.WorkMode					= TCFG_EEPROM_GetWorkMode();
		MqttSNInfoWorkStructure.RfChannel					= TCFG_EEPROM_GetRfChannel();
		NET_MqttSN_Message_InfoWorkEnqueue(MqttSNInfoWorkStructure);
		NETMqttSNNeedSendCode.InfoWork = 0;
		TCFG_Utility_Add_MqttSN_SentCount();
	}
	/* MQTTSN INFO BASIC DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.InfoBasic) {
		MqttSNInfoBasicStructure.DeviceSN					= TCFG_EEPROM_Get_MAC_SN();
		MqttSNInfoBasicStructure.DeviceType				= TCFG_Utility_Get_Mvb_ModelType();
		NET_MqttSN_Message_InfoBasicEnqueue(MqttSNInfoBasicStructure);
		NETMqttSNNeedSendCode.InfoBasic = 0;
		TCFG_Utility_Add_MqttSN_SentCount();
	}
	/* MQTTSN INFO DYNAMIC DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.InfoDynamic) {
		MqttSNInfoDynamicStructure.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		MqttSNInfoDynamicStructure.Runtime					= TCFG_Utility_Get_Run_Time();
		MqttSNInfoDynamicStructure.NBIotRssi				= TCFG_Utility_Get_Nbiot_Rssi_IntVal();
		MqttSNInfoDynamicStructure.DeviceBatt				= TCFG_Utility_Get_Device_Batt_ShortVal();
		MqttSNInfoDynamicStructure.RadarLib				= TCFG_Utility_Get_RadarLibNum();
		MqttSNInfoDynamicStructure.RadarCount				= TCFG_GetRadarCount();
		MqttSNInfoDynamicStructure.DeviceTemperature			= TCFG_Utility_Get_Device_Temperature();
		MqttSNInfoDynamicStructure.RadarDbgMode				= TCFG_EEPROM_GetRadarDbgMode();
		MqttSNInfoDynamicStructure.NBiotPSMEnable			= TCFG_EEPROM_GetEnableNBiotPSM();
		MqttSNInfoDynamicStructure.AlgoLib					= TCFG_Utility_Get_AlgoLibNum();
		MqttSNInfoDynamicStructure.ReInitModuleCount			= TCFG_Utility_Get_ReInitModuleCount();
		MqttSNInfoDynamicStructure.NBIotBootCount			= TCFG_Utility_Get_Nbiot_BootCount();
		MqttSNInfoDynamicStructure.NBIotSentCount			= TCFG_Utility_Get_Nbiot_SentCount();
		MqttSNInfoDynamicStructure.NBIotRecvCount			= TCFG_Utility_Get_Nbiot_RecvCount();
		MqttSNInfoDynamicStructure.MagMode					= TCFG_EEPROM_GetMagMode();
		MqttSNInfoDynamicStructure.NbiotHeart				= TCFG_EEPROM_GetNbiotHeart();
		MqttSNInfoDynamicStructure.DistanceRange			= TCFG_Utility_Get_DistanceRange();
		NET_MqttSN_Message_InfoDynamicEnqueue(MqttSNInfoDynamicStructure);
		NETMqttSNNeedSendCode.InfoDynamic = 0;
		TCFG_Utility_Add_MqttSN_SentCount();
	}
	/* MQTTSN INFO RADAR DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.InfoRadar) {
		MqttSNInfoRadarStructure.DeviceSN					= TCFG_EEPROM_Get_MAC_SN();
		NET_MqttSN_Message_InfoRadarEnqueue(MqttSNInfoRadarStructure);
		NETMqttSNNeedSendCode.InfoRadar = 0;
		TCFG_Utility_Add_MqttSN_SentCount();
	}
#endif
}

/**********************************************************************************************************
 @Function			void NET_NBIOT_TaskProcessing(void)
 @Description			NET_NBIOT_TaskProcessing						: NET工作处理
 @Input				void
 @Return				void
**********************************************************************************************************/
void NET_NBIOT_TaskProcessing(void)
{
	/* NBIOT PollExecution */
#if NETPROTOCAL == NETCOAP
	
	NET_COAP_APP_PollExecution(&NbiotClientHandler);
	
#elif NETPROTOCAL == NETMQTTSN
	
	NET_MQTTSN_APP_PollExecution(&MqttSNClientHandler);
	
#endif
}

/**********************************************************************************************************
 @Function			void NET_NBIOT_App_Task(void)
 @Description			NET_NBIOT_App_Task							: NET处理
 @Input				void
 @Return				void
**********************************************************************************************************/
void NET_NBIOT_App_Task(void)
{
	NET_NBIOT_DataProcessing();											//数据处理
	
	NET_NBIOT_TaskProcessing();											//工作处理
}

/********************************************** END OF FLEE **********************************************/
