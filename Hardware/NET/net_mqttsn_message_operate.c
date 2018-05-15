/**
  *********************************************************************************************************
  * @file    net_mqttsn_message_operate.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-02
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "net_mqttsn_message_operate.h"
#include "platform_config.h"
#include "platform_map.h"
#include "stm32l1xx_config.h"
#include "hal_rtc.h"
#include "radar_api.h"
#include "tmesh_algorithm.h"
#include "string.h"

MQTTSN_SwapStatusBasicTypeDef		NETMqttSNMessageParkStatusBasic;
MQTTSN_SwapStatusExtendTypeDef	NETMqttSNMessageParkStatusExtend;
MQTTSN_SwapInfoWorkTypeDef		NETMqttSNMessageParkInfoWork;
MQTTSN_SwapInfoBasicTypeDef		NETMqttSNMessageParkInfoBasic;
MQTTSN_SwapInfoDynamicTypeDef		NETMqttSNMessageParkInfoDynamic;
MQTTSN_SwapInfoRadarTypeDef		NETMqttSNMessageParkInfoRadar;

/**********************************************************************************************************
 @Function			int NET_Message_Operate_Creat_Json_MoteStatus_Basic(char* outBuffer)
 @Description			NET_Message_Operate_Creat_Json_MoteStatus_Basic
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 256Byte >>>!!
**********************************************************************************************************/
int NET_Message_Operate_Creat_Json_MoteStatus_Basic(char* outBuffer)
{
	MQTTSN_StatusBasicTypeDef dataBuf;
	struct tm datatime;
	
	if (NET_MqttSN_Message_StatusBasicDequeue(&dataBuf) != true) {
		return 0;
	}
	
	datatime = RTC_ConvUnixToCalendar(dataBuf.DateTime + 8 * 3600);
	
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"Name\":\"TMoteStatus\","
			"\"TMoteStatus\":"
			"{"
				"\"Status\":%d,"
				"\"Count\":%d,"
				"\"Time\":\"20%02d-%02d-%02d %02d:%02d:%02d\""
			"}"
		"}",
		
		dataBuf.DeviceSN,
		dataBuf.Status,
		dataBuf.Count,
		datatime.tm_year, datatime.tm_mon, datatime.tm_mday,
		datatime.tm_hour, datatime.tm_min, datatime.tm_sec
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			int NET_Message_Operate_Creat_Json_MoteStatus_Extend(char* outBuffer)
 @Description			NET_Message_Operate_Creat_Json_MoteStatus_Extend
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 256Byte >>>!!
**********************************************************************************************************/
int NET_Message_Operate_Creat_Json_MoteStatus_Extend(char* outBuffer)
{
	MQTTSN_StatusExtendTypeDef dataBuf;
	struct tm datatime;
	
	if (NET_MqttSN_Message_StatusExtendDequeue(&dataBuf) != true) {
		return 0;
	}
	
	datatime = RTC_ConvUnixToCalendar(dataBuf.DateTime + 8 * 3600);
	
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"Name\":\"TMoteStatus\","
			"\"TMoteStatus\":"
			"{"
				"\"Status\":%d,"
				"\"Count\":%d,"
				"\"Time\":\"20%02d-%02d-%02d %02d:%02d:%02d\","
				"\"x\":%d,"
				"\"y\":%d,"
				"\"z\":%d,"
				"\"MagDiff\":%d,"
				"\"Distance\":%d,"
				"\"Strength\":%d,"
				"\"CoverCount\":%d,"
				"\"RadarDiff\":%d"
			"}"
		"}",
		
		dataBuf.DeviceSN,
		dataBuf.Status,
		dataBuf.Count,
		datatime.tm_year, datatime.tm_mon, datatime.tm_mday,
		datatime.tm_hour, datatime.tm_min, datatime.tm_sec,
		dataBuf.MagX,
		dataBuf.MagY,
		dataBuf.MagZ,
		dataBuf.MagDiff,
		dataBuf.Distance,
		dataBuf.Strength,
		dataBuf.CoverCount,
		dataBuf.RadarDiff
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			int NET_Message_Operate_Creat_Json_MoteInfo_Work(char* outBuffer)
 @Description			NET_Message_Operate_Creat_Json_MoteInfo_Work
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 256Byte >>>!!
**********************************************************************************************************/
int NET_Message_Operate_Creat_Json_MoteInfo_Work(char* outBuffer)
{
	MQTTSN_InfoWorkTypeDef dataBuf;
	
	if (NET_MqttSN_Message_InfoWorkDequeue(&dataBuf) != true) {
		return 0;
	}
	
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"WorkInfo\":"
			"{"
				"\"Sense\":%d,"
				"\"Mode\":\"%s\","
				"\"Channel\":%d"
			"}"
		"}",
		
		dataBuf.DeviceSN,
		dataBuf.Sensitivity,
		TCFG_EEPROM_Get_WorkMode_String(),
		dataBuf.RfChannel
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			int NET_Message_Operate_Creat_Json_MoteInfo_Basic(char* outBuffer)
 @Description			NET_Message_Operate_Creat_Json_MoteInfo_Basic
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 256Byte >>>!!
**********************************************************************************************************/
int NET_Message_Operate_Creat_Json_MoteInfo_Basic(char* outBuffer)
{
	MQTTSN_InfoBasicTypeDef dataBuf;
	
	if (NET_MqttSN_Message_InfoBasicDequeue(&dataBuf) != true) {
		return 0;
	}
	
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"TMoteInfo\":"
			"{"
				"\"Type\":\"%d.1\","
				"\"Vender\":\"%s\","
				"\"Hard\":\"%d\","
				"\"Soft\":\"%d.%d\","
				"\"Build\":\"%s\","
				"\"Sim\":\"%s\","
				"\"Imei\":\"%s\","
				"\"Boot\":\"%d.%d\""
			"}"
		"}",
		
		dataBuf.DeviceSN,
		dataBuf.DeviceType,
		TCFG_EEPROM_Get_Vender_String(),
		TCFG_Utility_Get_Major_Hardnumber(),
		TCFG_Utility_Get_Major_Softnumber(), TCFG_Utility_Get_Sub_Softnumber(),
		TCFG_Utility_Get_Build_Time_String(),
		TCFG_Utility_Get_Nbiot_Iccid_String(),
		TCFG_Utility_Get_Nbiot_Imei_String(),
		TCFG_Utility_Get_SoftResetFlag(), TCFG_Utility_Get_Device_BootCount()
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			int NET_Message_Operate_Creat_Json_MoteInfo_Dynamic(char* outBuffer)
 @Description			NET_Message_Operate_Creat_Json_MoteInfo_Dynamic
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 256Byte >>>!!
**********************************************************************************************************/
int NET_Message_Operate_Creat_Json_MoteInfo_Dynamic(char* outBuffer)
{
	MQTTSN_InfoDynamicTypeDef dataBuf;
	
	if (NET_MqttSN_Message_InfoDynamicDequeue(&dataBuf) != true) {
		return 0;
	}
	
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"TMoteInfo\":"
			"{"
				"\"Runtime\":%d,"
				"\"Rssi\":%d,"
				"\"Batt\":%d,"
				"\"rlib\":\"%d\","
				"\"rcnt\":%d,"
				"\"temp\":%d,"
				"\"rdgb\":%d,"
				"\"psm\":%d,"
				"\"algo\":%d,"
				"\"qmcrbt\":%d,"
				"\"nbboot\":%d,"
				"\"nbsent\":%d,"
				"\"nbrecv\":%d,"
				"\"magmod\":%d,"
				"\"nbheart\":%d,"
				"\"range\":%d"
			"}"
		"}",
		
		dataBuf.DeviceSN,
		dataBuf.Runtime,
		dataBuf.NBIotRssi,
		dataBuf.DeviceBatt,
		dataBuf.RadarLib,
		dataBuf.RadarCount,
		dataBuf.DeviceTemperature,
		dataBuf.RadarDbgMode,
		dataBuf.NBiotPSMEnable,
		dataBuf.AlgoLib,
		dataBuf.ReInitModuleCount,
		dataBuf.NBIotBootCount,
		dataBuf.NBIotSentCount,
		dataBuf.NBIotRecvCount,
		dataBuf.MagMode,
		dataBuf.NbiotHeart,
		dataBuf.DistanceRange
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			int NET_Message_Operate_Creat_Json_MoteInfo_Radar(char* outBuffer)
 @Description			NET_Message_Operate_Creat_Json_MoteInfo_Radar
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 256Byte >>>!!
**********************************************************************************************************/
int NET_Message_Operate_Creat_Json_MoteInfo_Radar(char* outBuffer)
{
	MQTTSN_InfoRadarTypeDef dataBuf;
	
	if (NET_MqttSN_Message_InfoRadarDequeue(&dataBuf) != true) {
		return 0;
	}
	
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"RadarDbg\":"
			"{"
				"\"dif\":%d,"
				"\"x\":%d,"
				"\"y\":%d,"
				"\"z\":%d,"
				"\"snr\":%d,"
				"\"rssi\":%d,"
				"\"temp\":%d,"
				"\"other\":\"%02d%02d%02d%02d%02d,%02d%02d%02d%02d%02d,%02d%02d%02d%02d%02d%02d;%02d%02d%02d%02d%02d,%02d%02d%02d.dif=%d.tm=%d.frl=%d&%d.tml=%d(%d,%d,%d)\""
				
			"}"
		"}",
		
		dataBuf.DeviceSN,
		sRadarData.Diff,
		Qmc5883lData.X_Now,
		Qmc5883lData.Y_Now,
		Qmc5883lData.Z_Now,
		TCFG_Utility_Get_Nbiot_CellSnr(),
		TCFG_Utility_Get_Nbiot_Rssi_IntVal(),
		TCFG_Utility_Get_Device_Temperature(),
		radar_targetinfo.pMagNow[2],  radar_targetinfo.pMagNow[3],  radar_targetinfo.pMagNow[4],  radar_targetinfo.pMagNow[5],
		radar_targetinfo.pMagNow[6],  radar_targetinfo.pMagNow[7],  radar_targetinfo.pMagNow[8],  radar_targetinfo.pMagNow[9],
		radar_targetinfo.pMagNow[10], radar_targetinfo.pMagNow[11], radar_targetinfo.pMagNow[12], radar_targetinfo.pMagNow[13],
		radar_targetinfo.pMagNow[14], radar_targetinfo.pMagNow[15], radar_targetinfo.pMagNow[16], radar_targetinfo.pMagNow[17],
		radar_targetinfo.pMagBG[2], radar_targetinfo.pMagBG[3], radar_targetinfo.pMagBG[4], radar_targetinfo.pMagBG[5],
		radar_targetinfo.pMagBG[6], radar_targetinfo.pMagBG[7], radar_targetinfo.pMagBG[8], radar_targetinfo.pMagBG[9],
		sRadarData.Diff_v2,
		sRadarData.timedomain_square_dif/100,
		talgo_get_fredomain_least(),
		talgo_get_fredomain_least_inhalfhour(),
		talgo_get_timedomain_least(),
		Qmc5883lData.X_Back,
		Qmc5883lData.Y_Back,
		Qmc5883lData.Z_Back
	);
	
	return strlen(outBuffer);
}

/********************************************Is Full******************************************************/
/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusBasicisFull(void)
 @Description			NET_MqttSN_Message_StatusBasicisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusBasicisFull(void)
{
	bool MessageState;
	
	if ((NETMqttSNMessageParkStatusBasic.Rear + 1) % MQTTSN_STATUS_BASIC_PARK_NUM == NETMqttSNMessageParkStatusBasic.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusExtendisFull(void)
 @Description			NET_MqttSN_Message_StatusExtendisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusExtendisFull(void)
{
	bool MessageState;
	
	if ((NETMqttSNMessageParkStatusExtend.Rear + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM == NETMqttSNMessageParkStatusExtend.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoWorkisFull(void)
 @Description			NET_MqttSN_Message_InfoWorkisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoWorkisFull(void)
{
	bool MessageState;
	
	if ((NETMqttSNMessageParkInfoWork.Rear + 1) % MQTTSN_INFO_WORK_PARK_NUM == NETMqttSNMessageParkInfoWork.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoBasicisFull(void)
 @Description			NET_MqttSN_Message_InfoBasicisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoBasicisFull(void)
{
	bool MessageState;
	
	if ((NETMqttSNMessageParkInfoBasic.Rear + 1) % MQTTSN_INFO_BASIC_PARK_NUM == NETMqttSNMessageParkInfoBasic.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoDynamicisFull(void)
 @Description			NET_MqttSN_Message_InfoDynamicisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoDynamicisFull(void)
{
	bool MessageState;
	
	if ((NETMqttSNMessageParkInfoDynamic.Rear + 1) % MQTTSN_INFO_DYNAMIC_PARK_NUM == NETMqttSNMessageParkInfoDynamic.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoRadarisFull(void)
 @Description			NET_MqttSN_Message_InfoRadarisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoRadarisFull(void)
{
	bool MessageState;
	
	if ((NETMqttSNMessageParkInfoRadar.Rear + 1) % MQTTSN_INFO_RADAR_PARK_NUM == NETMqttSNMessageParkInfoRadar.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/********************************************Is Empty*****************************************************/
/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusBasicisEmpty(void)
 @Description			NET_MqttSN_Message_StatusBasicisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusBasicisEmpty(void)
{
	bool MessageState;
	
	if (NETMqttSNMessageParkStatusBasic.Front == NETMqttSNMessageParkStatusBasic.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusExtendisEmpty(void)
 @Description			NET_MqttSN_Message_StatusExtendisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusExtendisEmpty(void)
{
	bool MessageState;
	
	if (NETMqttSNMessageParkStatusExtend.Front == NETMqttSNMessageParkStatusExtend.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoWorkisEmpty(void)
 @Description			NET_MqttSN_Message_InfoWorkisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoWorkisEmpty(void)
{
	bool MessageState;
	
	if (NETMqttSNMessageParkInfoWork.Front == NETMqttSNMessageParkInfoWork.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoBasicisEmpty(void)
 @Description			NET_MqttSN_Message_InfoBasicisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoBasicisEmpty(void)
{
	bool MessageState;
	
	if (NETMqttSNMessageParkInfoBasic.Front == NETMqttSNMessageParkInfoBasic.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoDynamicisEmpty(void)
 @Description			NET_MqttSN_Message_InfoDynamicisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoDynamicisEmpty(void)
{
	bool MessageState;
	
	if (NETMqttSNMessageParkInfoDynamic.Front == NETMqttSNMessageParkInfoDynamic.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoRadarisEmpty(void)
 @Description			NET_MqttSN_Message_InfoRadarisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoRadarisEmpty(void)
{
	bool MessageState;
	
	if (NETMqttSNMessageParkInfoRadar.Front == NETMqttSNMessageParkInfoRadar.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/******************************************** Enqueue ****************************************************/
/**********************************************************************************************************
 @Function			void NET_MqttSN_Message_StatusBasicEnqueue(StatusBasicTypeDef dataBuf)
 @Description			NET_MqttSN_Message_StatusBasicEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void NET_MqttSN_Message_StatusBasicEnqueue(MQTTSN_StatusBasicTypeDef dataBuf)
{
	if (NET_MqttSN_Message_StatusBasicisFull() == true) {															//队列已满
		NETMqttSNMessageParkStatusBasic.Rear = (NETMqttSNMessageParkStatusBasic.Rear + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;		//队尾偏移1
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].Status = dataBuf.Status;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].Count = dataBuf.Count;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].DateTime = dataBuf.DateTime;
		NETMqttSNMessageParkStatusBasic.Front = (NETMqttSNMessageParkStatusBasic.Front + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;	//队头偏移1
	}
	else {																								//队列未满
		NETMqttSNMessageParkStatusBasic.Rear = (NETMqttSNMessageParkStatusBasic.Rear + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;		//队尾偏移1
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].Status = dataBuf.Status;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].Count = dataBuf.Count;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].DateTime = dataBuf.DateTime;
	}
}

/**********************************************************************************************************
 @Function			void NET_MqttSN_Message_StatusExtendEnqueue(StatusExtendTypeDef dataBuf)
 @Description			NET_MqttSN_Message_StatusExtendEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void NET_MqttSN_Message_StatusExtendEnqueue(MQTTSN_StatusExtendTypeDef dataBuf)
{
	if (NET_MqttSN_Message_StatusExtendisFull() == true) {															//队列已满
		NETMqttSNMessageParkStatusExtend.Rear = (NETMqttSNMessageParkStatusExtend.Rear + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;	//队尾偏移1
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Status = dataBuf.Status;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Count = dataBuf.Count;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].DateTime = dataBuf.DateTime;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagX = dataBuf.MagX;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagY = dataBuf.MagY;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagZ = dataBuf.MagZ;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagDiff = dataBuf.MagDiff;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Distance = dataBuf.Distance;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Strength = dataBuf.Strength;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].CoverCount = dataBuf.CoverCount;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].RadarDiff = dataBuf.RadarDiff;
		NETMqttSNMessageParkStatusExtend.Front = (NETMqttSNMessageParkStatusExtend.Front + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;	//队头偏移1
	}
	else {																								//队列未满
		NETMqttSNMessageParkStatusExtend.Rear = (NETMqttSNMessageParkStatusExtend.Rear + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;	//队尾偏移1
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Status = dataBuf.Status;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Count = dataBuf.Count;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].DateTime = dataBuf.DateTime;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagX = dataBuf.MagX;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagY = dataBuf.MagY;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagZ = dataBuf.MagZ;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagDiff = dataBuf.MagDiff;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Distance = dataBuf.Distance;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Strength = dataBuf.Strength;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].CoverCount = dataBuf.CoverCount;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].RadarDiff = dataBuf.RadarDiff;
	}
}

/**********************************************************************************************************
 @Function			void NET_MqttSN_Message_InfoWorkEnqueue(MQTTSN_InfoWorkTypeDef dataBuf)
 @Description			NET_MqttSN_Message_InfoWorkEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void NET_MqttSN_Message_InfoWorkEnqueue(MQTTSN_InfoWorkTypeDef dataBuf)
{
	if (NET_MqttSN_Message_InfoWorkisFull() == true) {															//队列已满
		NETMqttSNMessageParkInfoWork.Rear = (NETMqttSNMessageParkInfoWork.Rear + 1) % MQTTSN_INFO_WORK_PARK_NUM;				//队尾偏移1
		NETMqttSNMessageParkInfoWork.InfoWork[NETMqttSNMessageParkInfoWork.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkInfoWork.InfoWork[NETMqttSNMessageParkInfoWork.Rear].Sensitivity = dataBuf.Sensitivity;
		NETMqttSNMessageParkInfoWork.InfoWork[NETMqttSNMessageParkInfoWork.Rear].WorkMode = dataBuf.WorkMode;
		NETMqttSNMessageParkInfoWork.InfoWork[NETMqttSNMessageParkInfoWork.Rear].RfChannel = dataBuf.RfChannel;
		NETMqttSNMessageParkInfoWork.Front = (NETMqttSNMessageParkInfoWork.Front + 1) % MQTTSN_INFO_WORK_PARK_NUM;			//队头偏移1
	}
	else {																								//队列未满
		NETMqttSNMessageParkInfoWork.Rear = (NETMqttSNMessageParkInfoWork.Rear + 1) % MQTTSN_INFO_WORK_PARK_NUM;				//队尾偏移1
		NETMqttSNMessageParkInfoWork.InfoWork[NETMqttSNMessageParkInfoWork.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkInfoWork.InfoWork[NETMqttSNMessageParkInfoWork.Rear].Sensitivity = dataBuf.Sensitivity;
		NETMqttSNMessageParkInfoWork.InfoWork[NETMqttSNMessageParkInfoWork.Rear].WorkMode = dataBuf.WorkMode;
		NETMqttSNMessageParkInfoWork.InfoWork[NETMqttSNMessageParkInfoWork.Rear].RfChannel = dataBuf.RfChannel;
	}
}

/**********************************************************************************************************
 @Function			void NET_MqttSN_Message_InfoBasicEnqueue(MQTTSN_InfoBasicTypeDef dataBuf)
 @Description			NET_MqttSN_Message_InfoBasicEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void NET_MqttSN_Message_InfoBasicEnqueue(MQTTSN_InfoBasicTypeDef dataBuf)
{
	if (NET_MqttSN_Message_InfoBasicisFull() == true) {															//队列已满
		NETMqttSNMessageParkInfoBasic.Rear = (NETMqttSNMessageParkInfoBasic.Rear + 1) % MQTTSN_INFO_BASIC_PARK_NUM;			//队尾偏移1
		NETMqttSNMessageParkInfoBasic.InfoBasic[NETMqttSNMessageParkInfoBasic.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkInfoBasic.InfoBasic[NETMqttSNMessageParkInfoBasic.Rear].DeviceType = dataBuf.DeviceType;
		NETMqttSNMessageParkInfoBasic.Front = (NETMqttSNMessageParkInfoBasic.Front + 1) % MQTTSN_INFO_BASIC_PARK_NUM;			//队头偏移1
	}
	else {																								//队列未满
		NETMqttSNMessageParkInfoBasic.Rear = (NETMqttSNMessageParkInfoBasic.Rear + 1) % MQTTSN_INFO_BASIC_PARK_NUM;			//队尾偏移1
		NETMqttSNMessageParkInfoBasic.InfoBasic[NETMqttSNMessageParkInfoBasic.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkInfoBasic.InfoBasic[NETMqttSNMessageParkInfoBasic.Rear].DeviceType = dataBuf.DeviceType;
	}
}

/**********************************************************************************************************
 @Function			void NET_MqttSN_Message_InfoDynamicEnqueue(MQTTSN_InfoDynamicTypeDef dataBuf)
 @Description			NET_MqttSN_Message_InfoDynamicEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void NET_MqttSN_Message_InfoDynamicEnqueue(MQTTSN_InfoDynamicTypeDef dataBuf)
{
	if (NET_MqttSN_Message_InfoDynamicisFull() == true) {															//队列已满
		NETMqttSNMessageParkInfoDynamic.Rear = (NETMqttSNMessageParkInfoDynamic.Rear + 1) % MQTTSN_INFO_DYNAMIC_PARK_NUM;		//队尾偏移1
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].Runtime = dataBuf.Runtime;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBIotRssi = dataBuf.NBIotRssi;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].DeviceBatt = dataBuf.DeviceBatt;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].RadarLib = dataBuf.RadarLib;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].RadarCount = dataBuf.RadarCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].DeviceTemperature = dataBuf.DeviceTemperature;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].RadarDbgMode = dataBuf.RadarDbgMode;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBiotPSMEnable = dataBuf.NBiotPSMEnable;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].AlgoLib = dataBuf.AlgoLib;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].ReInitModuleCount = dataBuf.ReInitModuleCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBIotBootCount = dataBuf.NBIotBootCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBIotSentCount = dataBuf.NBIotSentCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBIotRecvCount = dataBuf.NBIotRecvCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].MagMode = dataBuf.MagMode;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NbiotHeart = dataBuf.NbiotHeart;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].DistanceRange = dataBuf.DistanceRange;
		NETMqttSNMessageParkInfoDynamic.Front = (NETMqttSNMessageParkInfoDynamic.Front + 1) % MQTTSN_INFO_DYNAMIC_PARK_NUM;	//队头偏移1
	}
	else {																								//队列未满
		NETMqttSNMessageParkInfoDynamic.Rear = (NETMqttSNMessageParkInfoDynamic.Rear + 1) % MQTTSN_INFO_DYNAMIC_PARK_NUM;		//队尾偏移1
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].Runtime = dataBuf.Runtime;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBIotRssi = dataBuf.NBIotRssi;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].DeviceBatt = dataBuf.DeviceBatt;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].RadarLib = dataBuf.RadarLib;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].RadarCount = dataBuf.RadarCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].DeviceTemperature = dataBuf.DeviceTemperature;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].RadarDbgMode = dataBuf.RadarDbgMode;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBiotPSMEnable = dataBuf.NBiotPSMEnable;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].AlgoLib = dataBuf.AlgoLib;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].ReInitModuleCount = dataBuf.ReInitModuleCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBIotBootCount = dataBuf.NBIotBootCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBIotSentCount = dataBuf.NBIotSentCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NBIotRecvCount = dataBuf.NBIotRecvCount;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].MagMode = dataBuf.MagMode;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].NbiotHeart = dataBuf.NbiotHeart;
		NETMqttSNMessageParkInfoDynamic.InfoDynamic[NETMqttSNMessageParkInfoDynamic.Rear].DistanceRange = dataBuf.DistanceRange;
	}
}

/**********************************************************************************************************
 @Function			void NET_MqttSN_Message_InfoRadarEnqueue(MQTTSN_InfoRadarTypeDef dataBuf)
 @Description			NET_MqttSN_Message_InfoRadarEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void NET_MqttSN_Message_InfoRadarEnqueue(MQTTSN_InfoRadarTypeDef dataBuf)
{
	if (NET_MqttSN_Message_InfoRadarisFull() == true) {															//队列已满
		NETMqttSNMessageParkInfoRadar.Rear = (NETMqttSNMessageParkInfoRadar.Rear + 1) % MQTTSN_INFO_RADAR_PARK_NUM;			//队尾偏移1
		NETMqttSNMessageParkInfoRadar.InfoRadar[NETMqttSNMessageParkInfoRadar.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkInfoRadar.Front = (NETMqttSNMessageParkInfoRadar.Front + 1) % MQTTSN_INFO_RADAR_PARK_NUM;			//队头偏移1
	}
	else {																								//队列未满
		NETMqttSNMessageParkInfoRadar.Rear = (NETMqttSNMessageParkInfoRadar.Rear + 1) % MQTTSN_INFO_RADAR_PARK_NUM;			//队尾偏移1
		NETMqttSNMessageParkInfoRadar.InfoRadar[NETMqttSNMessageParkInfoRadar.Rear].DeviceSN = dataBuf.DeviceSN;
	}
}

/******************************************** Dequeue ****************************************************/
/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusBasicDequeue(StatusBasicTypeDef* dataBuf)
 @Description			NET_MqttSN_Message_StatusBasicDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusBasicDequeue(MQTTSN_StatusBasicTypeDef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_MqttSN_Message_StatusBasicisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (NETMqttSNMessageParkStatusBasic.Front + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;								//队头偏移1
		dataBuf->DeviceSN = NETMqttSNMessageParkStatusBasic.StatusBasic[front].DeviceSN;
		dataBuf->Status = NETMqttSNMessageParkStatusBasic.StatusBasic[front].Status;
		dataBuf->Count = NETMqttSNMessageParkStatusBasic.StatusBasic[front].Count;
		dataBuf->DateTime = NETMqttSNMessageParkStatusBasic.StatusBasic[front].DateTime;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusExtendDequeue(StatusExtendTypeDef* dataBuf)
 @Description			NET_MqttSN_Message_StatusExtendDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusExtendDequeue(MQTTSN_StatusExtendTypeDef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_MqttSN_Message_StatusExtendisEmpty() == true) {														//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (NETMqttSNMessageParkStatusExtend.Front + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;							//队头偏移1
		dataBuf->DeviceSN = NETMqttSNMessageParkStatusExtend.StatusExtend[front].DeviceSN;
		dataBuf->Status = NETMqttSNMessageParkStatusExtend.StatusExtend[front].Status;
		dataBuf->Count = NETMqttSNMessageParkStatusExtend.StatusExtend[front].Count;
		dataBuf->DateTime = NETMqttSNMessageParkStatusExtend.StatusExtend[front].DateTime;
		dataBuf->MagX = NETMqttSNMessageParkStatusExtend.StatusExtend[front].MagX;
		dataBuf->MagY = NETMqttSNMessageParkStatusExtend.StatusExtend[front].MagY;
		dataBuf->MagZ = NETMqttSNMessageParkStatusExtend.StatusExtend[front].MagZ;
		dataBuf->MagDiff = NETMqttSNMessageParkStatusExtend.StatusExtend[front].MagDiff;
		dataBuf->Distance = NETMqttSNMessageParkStatusExtend.StatusExtend[front].Distance;
		dataBuf->Strength = NETMqttSNMessageParkStatusExtend.StatusExtend[front].Strength;
		dataBuf->CoverCount = NETMqttSNMessageParkStatusExtend.StatusExtend[front].CoverCount;
		dataBuf->RadarDiff = NETMqttSNMessageParkStatusExtend.StatusExtend[front].RadarDiff;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoWorkDequeue(MQTTSN_InfoWorkTypeDef* dataBuf)
 @Description			NET_MqttSN_Message_InfoWorkDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoWorkDequeue(MQTTSN_InfoWorkTypeDef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_MqttSN_Message_InfoWorkisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (NETMqttSNMessageParkInfoWork.Front + 1) % MQTTSN_INFO_WORK_PARK_NUM;									//队头偏移1
		dataBuf->DeviceSN = NETMqttSNMessageParkInfoWork.InfoWork[front].DeviceSN;
		dataBuf->Sensitivity = NETMqttSNMessageParkInfoWork.InfoWork[front].Sensitivity;
		dataBuf->WorkMode = NETMqttSNMessageParkInfoWork.InfoWork[front].WorkMode;
		dataBuf->RfChannel = NETMqttSNMessageParkInfoWork.InfoWork[front].RfChannel;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoBasicDequeue(MQTTSN_InfoBasicTypeDef* dataBuf)
 @Description			NET_MqttSN_Message_InfoBasicDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoBasicDequeue(MQTTSN_InfoBasicTypeDef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_MqttSN_Message_InfoBasicisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (NETMqttSNMessageParkInfoBasic.Front + 1) % MQTTSN_INFO_BASIC_PARK_NUM;									//队头偏移1
		dataBuf->DeviceSN = NETMqttSNMessageParkInfoBasic.InfoBasic[front].DeviceSN;
		dataBuf->DeviceType = NETMqttSNMessageParkInfoBasic.InfoBasic[front].DeviceType;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoDynamicDequeue(MQTTSN_InfoDynamicTypeDef* dataBuf)
 @Description			NET_MqttSN_Message_InfoDynamicDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoDynamicDequeue(MQTTSN_InfoDynamicTypeDef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_MqttSN_Message_InfoDynamicisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (NETMqttSNMessageParkInfoDynamic.Front + 1) % MQTTSN_INFO_DYNAMIC_PARK_NUM;								//队头偏移1
		dataBuf->DeviceSN = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].DeviceSN;
		dataBuf->Runtime = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].Runtime;
		dataBuf->NBIotRssi = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].NBIotRssi;
		dataBuf->DeviceBatt = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].DeviceBatt;
		dataBuf->RadarLib = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].RadarLib;
		dataBuf->RadarCount = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].RadarCount;
		dataBuf->DeviceTemperature = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].DeviceTemperature;
		dataBuf->RadarDbgMode = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].RadarDbgMode;
		dataBuf->NBiotPSMEnable = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].NBiotPSMEnable;
		dataBuf->AlgoLib = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].AlgoLib;
		dataBuf->ReInitModuleCount = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].ReInitModuleCount;
		dataBuf->NBIotBootCount = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].NBIotBootCount;
		dataBuf->NBIotSentCount = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].NBIotSentCount;
		dataBuf->NBIotRecvCount = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].NBIotRecvCount;
		dataBuf->MagMode = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].MagMode;
		dataBuf->NbiotHeart = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].NbiotHeart;
		dataBuf->DistanceRange = NETMqttSNMessageParkInfoDynamic.InfoDynamic[front].DistanceRange;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoRadarDequeue(MQTTSN_InfoRadarTypeDef* dataBuf)
 @Description			NET_MqttSN_Message_InfoRadarDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoRadarDequeue(MQTTSN_InfoRadarTypeDef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_MqttSN_Message_InfoRadarisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (NETMqttSNMessageParkInfoRadar.Front + 1) % MQTTSN_INFO_RADAR_PARK_NUM;									//队头偏移1
		dataBuf->DeviceSN = NETMqttSNMessageParkInfoRadar.InfoRadar[front].DeviceSN;
		MessageState = true;
	}
	
	return MessageState;
}

/********************************************* OffSet ****************************************************/
/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusBasicOffSet(void)
 @Description			NET_MqttSN_Message_StatusBasicOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusBasicOffSet(void)
{
	bool MessageState;
	
	if (NET_MqttSN_Message_StatusBasicisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		NETMqttSNMessageParkStatusBasic.Front = (NETMqttSNMessageParkStatusBasic.Front + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusExtendOffSet(void)
 @Description			NET_MqttSN_Message_StatusExtendOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusExtendOffSet(void)
{
	bool MessageState;
	
	if (NET_MqttSN_Message_StatusExtendisEmpty() == true) {														//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		NETMqttSNMessageParkStatusExtend.Front = (NETMqttSNMessageParkStatusExtend.Front + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoWorkOffSet(void)
 @Description			NET_MqttSN_Message_InfoWorkOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoWorkOffSet(void)
{
	bool MessageState;
	
	if (NET_MqttSN_Message_InfoWorkisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		NETMqttSNMessageParkInfoWork.Front = (NETMqttSNMessageParkInfoWork.Front + 1) % MQTTSN_INFO_WORK_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoBasicOffSet(void)
 @Description			NET_MqttSN_Message_InfoBasicOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoBasicOffSet(void)
{
	bool MessageState;
	
	if (NET_MqttSN_Message_InfoBasicisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		NETMqttSNMessageParkInfoBasic.Front = (NETMqttSNMessageParkInfoBasic.Front + 1) % MQTTSN_INFO_BASIC_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoDynamicOffSet(void)
 @Description			NET_MqttSN_Message_InfoDynamicOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoDynamicOffSet(void)
{
	bool MessageState;
	
	if (NET_MqttSN_Message_InfoDynamicisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		NETMqttSNMessageParkInfoDynamic.Front = (NETMqttSNMessageParkInfoDynamic.Front + 1) % MQTTSN_INFO_DYNAMIC_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_InfoRadarOffSet(void)
 @Description			NET_MqttSN_Message_InfoRadarOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_InfoRadarOffSet(void)
{
	bool MessageState;
	
	if (NET_MqttSN_Message_InfoRadarisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		NETMqttSNMessageParkInfoRadar.Front = (NETMqttSNMessageParkInfoRadar.Front + 1) % MQTTSN_INFO_RADAR_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/********************************************* Rear *****************************************************/
/**********************************************************************************************************
 @Function			unsigned char NET_MqttSN_Message_StatusBasicRear(void)
 @Description			NET_MqttSN_Message_StatusBasicRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char NET_MqttSN_Message_StatusBasicRear(void)
{
	return NETMqttSNMessageParkStatusBasic.Rear;
}

/**********************************************************************************************************
 @Function			unsigned char NET_MqttSN_Message_StatusExtendRear(void)
 @Description			NET_MqttSN_Message_StatusExtendRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char NET_MqttSN_Message_StatusExtendRear(void)
{
	return NETMqttSNMessageParkStatusExtend.Rear;
}

/**********************************************************************************************************
 @Function			unsigned char NET_MqttSN_Message_InfoWorkRear(void)
 @Description			NET_MqttSN_Message_InfoWorkRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char NET_MqttSN_Message_InfoWorkRear(void)
{
	return NETMqttSNMessageParkInfoWork.Rear;
}

/**********************************************************************************************************
 @Function			unsigned char NET_MqttSN_Message_InfoBasicRear(void)
 @Description			NET_MqttSN_Message_InfoBasicRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char NET_MqttSN_Message_InfoBasicRear(void)
{
	return NETMqttSNMessageParkInfoBasic.Rear;
}

/**********************************************************************************************************
 @Function			unsigned char NET_MqttSN_Message_InfoDynamicRear(void)
 @Description			NET_MqttSN_Message_InfoDynamicRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char NET_MqttSN_Message_InfoDynamicRear(void)
{
	return NETMqttSNMessageParkInfoDynamic.Rear;
}

/**********************************************************************************************************
 @Function			unsigned char NET_MqttSN_Message_InfoRadarRear(void)
 @Description			NET_MqttSN_Message_InfoRadarRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char NET_MqttSN_Message_InfoRadarRear(void)
{
	return NETMqttSNMessageParkInfoRadar.Rear;
}

/********************************************** END OF FLEE **********************************************/
