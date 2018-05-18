/**
  *********************************************************************************************************
  * @file    net_mqttsn_app.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-12
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "net_mqttsn_app.h"
#include "hal_rtc.h"
#include "hal_beep.h"
#include "string.h"

#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
#include "radio_rf_app.h"
#endif

/**********************************************************************************************************
 @Function			void NET_MQTTSN_APP_PollExecution(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_APP_PollExecution				: MqttSN逻辑处理
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_APP_PollExecution(MQTTSN_ClientsTypeDef* pClient)
{
	switch (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent)
	{
	case STOP_MODE:
		NET_MQTTSN_NBIOT_Event_StopMode(pClient);
		break;
		
	case HARDWARE_REBOOT:
		NET_MQTTSN_NBIOT_Event_HardwareReboot(pClient);
		break;
	
	case MODULE_CHECK:
		NET_MQTTSN_NBIOT_Event_ModuleCheck(pClient);
		break;
	
	case PARAMETER_CONFIG:
		NET_MQTTSN_NBIOT_Event_ParameterConfig(pClient);
		break;
	
	case ICCID_CHECK:
		NET_MQTTSN_NBIOT_Event_SimICCIDCheck(pClient);
		break;
	
	case MISC_EQUIP_CONFIG:
		NET_MQTTSN_NBIOT_Event_MiscEquipConfig(pClient);
		break;
	
	case ATTACH_CHECK:
		NET_MQTTSN_NBIOT_Event_AttachCheck(pClient);
		break;
	
	case ATTACH_EXECUTE:
		NET_MQTTSN_NBIOT_Event_AttachExecute(pClient);
		break;
	
	case ATTACH_INQUIRE:
		NET_MQTTSN_NBIOT_Event_AttachInquire(pClient);
		break;
	
	case PARAMETER_CHECKOUT:
		NET_MQTTSN_NBIOT_Event_PatameterCheckOut(pClient);
		break;
	
	case MINIMUM_FUNCTIONALITY:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case FULL_FUNCTIONALITY:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case CDP_SERVER_CHECK:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case CDP_SERVER_CONFIG:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case SEND_DATA:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case RECV_DATA:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case EXECUT_DOWNLINK_DATA:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case MQTTSN_PROCESS_STACK:
		NET_MQTTSN_APP_ProcessExecution(pClient);
		break;
	
	case DNS_PROCESS_STACK:
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_APP_ProcessExecution(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_APP_ProcessExecution		: MqttSN协议逻辑处理
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_APP_ProcessExecution(MQTTSN_ClientsTypeDef* pClient)
{
	switch (pClient->SubState)
	{
	case MQTTSN_SUBSTATE_INIT:
		NET_MQTTSN_Event_Init(pClient);
		break;
		
	case MQTTSN_SUBSTATE_DISCONNECT:
		NET_MQTTSN_Event_Disconnect(pClient);
		break;
	
	case MQTTSN_SUBSTATE_ACTIVE:
		NET_MQTTSN_Event_Active(pClient);
		break;
	
	case MQTTSN_SUBSTATE_SLEEP:
		NET_MQTTSN_Event_Sleep(pClient);
		break;
	
	case MQTTSN_SUBSTATE_AWAKE:
		NET_MQTTSN_Event_Aweak(pClient);
		break;
	
	case MQTTSN_SUBSTATE_LOST:
		NET_MQTTSN_Event_Lost(pClient);
		break;
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_StopMode(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_StopMode		: 停止模式
 @Input				pClient							: MqttSN客户端实例
 @Return				void
 @attention			当30分钟或有数据需要发送时退出停止模式尝试重启NB,开启NB运行
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_StopMode(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	static unsigned char MqttSNStatusBasicIndex;
	static unsigned char MqttSNStatusExtendIndex;
	static unsigned char MqttSNInfoWorkIndex;
	static unsigned char MqttSNInfoBasicIndex;
	static unsigned char MqttSNInfoDynamicIndex;
	static unsigned char MqttSNInfoRadarIndex;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 1800;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
		/* NBIOT Module Poweroff */
		NBIOT_Neul_NBxx_HardwarePoweroff(pClient->SocketStack->NBIotStack);
		/* Init Message Index */
		MqttSNStatusBasicIndex = NET_MqttSN_Message_StatusBasicRear();
		MqttSNStatusExtendIndex = NET_MqttSN_Message_StatusExtendRear();
		MqttSNInfoWorkIndex = NET_MqttSN_Message_InfoWorkRear();
		MqttSNInfoBasicIndex = NET_MqttSN_Message_InfoBasicRear();
		MqttSNInfoDynamicIndex = NET_MqttSN_Message_InfoDynamicRear();
		MqttSNInfoRadarIndex = NET_MqttSN_Message_InfoRadarRear();
	}
	
	if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
		/* Dictate TimeOut */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		pClient->SubState = MQTTSN_SUBSTATE_INIT;
		pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
	}
	else {
		/* Dictate isn't TimeOut */
		if ( (NET_MqttSN_Message_StatusBasicRear() != MqttSNStatusBasicIndex) || 
			(NET_MqttSN_Message_StatusExtendRear() != MqttSNStatusExtendIndex) ||
			(NET_MqttSN_Message_InfoWorkRear() != MqttSNInfoWorkIndex) ||
			(NET_MqttSN_Message_InfoBasicRear() != MqttSNInfoBasicIndex) ||
			(NET_MqttSN_Message_InfoDynamicRear() != MqttSNInfoDynamicIndex) ||
			(NET_MqttSN_Message_InfoRadarRear() != MqttSNInfoRadarIndex) ) {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SubState = MQTTSN_SUBSTATE_INIT;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
		}
		else {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			pClient->SubState = MQTTSN_SUBSTATE_INIT;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_HardwareReboot(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_HardwareReboot	: 硬件重启
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_HardwareReboot(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_HardwareReboot(pClient->SocketStack->NBIotStack, 8000) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MODULE_CHECK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRebootFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT HDReboot Ok, Baud : %d", NBIOTBaudRate.Baud);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRebootFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRebootFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRebootFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT HDReboot Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_ModuleCheck(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_ModuleCheck		: 模块检测
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_ModuleCheck(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if ((NBIOT_Neul_NBxx_CheckReadManufacturer(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadManufacturerModel(pClient->SocketStack->NBIotStack) == NBIOT_OK)) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateModuleCheckFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ModuleCheck Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateModuleCheckFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateModuleCheckFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateModuleCheckFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MODULE_CHECK;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ModuleCheck Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_ParameterConfig(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_ParameterConfig	: 参数配置
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_ParameterConfig(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadConfigUE(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ConfigRead Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ConfigRead Fail");
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.autoConnect != NConfigTrue) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, AutoConnect, NConfigTrue) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT AutoConnectSet %d Ok", NConfigTrue);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT AutoConnectSet %d Fail", NConfigTrue);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.crScrambling != NConfigTrue) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, CrScrambling, NConfigTrue) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CrScramblingSet %d Ok", NConfigTrue);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CrScramblingSet %d Fail", NConfigTrue);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.crSiAvoid != NConfigTrue) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, CrSiAvoid, NConfigTrue) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CrSiAvoidSet %d Ok", NConfigTrue);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CrSiAvoidSet %d Fail", NConfigTrue);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.combineAttach != NConfigFalse) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, CombineAttach, NConfigFalse) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CombineAttachSet %d Ok", NConfigFalse);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CombineAttachSet %d Fail", NConfigFalse);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.cellReselection != NConfigFalse) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, CellReselection, NConfigFalse) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CellReselectionSet %d Ok", NConfigFalse);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CellReselectionSet %d Fail", NConfigFalse);
#endif
			return;
		}
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.nconfig.enableBip != NConfigFalse) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient->SocketStack->NBIotStack, EnableBip, NConfigFalse) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT EnableBipSet %d Ok", NConfigFalse);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT EnableBipSet %d Fail", NConfigFalse);
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_SimICCIDCheck(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_SimICCIDCheck	: Sim卡检测
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_SimICCIDCheck(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadICCID(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateSimICCIDCheckFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ICCIDCheck Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateSimICCIDCheckFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateSimICCIDCheckFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateSimICCIDCheckFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ICCID_CHECK;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ICCIDCheck Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_MiscEquipConfig(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_MiscEquipConfig	: 其他配置
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_MiscEquipConfig(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadSupportedBands(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT MiscRead Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT MiscRead Fail");
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.band != MQTTSN_NBIOT_BAND) {
		if (NBIOT_Neul_NBxx_SetSupportedBands(pClient->SocketStack->NBIotStack, MQTTSN_NBIOT_BAND) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT BandSet %d Ok", MQTTSN_NBIOT_BAND);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt++;
				if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt > 3) {
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT BandSet %d Fail", MQTTSN_NBIOT_BAND);
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_AttachCheck(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_AttachCheck		: 注网检查
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_AttachCheck(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadAttachOrDetach(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachCheckFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT CGATT %d Ok", pClient->SocketStack->NBIotStack->Parameter.netstate);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachCheckFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachCheckFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachCheckFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT CGATT %d Fail", pClient->SocketStack->NBIotStack->Parameter.netstate);
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.netstate != Attach) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_EXECUTE;
	}
	else {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_AttachExecute(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_AttachExecute	: 注网进行
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_AttachExecute(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_SetAttachOrDetach(pClient->SocketStack->NBIotStack, Attach) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachExecuteFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT Set CGATT %d Ok", Attach);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachExecuteFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachExecuteFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachExecuteFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_EXECUTE;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT Set CGATT %d Fail", Attach);
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_AttachInquire(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_AttachInquire	: 注网查询
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_AttachInquire(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 60;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadAttachOrDetach(pClient->SocketStack->NBIotStack) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT CGATT %d Ok", pClient->SocketStack->NBIotStack->Parameter.netstate);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT CGATT %d Fail", pClient->SocketStack->NBIotStack->Parameter.netstate);
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.netstate != Attach) {
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		}
	}
	else {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CHECKOUT;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_NBIOT_Event_PatameterCheckOut(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_NBIOT_Event_PatameterCheckOut: 参数检出
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_NBIOT_Event_PatameterCheckOut(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if ((NBIOT_Neul_NBxx_CheckReadIMEI(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadIMEISV(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadIMSI(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadCGPADDR(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadCGDCONT(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadRSSI(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadStatisticsCELL(pClient->SocketStack->NBIotStack) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadDateTime(pClient->SocketStack->NBIotStack) == NBIOT_OK)) {
		/* Dictate execute is Success */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT Patameter Check Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt++;
			if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt > 3) {
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = PARAMETER_CHECKOUT;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT Patameter Check Fail");
#endif
		return;
	}
	
	/* Set System Time */
	RTC_Set_Date(pClient->SocketStack->NBIotStack->Parameter.dataTime.year, pClient->SocketStack->NBIotStack->Parameter.dataTime.month, pClient->SocketStack->NBIotStack->Parameter.dataTime.day);
	RTC_Set_Time(pClient->SocketStack->NBIotStack->Parameter.dataTime.hour, pClient->SocketStack->NBIotStack->Parameter.dataTime.min,   pClient->SocketStack->NBIotStack->Parameter.dataTime.sec);
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_Event_Init(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_Event_Init				: INIT
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_Event_Init(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	/* Creat UDP Socket */
	if (pClient->SocketStack->Open(pClient->SocketStack) == MQTTSN_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
		pClient->SubState = MQTTSN_SUBSTATE_DISCONNECT;
		pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
		pClient->DictateRunCtl.dictateInitFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("MqttSN Creat UDP Socket Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SubState = MQTTSN_SUBSTATE_INIT;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
			pClient->DictateRunCtl.dictateInitFailureCnt++;
			if (pClient->DictateRunCtl.dictateInitFailureCnt > 3) {
				pClient->DictateRunCtl.dictateInitFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_INIT;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("MqttSN Creat UDP Socket Fail");
#endif
		return;
	}
	
	/* Initiation MQTTSN Status */
	pClient->DictateRunCtl.dictateSubscribeStatus = false;
	/* Set Pingreg Duration */
	pClient->DictateRunCtl.dictateTimeoutSec = TNET_MQTTSN_PINGREG_DURATION;
	Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
	pClient->PingTimer = dictateRunTime;
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_Event_Disconnect(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_Event_Disconnect			: DISCONNECT
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_Event_Disconnect(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 60;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	/* Connecting MQTTSN Server */
	options.clientID.cstring = MQTTSN_CLIENT_ID;
	options.duration = TNET_MQTTSN_ACTIVE_DURATION;
	options.cleansession = false;
	if (MQTTSN_Connect(pClient, &options) != MQTTSN_OK) {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SubState = MQTTSN_SUBSTATE_INIT;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
			pClient->DictateRunCtl.dictateDisconnectFailureCnt++;
			if (pClient->DictateRunCtl.dictateDisconnectFailureCnt > 3) {
				pClient->DictateRunCtl.dictateDisconnectFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
				pClient->SubState = MQTTSN_SUBSTATE_LOST;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_DISCONNECT;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
		}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("MqttSN Connect Server Fail");
#endif
	}
	else {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
		pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
		pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
		pClient->DictateRunCtl.dictateDisconnectFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("MqttSN Connect Server Ok");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_Event_Active(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_Event_Active				: ACTIVE
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_Event_Active(MQTTSN_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 60;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	/* Subscribing Topic */
	if (pClient->DictateRunCtl.dictateSubscribeStatus != true) {
		if (MQTTSN_Subscribe(pClient, MQTTSN_SUBSCRIBE_ID, QOS2, messageHandlerFunction) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
				pClient->DictateRunCtl.dictateActiveFailureCnt++;
				if (pClient->DictateRunCtl.dictateActiveFailureCnt > 3) {
					pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
					pClient->SubState = MQTTSN_SUBSTATE_LOST;
					pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
				pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Subscrib %s Topic Fail", MQTTSN_SUBSCRIBE_ID);
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
			pClient->DictateRunCtl.dictateSubscribeStatus = true;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Subscrib %s Topic Ok", MQTTSN_SUBSCRIBE_ID);
#endif
		}
	}
	
	/* Whether the query has data needs to be sent */
	/* OBJECT_TYPE_TMOTES_STATUS_BASIC_PUT */
	if (NET_MqttSN_Message_StatusBasicisEmpty() != true) {
		pClient->MessageSendCtl.messageStatusBasic = true;
	}
	if (pClient->MessageSendCtl.messageStatusBasic != false) {
		if (NET_MQTTSN_SendPayloadPacket(pClient, OBJECT_TYPE_TMOTES_STATUS_BASIC_PUT) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
				pClient->DictateRunCtl.dictateActiveFailureCnt++;
				if (pClient->DictateRunCtl.dictateActiveFailureCnt > 3) {
					pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
					pClient->SubState = MQTTSN_SUBSTATE_LOST;
					pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send StatusBasic Fail");
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			pClient->MessageSendCtl.messageStatusBasic = false;
			NET_MqttSN_Message_StatusBasicOffSet();
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send StatusBasic Ok");
#endif
		}
	}
	
	/* OBJECT_TYPE_TMOTES_STATUS_EXTEND_PUT */
	if (NET_MqttSN_Message_StatusExtendisEmpty() != true) {
		pClient->MessageSendCtl.messageStatusExtend = true;
	}
	if (pClient->MessageSendCtl.messageStatusExtend != false) {
		if (NET_MQTTSN_SendPayloadPacket(pClient, OBJECT_TYPE_TMOTES_STATUS_EXTEND_PUT) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
				pClient->DictateRunCtl.dictateActiveFailureCnt++;
				if (pClient->DictateRunCtl.dictateActiveFailureCnt > 3) {
					pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
					pClient->SubState = MQTTSN_SUBSTATE_LOST;
					pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send StatusExtend Fail");
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			pClient->MessageSendCtl.messageStatusExtend = false;
			NET_MqttSN_Message_StatusExtendOffSet();
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send StatusExtend Ok");
#endif
		}
	}
	
	/* OBJECT_TYPE_TMOTES_INFO_WORK_PUT */
	if (NET_MqttSN_Message_InfoWorkisEmpty() != true) {
		pClient->MessageSendCtl.messageInfoWork = true;
	}
	if (pClient->MessageSendCtl.messageInfoWork != false) {
		if (NET_MQTTSN_SendPayloadPacket(pClient, OBJECT_TYPE_TMOTES_INFO_WORK_PUT) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
				pClient->DictateRunCtl.dictateActiveFailureCnt++;
				if (pClient->DictateRunCtl.dictateActiveFailureCnt > 3) {
					pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
					pClient->SubState = MQTTSN_SUBSTATE_LOST;
					pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send InfoWork Fail");
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			pClient->MessageSendCtl.messageInfoWork = false;
			NET_MqttSN_Message_InfoWorkOffSet();
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send InfoWork Ok");
#endif
		}
	}
	
	/* OBJECT_TYPE_TMOTES_INFO_BASIC_PUT */
	if (NET_MqttSN_Message_InfoBasicisEmpty() != true) {
		pClient->MessageSendCtl.messageInfoBasic = true;
	}
	if (pClient->MessageSendCtl.messageInfoBasic != false) {
		if (NET_MQTTSN_SendPayloadPacket(pClient, OBJECT_TYPE_TMOTES_INFO_BASIC_PUT) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
				pClient->DictateRunCtl.dictateActiveFailureCnt++;
				if (pClient->DictateRunCtl.dictateActiveFailureCnt > 3) {
					pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
					pClient->SubState = MQTTSN_SUBSTATE_LOST;
					pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send InfoBasic Fail");
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			pClient->MessageSendCtl.messageInfoBasic = false;
			NET_MqttSN_Message_InfoBasicOffSet();
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send InfoBasic Ok");
#endif
		}
	}
	
	/* OBJECT_TYPE_TMOTES_INFO_DYNAMIC_PUT */
	if (NET_MqttSN_Message_InfoDynamicisEmpty() != true) {
		pClient->MessageSendCtl.messageInfoDynamic = true;
	}
	if (pClient->MessageSendCtl.messageInfoDynamic != false) {
		if (NET_MQTTSN_SendPayloadPacket(pClient, OBJECT_TYPE_TMOTES_INFO_DYNAMIC_PUT) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
				pClient->DictateRunCtl.dictateActiveFailureCnt++;
				if (pClient->DictateRunCtl.dictateActiveFailureCnt > 3) {
					pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
					pClient->SubState = MQTTSN_SUBSTATE_LOST;
					pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send InfoDynamic Fail");
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			pClient->MessageSendCtl.messageInfoDynamic = false;
			NET_MqttSN_Message_InfoDynamicOffSet();
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send InfoDynamic Ok");
#endif
		}
	}
	
	/* OBJECT_TYPE_TMOTES_INFO_RADAR_PUT */
	if (NET_MqttSN_Message_InfoRadarisEmpty() != true) {
		pClient->MessageSendCtl.messageInfoRadar = true;
	}
	if (pClient->MessageSendCtl.messageInfoRadar != false) {
		if (NET_MQTTSN_SendPayloadPacket(pClient, OBJECT_TYPE_TMOTES_INFO_RADAR_PUT) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
				pClient->DictateRunCtl.dictateActiveFailureCnt++;
				if (pClient->DictateRunCtl.dictateActiveFailureCnt > 3) {
					pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
					pClient->SubState = MQTTSN_SUBSTATE_LOST;
					pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send InfoRadar Fail");
#endif
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			pClient->MessageSendCtl.messageInfoRadar = false;
			NET_MqttSN_Message_InfoRadarOffSet();
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN Send InfoRadar Ok");
#endif
		}
	}
	
	/* Keep active for Active seconds before to Sleep, so we can send messsage contiguously */
	if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
		/* Arrival time for Sleep */
		if (MQTTSN_DisConnect(pClient, TNET_MQTTSN_SLEEP_DURATION) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SubState = MQTTSN_SUBSTATE_INIT;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
			pClient->DictateRunCtl.dictateActiveFailureCnt++;
			if (pClient->DictateRunCtl.dictateActiveFailureCnt > 3) {
				pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
				pClient->SubState = MQTTSN_SUBSTATE_LOST;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
			}
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN DisConnect Fail");
#endif
		}
		else {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_AWAKE;
			pClient->DictateRunCtl.dictateActiveFailureCnt = 0;
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("MqttSN DisConnect Ok");
#endif
		}
	}
	else {
		/* Keep Active */
		MQTTSN_RecvAck(pClient);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
		pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_Event_Sleep(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_Event_Sleep				: SLEEP
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_Event_Sleep(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 60;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	/* Whether the query has data needs to be sent */
	if (NET_MqttSN_Message_StatusBasicisEmpty() != true) {
		pClient->MessageSendCtl.messageStatusBasic = true;
	}
	if (NET_MqttSN_Message_StatusExtendisEmpty() != true) {
		pClient->MessageSendCtl.messageStatusExtend = true;
	}
	if (NET_MqttSN_Message_InfoWorkisEmpty() != true) {
		pClient->MessageSendCtl.messageInfoWork = true;
	}
	if (NET_MqttSN_Message_InfoBasicisEmpty() != true) {
		pClient->MessageSendCtl.messageInfoBasic = true;
	}
	if (NET_MqttSN_Message_InfoDynamicisEmpty() != true) {
		pClient->MessageSendCtl.messageInfoDynamic = true;
	}
	if (NET_MqttSN_Message_InfoRadarisEmpty() != true) {
		pClient->MessageSendCtl.messageInfoRadar = true;
	}
	if ( (pClient->MessageSendCtl.messageStatusBasic != false) || 
		(pClient->MessageSendCtl.messageStatusExtend != false) ||
		(pClient->MessageSendCtl.messageInfoWork != false) ||
		(pClient->MessageSendCtl.messageInfoBasic != false) ||
		(pClient->MessageSendCtl.messageInfoDynamic != false) ||
		(pClient->MessageSendCtl.messageInfoRadar != false) ) {
		options.clientID.cstring = MQTTSN_CLIENT_ID;
		options.duration = TNET_MQTTSN_ACTIVE_DURATION;
		options.cleansession = false;
		if (MQTTSN_Connect(pClient, &options) != MQTTSN_OK) {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->SubState = MQTTSN_SUBSTATE_INIT;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
				pClient->DictateRunCtl.dictateSleepFailureCnt++;
				if (pClient->DictateRunCtl.dictateSleepFailureCnt > 3) {
					pClient->DictateRunCtl.dictateSleepFailureCnt = 0;
					pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
					pClient->SubState = MQTTSN_SUBSTATE_LOST;
					pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->SubState = MQTTSN_SUBSTATE_SLEEP;
			}
			return;
		}
		else {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
			pClient->SubState = MQTTSN_SUBSTATE_ACTIVE;
			pClient->DictateRunCtl.dictateSleepFailureCnt = 0;
			return;
		}
	}
	
	/* If time to Aweak, then send a pingreq */
	if (Stm32_Calculagraph_IsExpiredSec(&pClient->PingTimer) == true) {
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
		pClient->SubState = MQTTSN_SUBSTATE_AWAKE;
		pClient->DictateRunCtl.dictateSleepFailureCnt = 0;
	}
	else {
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
		pClient->SubState = MQTTSN_SUBSTATE_SLEEP;
		pClient->DictateRunCtl.dictateSleepFailureCnt = 0;
	}
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_Event_Aweak(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_Event_Aweak				: AWAKE
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_Event_Aweak(MQTTSN_ClientsTypeDef* pClient)
{
	MQTTSNString clientid = MQTTSNString_initializer;
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 60;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	/* Pingreqing MQTTSN Server */
	clientid.cstring = MQTTSN_CLIENT_ID;
	if (MQTTSN_Pingreq(pClient, &clientid) != MQTTSN_OK) {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->SubState = MQTTSN_SUBSTATE_INIT;
			pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_DNS;
			pClient->DictateRunCtl.dictateAweakFailureCnt++;
			if (pClient->DictateRunCtl.dictateAweakFailureCnt > 3) {
				pClient->DictateRunCtl.dictateAweakFailureCnt = 0;
				pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
				pClient->SubState = MQTTSN_SUBSTATE_LOST;
				pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->SubState = MQTTSN_SUBSTATE_AWAKE;
		}
		return;
	}
	else {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = MQTTSN_PROCESS_STACK;
		pClient->SubState = MQTTSN_SUBSTATE_SLEEP;
		pClient->DictateRunCtl.dictateAweakFailureCnt = 0;
	}
	
	/* Set Pingreg Duration */
	pClient->DictateRunCtl.dictateTimeoutSec = TNET_MQTTSN_PINGREG_DURATION;
	Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
	pClient->PingTimer = dictateRunTime;
}

/**********************************************************************************************************
 @Function			void NET_MQTTSN_Event_Lost(MQTTSN_ClientsTypeDef* pClient)
 @Description			NET_MQTTSN_Event_Lost				: LOST
 @Input				pClient							: MqttSN客户端实例
 @Return				void
**********************************************************************************************************/
void NET_MQTTSN_Event_Lost(MQTTSN_ClientsTypeDef* pClient)
{
	pClient->DictateRunCtl.dictateEnable = false;
	pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
	pClient->SubState = MQTTSN_SUBSTATE_INIT;
	pClient->NetNbiotStack->PollExecution = NET_POLL_EXECUTION_MQTTSN;
}











/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef messageHandlerFunction(MQTTSN_ClientsTypeDef* pClient, MQTTSN_MessageDataTypeDef* messageHandler)
 @Description			messageHandlerFunction		: MQTTSN接收处理
 @Input				pClient					: MqttSN客户端实例
					messageHandler				: 下行消息
 @Return				MQTTSN_StatusTypeDef		: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef messageHandlerFunction(MQTTSN_ClientsTypeDef* pClient, MQTTSN_MessageDataTypeDef* messageHandler)
{
	//Todo
	
	printf("\nRecv MQTTSN Publish topicFilter : %s, topicid : %d\n", pClient->messageHandlers[0].topicFilter, pClient->messageHandlers[0].topicid);
	printf("\nRecv MQTTSN Publish Payload : %.*s\n", messageHandler->message->payloadlen, messageHandler->message->payload);
	
	BEEP_CtrlRepeat_Extend(5, 25, 25);
	
#ifdef MQTTSN_DEBUG_LOG_RF_PRINT
	Radio_Trf_Debug_Printf("MqttSN Recv Filter:%s, topicid:%d", pClient->messageHandlers[0].topicFilter, pClient->messageHandlers[0].topicid);
	Radio_Trf_Debug_Printf("MqttSN Recv Payload:%.*s", messageHandler->message->payloadlen, messageHandler->message->payload);
#endif
	
	
	
	
	
	
	
	
	
	
	
	return MQTTSN_OK;
}














/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef NET_MQTTSN_SendPayloadPacket(MQTTSN_ClientsTypeDef* pClient, NET_MQTTSN_ObjectPacketTypeDef ObjectPacket)
 @Description			NET_MQTTSN_SendPayloadPacket	: MQTTSN发送负载包
 @Input				pClient					: MqttSN客户端实例
					ObjectPacket				: 对象类型
 @Return				MQTTSN_StatusTypeDef		: MQTTSN处理状态
**********************************************************************************************************/
MQTTSN_StatusTypeDef NET_MQTTSN_SendPayloadPacket(MQTTSN_ClientsTypeDef* pClient, NET_MQTTSN_ObjectPacketTypeDef ObjectPacket)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	NET_Message_TcldMsgTypeDef *pMsg = (NET_Message_TcldMsgTypeDef*)pClient->DataProcessStack;
	MQTTSN_MessageTypeDef message;
	MQTTSN_topicid topic;
	
	memset(pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	
	message.qos		= QOS1;
	message.msgid		= 1;
	message.dup		= 0;
	message.retained	= 0;
	message.payload	= pClient->DataProcessStack;
	
	/* seclect different data and send */
	switch (ObjectPacket)
	{
		case OBJECT_TYPE_TMOTES_STATUS_BASIC_PUT:
		{
			NET_MESSAGE_GET_MAGICNUM(pMsg->MsgHead.MagicNum);
			pMsg->MsgHead.MsgType		=	MSG_JSON;
			pMsg->MsgHead.Version		=	MESSAGE_VERSION;
			pMsg->MsgHead.EncryptMode	=	ENCRYPT_NONE;
			pMsg->MsgHead.MsgId			=	MSGID_PUT;
			
			message.payloadlen			=	NET_Message_Operate_Creat_Json_MoteStatus_Basic(pMsg->pData) + sizeof(NET_Message_TcldHeadTypeDef);
			
			topic.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
			topic.data.id = TOPICID_MOTESTATUS;
			if ((MQTTSNStatus = MQTTSN_Publish(pClient, topic, &message)) != MQTTSN_OK) {
				//Todo
			}
			
			break;
		}
		
		case OBJECT_TYPE_TMOTES_STATUS_EXTEND_PUT:
		{
			NET_MESSAGE_GET_MAGICNUM(pMsg->MsgHead.MagicNum);
			pMsg->MsgHead.MsgType		=	MSG_JSON;
			pMsg->MsgHead.Version		=	MESSAGE_VERSION;
			pMsg->MsgHead.EncryptMode	=	ENCRYPT_NONE;
			pMsg->MsgHead.MsgId			=	MSGID_PUT;
			
			message.payloadlen			=	NET_Message_Operate_Creat_Json_MoteStatus_Extend(pMsg->pData) + sizeof(NET_Message_TcldHeadTypeDef);
			
			topic.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
			topic.data.id = TOPICID_MOTESTATUS;
			if ((MQTTSNStatus = MQTTSN_Publish(pClient, topic, &message)) != MQTTSN_OK) {
				//Todo
			}
			
			break;
		}
		
		case OBJECT_TYPE_TMOTES_INFO_WORK_PUT:
		{
			NET_MESSAGE_GET_MAGICNUM(pMsg->MsgHead.MagicNum);
			pMsg->MsgHead.MsgType		=	MSG_JSON;
			pMsg->MsgHead.Version		=	MESSAGE_VERSION;
			pMsg->MsgHead.EncryptMode	=	ENCRYPT_NONE;
			pMsg->MsgHead.MsgId			=	MSGID_PUT;
			
			message.payloadlen			=	NET_Message_Operate_Creat_Json_MoteInfo_Work(pMsg->pData) + sizeof(NET_Message_TcldHeadTypeDef);
			
			topic.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
			topic.data.id = TOPICID_MOTEINFO;
			if ((MQTTSNStatus = MQTTSN_Publish(pClient, topic, &message)) != MQTTSN_OK) {
				//Todo
			}
			
			break;
		}
		
		case OBJECT_TYPE_TMOTES_INFO_BASIC_PUT:
		{
			NET_MESSAGE_GET_MAGICNUM(pMsg->MsgHead.MagicNum);
			pMsg->MsgHead.MsgType		=	MSG_JSON;
			pMsg->MsgHead.Version		=	MESSAGE_VERSION;
			pMsg->MsgHead.EncryptMode	=	ENCRYPT_NONE;
			pMsg->MsgHead.MsgId			=	MSGID_PUT;
			
			message.payloadlen			=	NET_Message_Operate_Creat_Json_MoteInfo_Basic(pMsg->pData) + sizeof(NET_Message_TcldHeadTypeDef);
			
			topic.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
			topic.data.id = TOPICID_MOTEINFO;
			if ((MQTTSNStatus = MQTTSN_Publish(pClient, topic, &message)) != MQTTSN_OK) {
				//Todo
			}
			
			break;
		}
		
		case OBJECT_TYPE_TMOTES_INFO_DYNAMIC_PUT:
		{
			NET_MESSAGE_GET_MAGICNUM(pMsg->MsgHead.MagicNum);
			pMsg->MsgHead.MsgType		=	MSG_JSON;
			pMsg->MsgHead.Version		=	MESSAGE_VERSION;
			pMsg->MsgHead.EncryptMode	=	ENCRYPT_NONE;
			pMsg->MsgHead.MsgId			=	MSGID_PUT;
			
			message.payloadlen			=	NET_Message_Operate_Creat_Json_MoteInfo_Dynamic(pMsg->pData) + sizeof(NET_Message_TcldHeadTypeDef);
			
			topic.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
			topic.data.id = TOPICID_MOTEINFO;
			if ((MQTTSNStatus = MQTTSN_Publish(pClient, topic, &message)) != MQTTSN_OK) {
				//Todo
			}
			
			break;
		}
		
		case OBJECT_TYPE_TMOTES_INFO_RADAR_PUT:
		{
			NET_MESSAGE_GET_MAGICNUM(pMsg->MsgHead.MagicNum);
			pMsg->MsgHead.MsgType		=	MSG_JSON;
			pMsg->MsgHead.Version		=	MESSAGE_VERSION;
			pMsg->MsgHead.EncryptMode	=	ENCRYPT_NONE;
			pMsg->MsgHead.MsgId			=	MSGID_PUT;
			
			message.payloadlen			=	NET_Message_Operate_Creat_Json_MoteInfo_Radar(pMsg->pData) + sizeof(NET_Message_TcldHeadTypeDef);
			
			topic.type = MQTTSN_TOPIC_TYPE_PREDEFINED;
			topic.data.id = TOPICID_MOTEINFO;
			if ((MQTTSNStatus = MQTTSN_Publish(pClient, topic, &message)) != MQTTSN_OK) {
				//Todo
			}
			
			break;
		}
	}
	
	return MQTTSNStatus;
}

/********************************************** END OF FLEE **********************************************/
