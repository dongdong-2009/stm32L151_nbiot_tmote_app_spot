/**
  *********************************************************************************************************
  * @file    tmesh_rf_app.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-29
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "radio_rf_app.h"
#include "radio_hal_rf.h"
#include "radio_msg_config.h"
#include "radio.h"
#include "tmesh_xtea.h"
#include "platform_map.h"
#include "platform_config.h"
#include "stm32l1xx_config.h"
#include "hal_beep.h"
#include "hal_qmc5883l.h"
#include "radar_api.h"
#include "inspectconfig.h"
#include "net_nbiot_app.h"
#include "delay.h"
#include "string.h"
#include <stdarg.h>

#include "tmesh_algorithm.h"

static frameInfo_t sInFrameQ[SIZE_INFRAME_Q];

unsigned char TRF_SendBuf[RF_BUFFER_SIZE];
unsigned char TRF_RecvBuf[RF_BUFFER_SIZE];
unsigned char TRF_PrintfBuf[RF_PRINTF_BUFFER_SIZE];

/**********************************************************************************************************
 @Function			void Radio_Rf_QInit(void)
 @Description			Radio_Rf_QInit
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_QInit(void)
{
	memset(sInFrameQ, 0, sizeof(sInFrameQ));
}

/**********************************************************************************************************
 @Function			void Radio_Rf_QPush(mrfiPacket_t* rfpacket)
 @Description			Radio_Rf_QPush
 @Input				rfpacket
 @Return				void
**********************************************************************************************************/
void Radio_Rf_QPush(mrfiPacket_t* rfpacket)
{
	int8_t i, oldest = 0, avail = -1;
	
	for (i = 0; i < SIZE_INFRAME_Q; i++) {
		if (sInFrameQ[i].fi_usage == FI_AVAILABLE) {
			avail = i;
		}
		if (sInFrameQ[i].orderStamp < sInFrameQ[oldest].orderStamp) {
			oldest = i;
		}
	}
	
	if (avail < 0) {
		avail = oldest;
	}
	
	memcpy(&sInFrameQ[avail].mrfiPkt, rfpacket, sizeof(mrfiPacket_t));
	sInFrameQ[avail].fi_usage = FI_INUSE_UNTIL_DEL;
	sInFrameQ[avail].orderStamp = sInFrameQ[oldest].orderStamp++;
}

/**********************************************************************************************************
 @Function			mrfiPacket_t* Radio_Rf_QPop(void)
 @Description			Radio_Rf_QPop
 @Input				void
 @Return				rfpacket
**********************************************************************************************************/
mrfiPacket_t* Radio_Rf_QPop(void)
{
	int8_t i;
	
	for (i = 0; i < SIZE_INFRAME_Q; i++) {
		if (sInFrameQ[i].fi_usage == FI_INUSE_UNTIL_DEL) {
			sInFrameQ[i].fi_usage = FI_AVAILABLE;
			return &sInFrameQ[i].mrfiPkt;
		}
	}
	
	return NULL;
}

/**********************************************************************************************************
 @Function			char Radio_Rf_is_Sleep(void)
 @Description			Radio_Rf_is_Sleep			: set the radio to sleep mode
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_BeforeSleep(void)
{
#ifdef	RADIO_SI4438
	if (Radio_Rf_is_Sleep() == 0) {
		Radio_Rf_Sleep();
		Radio_Rf_Interface_Deinit();
	}
#endif
}

/**********************************************************************************************************
 @Function			void Radio_Rf_Data_Handle_Isr(mrfiPacket_t* rfpacket)
 @Description			Radio_Rf_Data_Handle_Isr
 @Input				rfpacket
 @Return				void
**********************************************************************************************************/
void Radio_Rf_Data_Handle_Isr(mrfiPacket_t* rfpacket)
{
	Radio_Rf_QPush(rfpacket);
}

/**********************************************************************************************************
 @Function			char Radio_Rf_Receive(uint8_t *outmsg, uint8_t *len)
 @Description			Radio_Rf_Receive
 @Input				outmsg
					len
 @Return				void
**********************************************************************************************************/
char Radio_Rf_Receive(uint8_t *outmsg, uint8_t *len)
{
	mrfiPacket_t* rfpacket;
	rfpacket = Radio_Rf_QPop();
	
	if (rfpacket != NULL) {
		*len = 1 + (CFG_GET_FROM_FRAME(CFG_P_FRAME_LEN(rfpacket->frame), CFG_LEN_OS));
		memcpy(outmsg, rfpacket->frame, *len);
		
		return TRF_SUCCESS;
	}
	
	return TRF_NO_PAYLOAD;
}

/**********************************************************************************************************
 @Function			char Radio_Rf_Send(uint8_t *inmsg, uint8_t len)
 @Description			Radio_Rf_Send
 @Input				inmsg
					len
 @Return				void
**********************************************************************************************************/
char Radio_Rf_Send(uint8_t *inmsg, uint8_t len)
{
	static char error_cnt = 0;
	char rc;
	
	rc = Radio_Rf_PrepareToTx(inmsg, len);
	if (rc == TRF_ERROR) {
		error_cnt++;
		if (error_cnt > 30) {
			trf_status = TRF_ERROR;
		}
		else if ((error_cnt % 3) == 0) {
			/* 初始化地磁和无线 */
			ReInitModule();
		}
	}
	else {
		error_cnt = 0;
	}
	
	return rc;
}

/**********************************************************************************************************
 @Function			char Radio_Rf_Operate_Recvmsg(uint8_t *inmsg, uint8_t len)
 @Description			Radio_Rf_Operate_Recvmsg
 @Input				inmsg
					len
 @Return				void
**********************************************************************************************************/
char Radio_Rf_Operate_Recvmsg(uint8_t *inmsg, uint8_t len)
{
	char rc = TRF_SUCCESS;
	trf_msg_s* pPayload;
	
	unsigned int mac_sn = 0;
	unsigned int uval32 = 0;
	unsigned short int uval16 = 0;
	
	mac_sn = TCFG_EEPROM_Get_MAC_SN();
	
	if (CFG_GET_FROM_FRAME(CFG_P_FRAME_PRE(inmsg), CFG_PRE_TYPE_OS) == XMESHCFG_PRT)
	{
		pPayload = (trf_msg_s*)CFG_P_FRAME_PAYLOAD(inmsg);
		if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_SET)
		{
			uint32_t cnt;
			uint8_t payload_len;
			
			BEEP_CtrlRepeat_Extend(1, 300, 0);
			cnt = CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_PKTNUM_OS);
			payload_len = CFG_GET_PAYLOAD_LEN(inmsg) + CFG_FRAME_LEN_SIZE;
			
			tmesh_securityMix(mac_sn);
			tmesh_decipher((uint8_t*)pPayload, payload_len , &cnt);
			
			if (pPayload->head.destSN != mac_sn) {
				rc = TRF_NOT_FORME;
				return rc;
			}
			
			/* 升级指令 */
			if (pPayload->head.type == TRF_MSG_UPGRADE) {
				for (int i = 0; i < 5; i++) {
					TCFG_EEPROM_SetBootMode(TCFG_ENV_BOOTMODE_TOUPDATE);
					if (TCFG_ENV_BOOTMODE_TOUPDATE == TCFG_EEPROM_GetBootMode()) {
						Radio_Trf_Default_Resp(100, TRF_MSG_UPGRADE);
						BEEP_CtrlRepeat_Extend(5, 25, 25);
						Stm32_System_Software_Reboot();
					}
				}
			}
			/* 传感器灵敏度配置指令 */
			else if (pPayload->head.type == TRF_MSG_SENSITIVITY) {
				TCFG_SystemData.Sensitivity = ((tmote_sensitivity_s*)CFG_P_FRAME_PAYLOAD(inmsg))->sensitivity;
				if ((TCFG_SystemData.Sensitivity > SENSE_LOWEST) || (TCFG_SystemData.Sensitivity < SENSE_HIGHEST)) {
					TCFG_SystemData.Sensitivity = SENSE_MIDDLE;
					TCFG_EEPROM_SetSavedSensitivity(TCFG_SystemData.Sensitivity);
				}
				else {
					TCFG_EEPROM_SetSavedSensitivity(TCFG_SystemData.Sensitivity);
				}
				Radio_Trf_Printf("Inspect Spot Sensitivity : %hu", TCFG_EEPROM_GetSavedSensitivity());
				__NOP();
			}
			/* 工作模式配置指令 */
			else if (pPayload->head.type == TRF_MSG_WORKMODE) {
				TCFG_SystemData.WorkMode = ((tmote_work_mode_s*)CFG_P_FRAME_PAYLOAD(inmsg))->mode;
				if ((TCFG_SystemData.WorkMode != DEBUG_WORK) && (TCFG_SystemData.WorkMode != NORMAL_WORK)) {
					TCFG_SystemData.WorkMode = NORMAL_WORK;
					TCFG_EEPROM_SetWorkMode(TCFG_SystemData.WorkMode);
				}
				else {
					TCFG_EEPROM_SetWorkMode(TCFG_SystemData.WorkMode);
				}
				Radio_Trf_Printf("Work Mode : %hu", TCFG_EEPROM_GetWorkMode());
				__NOP();
			}
			/* 无线心跳间隔时间配置指令 */
			else if (pPayload->head.type == TRF_MSG_RFHEART_INTERVAL) {
				TCFG_SystemData.Heartinterval = ((tmote_beat_interval_s*)CFG_P_FRAME_PAYLOAD(inmsg))->seconds;
				if ((TCFG_SystemData.Heartinterval > 120) || (TCFG_SystemData.Heartinterval < 10)) {
					TCFG_SystemData.Heartinterval = HEART_INTERVAL;
					TCFG_EEPROM_SetHeartinterval(TCFG_SystemData.Heartinterval);
				}
				else {
					TCFG_EEPROM_SetHeartinterval(TCFG_SystemData.Heartinterval);
				}
				Radio_Trf_Printf("RF Heartinter val : %hu", TCFG_EEPROM_GetHeartinterval());
				__NOP();
			}
			/* 初始化传感器指令 */
			else if (pPayload->head.type == TRF_MSG_INITBACKGROUND) {
				Radar_InitBackground(TO_SAVE_RADAR_BACKGROUND);
				QMC5883L_InitBackgroud();
				Radio_Trf_Printf("Inspect Spot Background Init");
				__NOP();
			}
			/* 其他下行指令 */
			else if (pPayload->head.type == TRF_MSG_GENERAL_CMD) {
				((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf[15] = 0;
				/* Reboot */
				if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "reboot")) {
					BEEP_CtrlRepeat_Extend(2, 500, 250);
					Radio_Trf_Printf("Reboot Success");
					Stm32_System_Software_Reboot();
					__NOP();
				}
				/* NewSn */
				else if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "newsn")) {
					sscanf(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "newsn:%08x", &uval32);
					TCFG_EEPROM_Set_MAC_SN(uval32);
					Radio_Trf_Printf("New SN : %08x", TCFG_EEPROM_Get_MAC_SN());
					__NOP();
				}
				/* Active */
				else if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "active")) {
					sscanf(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "active:%hu", &uval16);
					TCFG_EEPROM_SetActiveDevice(uval16);
					Radio_Trf_Printf("Active : %hu", TCFG_EEPROM_GetActiveDevice());
					if (uval16) {
						DeviceActivedMode = true;
						BEEP_CtrlRepeat_Extend(5,30,70);
					}
					else {
						DeviceActivedMode = false;
						BEEP_CtrlRepeat_Extend(1,500,0);
					}
					__NOP();
				}
				/* RadarDbg */
				else if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "radardbg")) {
					sscanf(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "radardbg:%hu", &uval16);
					TCFG_EEPROM_SetRadarDbgMode(uval16);
					TCFG_SystemData.RadarDbgMode = TCFG_EEPROM_GetRadarDbgMode();
					Radio_Trf_Printf("RadarDbg : %hu", TCFG_SystemData.RadarDbgMode);
					__NOP();
				}
				/* Magmod */
				else if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "magmod")) {
					sscanf(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "magmod:%hu", &uval16);
					TCFG_EEPROM_SetMagMode(uval16);
					Radio_Trf_Printf("MagMod : %hu", TCFG_EEPROM_GetMagMode());
					talgo_set_magmod(uval16);
					__NOP();
				}
				/* NbHeart */
				else if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "nbheart")) {
					sscanf(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "nbheart:%hu", &uval16);
					TCFG_EEPROM_SetNbiotHeart(uval16);
					TCFG_SystemData.NBIotHeart = TCFG_EEPROM_GetNbiotHeart();
					Radio_Trf_Printf("NbHeart : %hu", TCFG_SystemData.NBIotHeart);
					__NOP();
				}
				/* DisRange */
				else if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "disrange")) {
					sscanf(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "disrange:%hu", &uval16);
					tradar_set_distance_range(uval16 + 4);
					TCFG_EEPROM_SetRadarRange(uval16);
					TCFG_SystemData.RadarRange = TCFG_EEPROM_GetRadarRange();
					Radio_Trf_Printf("RadarRange : %hu", TCFG_SystemData.RadarRange);
					__NOP();
				}
				/* NetInfo */
				else if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "netinfo")) {
				#if NETPROTOCAL == NETCOAP
					NETCoapNeedSendCode.BasicInfo = 1;
					NETCoapNeedSendCode.DynamicInfo = 1;
					Radio_Trf_Printf("Manufacturer:%s", NbiotClientHandler.Parameter.manufacturer);
					Radio_Trf_Printf("ManufacturerMode:%s", NbiotClientHandler.Parameter.manufacturermode);
					Radio_Trf_Printf("IMEI:%s", NbiotClientHandler.Parameter.imei);
					Radio_Trf_Printf("ICCID:%s", NbiotClientHandler.Parameter.iccid);
					Radio_Trf_Printf("IMSI:%s", NbiotClientHandler.Parameter.imsi);
					Radio_Trf_Printf("CGPADDR:%s", NbiotClientHandler.Parameter.cgpaddr);
					Radio_Trf_Printf("CGDCONT:%s", NbiotClientHandler.Parameter.cgdcont);
					Radio_Trf_Printf("RSSI:%d", NbiotClientHandler.Parameter.rssi);
					Radio_Trf_Printf("SNR:%d", NbiotClientHandler.Parameter.statisticsCELL.snr);
					Radio_Trf_Printf("CDPServerHost:%s", NbiotClientHandler.Parameter.cdpserver.CDPServerHost);
					Radio_Trf_Printf("CDPServerPort:%d", NbiotClientHandler.Parameter.cdpserver.CDPServerPort);
				#elif NETPROTOCAL == NETMQTTSN
					NETMqttSNNeedSendCode.InfoBasic = 1;
					NETMqttSNNeedSendCode.InfoDynamic = 1;
					Radio_Trf_Printf("Manufacturer:%s", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.manufacturer);
					Radio_Trf_Printf("ManufacturerMode:%s", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.manufacturermode);
					Radio_Trf_Printf("IMEI:%s", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.imei);
					Radio_Trf_Printf("ICCID:%s", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.iccid);
					Radio_Trf_Printf("IMSI:%s", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.imsi);
					Radio_Trf_Printf("CGPADDR:%s", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.cgpaddr);
					Radio_Trf_Printf("CGDCONT:%s", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.cgdcont);
					Radio_Trf_Printf("RSSI:%d", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.rssi);
					Radio_Trf_Printf("SNR:%d", MqttSNClientHandler.SocketStack->NBIotStack->Parameter.statisticsCELL.snr);
					Radio_Trf_Printf("MqttSN HostName:%s", MQTTSN_SERVER_HOST_NAME);
					Radio_Trf_Printf("MqttSN HostIP:%s:%d", MqttSNClientHandler.SocketStack->ServerHost, MqttSNClientHandler.SocketStack->ServerPort);
				#endif
					__NOP();
				}
				/* ...... */
			}
		}
		else {
			if (pPayload->head.destSN != mac_sn) {
				rc = TRF_NOT_FORME;
				return rc;
			}
			
			if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_GET)
			{
				BEEP_CtrlRepeat_Extend(2, 50, 50);
				if (pPayload->head.type == TRF_MSG_BASICINFO) {
					__NOP();
				}
				else if (pPayload->head.type == TRF_MSG_SENSITIVITY) {
					__NOP();
				}
				else if (pPayload->head.type == TRF_MSG_WORKMODE) {
					__NOP();
				}
				else if (pPayload->head.type == TRF_MSG_INITBACKGROUND) {
					__NOP();
				}
			}
			else if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_ACK)
			{
				__NOP();
			}
			else {
				rc = TRF_BAD_MSGTYPE;
			}
		}
	}
	else {
		rc = TRF_BAD_PROTOCAL;
	}
	
	if (rc == TRF_SUCCESS) {
		Radio_Trf_Default_Resp(100, pPayload->head.type);
	}
	
	return rc;
}

/**********************************************************************************************************
 @Function			void Radio_Trf_App_Task(void)
 @Description			Radio_Trf_App_Task
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Trf_App_Task(void)
{
	static uint32_t hearttime_pre = 0;										//心跳包计时器
	uint8_t len = 0;
	
	if (TRF_OK != Radio_Rf_get_Status()) {
		return;
	}
	
	/* 到达心跳时间 */
	if ((hearttime_pre + TCFG_SystemData.Heartinterval) < Stm32_GetSecondTick()) {
		hearttime_pre = Stm32_GetSecondTick();
		Radio_Rf_Interface_Init();
		Radio_Rf_Interrupt_Init();
		/* 发送心跳包 */
		Radio_Trf_Xmit_Heartbeat();
		/* 发送调试信息 */
		if (DEBUG_WORK == Radio_Trf_Get_Workmode()) {
			
		}
	}
	
	/* 接收无线下行数据 */
	if (TRF_SUCCESS == Radio_Rf_Receive(TRF_RecvBuf, &len)) {
		if (TRF_SUCCESS == Radio_Rf_Operate_Recvmsg(TRF_RecvBuf, len)) {
			
		}
	}
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Cfg_Buildframe(uint8_t *inmsg, uint8_t pkttype, uint8_t pktnum, uint32_t sn, uint8_t *outsend, uint8_t len)
 @Description			Radio_Trf_Cfg_Buildframe
 @Input				...
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Cfg_Buildframe(uint8_t *inmsg, uint8_t pkttype, uint8_t pktnum, uint32_t sn, uint8_t *outsend, uint8_t len)
{
	uint8_t i;
	uint16_t check_sum = 0;
	
	CFG_PUT_INTO_FRAME(CFG_P_FRAME_LEN(outsend), CFG_LEN_OS, CFG_FRAME_PAYLOAD_OFS + CFG_FRAME_CRC_SIZE + len - CFG_FRAME_LEN_SIZE);
	CFG_PUT_INTO_FRAME(CFG_P_FRAME_PRE(outsend), CFG_PRE_TYPE_OS, XMESHCFG_PRT);
	CFG_PUT_INTO_FRAME(CFG_P_FRAME_PRE(outsend), CFG_PRE_VS_OS, CONFIG_VERSION);
	CFG_PUT_INTO_FRAME(CFG_P_FRAME_HEAD(outsend), CFG_HEAD_TYPE_OS, pkttype);
	
	if (pktnum) {
		CFG_PUT_INTO_FRAME(CFG_P_FRAME_HEAD(outsend), CFG_PKTNUM_OS, pktnum);
	}
	
	if (len) {
		memcpy(CFG_P_FRAME_PAYLOAD(outsend), inmsg, len);
	}
	
	memcpy(CFG_P_FRAME_SN(outsend), &sn, 4);
	
	for (i = 1; i < CFG_FRAME_PAYLOAD_OFS + len; i++) {
		check_sum += outsend[i];
	}
	
	outsend[CFG_FRAME_PAYLOAD_OFS + len] = check_sum / 0x100;
	outsend[CFG_FRAME_PAYLOAD_OFS + len + 1] = check_sum % 0x100;
	
	return;
}

/**********************************************************************************************************
 @Function			uint8_t Radio_Trf_Xmit_Get_Pktnum(void)
 @Description			Radio_Trf_Xmit_Get_Pktnum
 @Input				void
 @Return				pktnum
**********************************************************************************************************/
uint8_t Radio_Trf_Xmit_Get_Pktnum(void)
{
	static uint8_t pktnum = 0;
	return pktnum++;
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Default_Resp(uint8_t ret, uint8_t type)
 @Description			Radio_Trf_Default_Resp
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Default_Resp(uint8_t ret, uint8_t type)
{
	trf_defaultrsp_s *pDefaultRsp = (trf_defaultrsp_s*)(TRF_SendBuf + 32);
	pDefaultRsp->head.destSN		= 0xFFFFFFFF;
	pDefaultRsp->head.version	= TRF_MSG_VERSION;
	pDefaultRsp->head.type		= type;
	pDefaultRsp->ret			= ret;
	
	Radio_Trf_Cfg_Buildframe((uint8_t *)pDefaultRsp, TMOTE_PLAIN_RSP, Radio_Trf_Xmit_Get_Pktnum(), TCFG_EEPROM_Get_MAC_SN(), TRF_SendBuf, sizeof(trf_defaultrsp_s));
	Radio_Rf_Send(TRF_SendBuf, TRF_SendBuf[0]);
}

/**********************************************************************************************************
 @Function			uint8_t Radio_Trf_Get_Workmode(void)
 @Description			Radio_Trf_Get_Workmode
 @Input				void
 @Return				workmode
**********************************************************************************************************/
uint8_t Radio_Trf_Get_Workmode(void)
{
	if (DeviceIdleMode == true) {
		return IDLE_WORK;
	}
	else if (DeviceActivedMode != true) {
		return NOTACTIVE_WORK;
	}
	else {
		return TCFG_SystemData.WorkMode;
	}
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Set_Workmode(uint8_t val)
 @Description			Radio_Trf_Set_Workmode
 @Input				workmode
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Set_Workmode(uint8_t val)
{
	TCFG_SystemData.WorkMode = val;
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Xmit_Heartbeat(void)
 @Description			Radio_Trf_Xmit_Heartbeat
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Xmit_Heartbeat(void)
{
	trf_heartbeat_s *pHeartBeat = (trf_heartbeat_s*)(TRF_SendBuf + 32);
	
	if (TRF_OK != Radio_Rf_get_Status()) {
		return;
	}
	
	memset((void*)pHeartBeat, 0, sizeof(trf_heartbeat_s));
	pHeartBeat->head.destSN		= 0xFFFFFFFF;
	pHeartBeat->head.version		= TRF_MSG_VERSION;
	pHeartBeat->head.type		= TRF_MSG_HEART;
	pHeartBeat->major_softver	= TCFG_Utility_Get_Major_Softnumber();
	pHeartBeat->sub_softver		= TCFG_Utility_Get_Sub_Softnumber();
	pHeartBeat->DevType			= MVB_MODEL_TYPE;
	pHeartBeat->sub_hardver		= TCFG_Utility_Get_Major_Hardnumber();
	if (DeviceActivedMode == true) {
		pHeartBeat->workmode	= TCFG_SystemData.WorkMode;
	}
	else {
		pHeartBeat->workmode	= NOTACTIVE_WORK;
	}
	/* 0=free, 1=occupy */
	pHeartBeat->status			= talgo_get_spotstatus();
	
	Radio_Trf_Cfg_Buildframe((uint8_t *)pHeartBeat, TMOTE_PLAIN_PUB, Radio_Trf_Xmit_Get_Pktnum(), TCFG_EEPROM_Get_MAC_SN(), TRF_SendBuf, sizeof(trf_heartbeat_s));
	Radio_Rf_Send(TRF_SendBuf, TRF_SendBuf[0]);
	Delay_MS(6);
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Do_Heartbeat(void)
 @Description			Radio_Trf_Do_Heartbeat
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Do_Heartbeat(void)
{
#ifdef	RADIO_SI4438
	Radio_Rf_Interface_Init();
	Radio_Rf_Interrupt_Init();
	
	Radio_Trf_Xmit_Heartbeat();
#endif
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Do_Rf_Pintf(char* info)
 @Description			Radio_Trf_Do_Rf_Pintf
 @Input				info
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Do_Rf_Pintf(char* info)
{
	uint8_t infolen;
	trf_msg_s *pMsg = (trf_msg_s*)(TRF_SendBuf + 32);
	
	infolen = strlen(info);
	if (infolen > 46) {
		infolen = 46;
	}
	
	Radio_Rf_Interface_Init();
	Radio_Rf_Interrupt_Init();
	
	if (TRF_OK != Radio_Rf_get_Status()) {
		return;
	}
	
	pMsg->head.destSN			= 0xFFFFFFFF;
	pMsg->head.version			= TRF_MSG_VERSION;
	pMsg->head.type			= TRF_MSG_DEBUG_INFO;
	strncpy(pMsg->pData, info, infolen);
	
	Radio_Trf_Cfg_Buildframe((uint8_t *)pMsg, TMOTE_PLAIN_PUB, Radio_Trf_Xmit_Get_Pktnum(), TCFG_EEPROM_Get_MAC_SN(), TRF_SendBuf, sizeof(trf_msghead_s) + infolen);
	Radio_Rf_Send(TRF_SendBuf, TRF_SendBuf[0]);
	if (TRF_SendBuf[0] >=  RADIO_TX_ALMOST_EMPTY_THRESHOLD) {
		Radio_StartTx_dummy(RF_CHANNEL1);
	}
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Debug_Printf(const char *fmt, ...)
 @Description			Radio_Trf_Debug_Printf		: Radio Debug Printf
 @Input				...
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Debug_Printf(const char *fmt, ...)
{
	if (DEBUG_WORK == Radio_Trf_Get_Workmode()) {
#ifdef	RADIO_SI4438
		__va_list args;
		va_start (args, fmt);
		
		memset(TRF_PrintfBuf, 0, sizeof(TRF_PrintfBuf));
		vsprintf((char*)TRF_PrintfBuf, fmt, args);
		va_end (args);
		Radio_Trf_Do_Rf_Pintf((char*)TRF_PrintfBuf);
#endif
	}
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Printf(const char *fmt, ...)
 @Description			Radio_Trf_Printf			: Radio Printf
 @Input				...
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Printf(const char *fmt, ...)
{
#ifdef	RADIO_SI4438
	__va_list args;
	va_start (args, fmt);
	
	memset(TRF_PrintfBuf, 0, sizeof(TRF_PrintfBuf));
	vsprintf ((char*)TRF_PrintfBuf, fmt, args);
	va_end (args);
	Radio_Trf_Do_Rf_Pintf((char*)TRF_PrintfBuf);
#endif
}

/********************************************** END OF FLEE **********************************************/
