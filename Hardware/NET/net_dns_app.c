/**
  *********************************************************************************************************
  * @file    net_dns_app.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-05-16
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "net_dns_app.h"
#include "hal_rtc.h"
#include "string.h"

#ifdef DNS_DEBUG_LOG_RF_PRINT
#include "radio_rf_app.h"
#endif

/**********************************************************************************************************
 @Function			void NET_DNS_APP_PollExecution(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_APP_PollExecution			: DNS逻辑处理
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_APP_PollExecution(DNS_ClientsTypeDef* pClient)
{
	switch (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent)
	{
	case STOP_MODE:
		NET_DNS_NBIOT_Event_StopMode(pClient);
		break;
	
	case HARDWARE_REBOOT:
		NET_DNS_NBIOT_Event_HardwareReboot(pClient);
		break;
	
	case MODULE_CHECK:
		NET_DNS_NBIOT_Event_ModuleCheck(pClient);
		break;
	
	case PARAMETER_CONFIG:
		NET_DNS_NBIOT_Event_ParameterConfig(pClient);
		break;
	
	case ICCID_CHECK:
		NET_DNS_NBIOT_Event_SimICCIDCheck(pClient);
		break;
	
	case MISC_EQUIP_CONFIG:
		NET_DNS_NBIOT_Event_MiscEquipConfig(pClient);
		break;
	
	case ATTACH_CHECK:
		NET_DNS_NBIOT_Event_AttachCheck(pClient);
		break;
	
	case ATTACH_EXECUTE:
		NET_DNS_NBIOT_Event_AttachExecute(pClient);
		break;
	
	case ATTACH_INQUIRE:
		NET_DNS_NBIOT_Event_AttachInquire(pClient);
		break;
	
	case PARAMETER_CHECKOUT:
		NET_DNS_NBIOT_Event_PatameterCheckOut(pClient);
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
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case DNS_PROCESS_STACK:
		NET_DNS_APP_ProcessExecution(pClient);
		break;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_APP_ProcessExecution(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_APP_ProcessExecution			: DNS协议逻辑处理
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_APP_ProcessExecution(DNS_ClientsTypeDef* pClient)
{
	switch (pClient->ProcessState)
	{
	case DNS_PROCESS_CREAT_UDP_SOCKET:
		
		break;
	
	case DNS_PROCESS_SEND_DNS_STRUCT_DATA:
		
		break;
	
	case DNS_PROCESS_RECV_DNS_STRUCT_DATA:
		
		break;
	
	case DNS_PROCESS_CLOSE_UDP_SOCKET:
		
		break;
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_StopMode(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_StopMode			: 停止模式
 @Input				pClient							: DNS客户端实例
 @Return				void
 @attention			当30分钟或有数据需要发送时退出停止模式尝试重启NB,开启NB运行
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_StopMode(DNS_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	static unsigned char DNSMqttSNStatusBasicIndex;
	static unsigned char DNSMqttSNStatusExtendIndex;
	static unsigned char DNSMqttSNInfoWorkIndex;
	static unsigned char DNSMqttSNInfoBasicIndex;
	static unsigned char DNSMqttSNInfoDynamicIndex;
	static unsigned char DNSMqttSNInfoRadarIndex;
	
	/* It is the first time to execute */
	if (pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable != true) {
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = true;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec = 1800;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->SocketStack->NBIotStack->DictateRunCtl.dictateTimeoutSec);
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime = dictateRunTime;
		/* NBIOT Module Poweroff */
		NBIOT_Neul_NBxx_HardwarePoweroff(pClient->SocketStack->NBIotStack);
		/* Init Message Index */
		DNSMqttSNStatusBasicIndex = NET_MqttSN_Message_StatusBasicRear();
		DNSMqttSNStatusExtendIndex = NET_MqttSN_Message_StatusExtendRear();
		DNSMqttSNInfoWorkIndex = NET_MqttSN_Message_InfoWorkRear();
		DNSMqttSNInfoBasicIndex = NET_MqttSN_Message_InfoBasicRear();
		DNSMqttSNInfoDynamicIndex = NET_MqttSN_Message_InfoDynamicRear();
		DNSMqttSNInfoRadarIndex = NET_MqttSN_Message_InfoRadarRear();
	}
	
	if (Stm32_Calculagraph_IsExpiredSec(&pClient->SocketStack->NBIotStack->DictateRunCtl.dictateRunTime) == true) {
		/* Dictate TimeOut */
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
	}
	else {
		/* Dictate isn't TimeOut */
		if ( (NET_MqttSN_Message_StatusBasicRear() != DNSMqttSNStatusBasicIndex) || 
			(NET_MqttSN_Message_StatusExtendRear() != DNSMqttSNStatusExtendIndex) ||
			(NET_MqttSN_Message_InfoWorkRear() != DNSMqttSNInfoWorkIndex) ||
			(NET_MqttSN_Message_InfoBasicRear() != DNSMqttSNInfoBasicIndex) ||
			(NET_MqttSN_Message_InfoDynamicRear() != DNSMqttSNInfoDynamicIndex) ||
			(NET_MqttSN_Message_InfoRadarRear() != DNSMqttSNInfoRadarIndex) ) {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->ProcessState = DNS_PROCESS_CREAT_UDP_SOCKET;
		}
		else {
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = STOP_MODE;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_HardwareReboot(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_HardwareReboot		: 硬件重启
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_HardwareReboot(DNS_ClientsTypeDef* pClient)
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT HDReboot Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_ModuleCheck(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_ModuleCheck		: 模块检测
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_ModuleCheck(DNS_ClientsTypeDef* pClient)
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ModuleCheck Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_ParameterConfig(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_ParameterConfig	: 参数配置
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_ParameterConfig(DNS_ClientsTypeDef* pClient)
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT EnableBipSet %d Fail", NConfigFalse);
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_NET_NBIOT_Event_SimICCIDCheck(DNS_ClientsTypeDef* pClient)
 @Description			NET_NET_NBIOT_Event_SimICCIDCheck		: Sim卡检测
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_SimICCIDCheck(DNS_ClientsTypeDef* pClient)
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ICCIDCheck Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_MiscEquipConfig(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_MiscEquipConfig	: 其他配置
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_MiscEquipConfig(DNS_ClientsTypeDef* pClient)
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT MiscRead Fail");
#endif
		return;
	}
	
	if (pClient->SocketStack->NBIotStack->Parameter.band != DNS_NBIOT_BAND) {
		if (NBIOT_Neul_NBxx_SetSupportedBands(pClient->SocketStack->NBIotStack, DNS_NBIOT_BAND) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEnable = false;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = ATTACH_CHECK;
			pClient->SocketStack->NBIotStack->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT BandSet %d Ok", DNS_NBIOT_BAND);
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT BandSet %d Fail", DNS_NBIOT_BAND);
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_AttachCheck(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_AttachCheck		: 注网检查
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_AttachCheck(DNS_ClientsTypeDef* pClient)
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
 @Function			void NET_DNS_NBIOT_Event_AttachExecute(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_AttachExecute		: 注网进行
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_AttachExecute(DNS_ClientsTypeDef* pClient)
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT Set CGATT %d Fail", Attach);
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_DNS_NBIOT_Event_AttachInquire(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_AttachInquire		: 注网查询
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_AttachInquire(DNS_ClientsTypeDef* pClient)
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
#ifdef DNS_DEBUG_LOG_RF_PRINT
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
 @Function			void NET_DNS_NBIOT_Event_PatameterCheckOut(DNS_ClientsTypeDef* pClient)
 @Description			NET_DNS_NBIOT_Event_PatameterCheckOut	: 参数检出
 @Input				pClient							: DNS客户端实例
 @Return				void
**********************************************************************************************************/
void NET_DNS_NBIOT_Event_PatameterCheckOut(DNS_ClientsTypeDef* pClient)
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
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictateEvent = DNS_PROCESS_STACK;
		pClient->SocketStack->NBIotStack->DictateRunCtl.dictatePatameterCheckOutFailureCnt = 0;
#ifdef DNS_DEBUG_LOG_RF_PRINT
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






















































/********************************************** END OF FLEE **********************************************/
