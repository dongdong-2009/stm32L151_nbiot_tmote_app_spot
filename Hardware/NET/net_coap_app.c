/**
  *********************************************************************************************************
  * @file    net_coap_app.c
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

#include "net_coap_app.h"
#include "hal_rtc.h"
#include "hal_beep.h"
#include "string.h"

#ifdef COAP_DEBUG_LOG_RF_PRINT
#include "radio_rf_app.h"
#endif

/**********************************************************************************************************
 @Function			void NET_COAP_APP_PollExecution(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_APP_PollExecution			: COAP逻辑处理
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_APP_PollExecution(NBIOT_ClientsTypeDef* pClient)
{
	switch (pClient->DictateRunCtl.dictateEvent)
	{
	case STOP_MODE:
		NET_COAP_NBIOT_Event_StopMode(pClient);
		break;
		
	case HARDWARE_REBOOT:
		NET_COAP_NBIOT_Event_HardwareReboot(pClient);
		break;
	
	case MODULE_CHECK:
		NET_COAP_NBIOT_Event_ModuleCheck(pClient);
		break;
	
	case PARAMETER_CONFIG:
		NET_COAP_NBIOT_Event_ParameterConfig(pClient);
		break;
	
	case ICCID_CHECK:
		NET_COAP_NBIOT_Event_SimICCIDCheck(pClient);
		break;
	
	case MISC_EQUIP_CONFIG:
		NET_COAP_NBIOT_Event_MiscEquipConfig(pClient);
		break;
	
	case ATTACH_CHECK:
		NET_COAP_NBIOT_Event_AttachCheck(pClient);
		break;
	
	case ATTACH_EXECUTE:
		NET_COAP_NBIOT_Event_AttachExecute(pClient);
		break;
	
	case ATTACH_INQUIRE:
		NET_COAP_NBIOT_Event_AttachInquire(pClient);
		break;
	
	case PARAMETER_CHECKOUT:
		NET_COAP_NBIOT_Event_PatameterCheckOut(pClient);
		break;
	
	case MINIMUM_FUNCTIONALITY:
		NET_COAP_NBIOT_Event_MinimumFunctionality(pClient);
		break;
	
	case FULL_FUNCTIONALITY:
		NET_COAP_NBIOT_Event_FullFunctionality(pClient);
		break;
	
	case CDP_SERVER_CHECK:
		NET_COAP_NBIOT_Event_CDPServerCheck(pClient);
		break;
	
	case CDP_SERVER_CONFIG:
		NET_COAP_NBIOT_Event_CDPServerConfig(pClient);
		break;
	
	case SEND_DATA:
		NET_COAP_NBIOT_Event_SendData(pClient);
		break;
	
	case RECV_DATA:
		NET_COAP_NBIOT_Event_RecvData(pClient);
		break;
	
	case EXECUT_DOWNLINK_DATA:
		NET_COAP_NBIOT_Event_ExecutDownlinkData(pClient);
		break;
	
	case MQTTSN_PROCESS_STACK:
		pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	
	case DNS_PROCESS_STACK:
		pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		break;
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_StopMode(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_StopMode			: 停止模式
 @Input				pClient							: NBIOT客户端实例
 @Return				void
 @attention			当30分钟或有数据需要发送时退出停止模式尝试重启NB,开启NB运行
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_StopMode(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	static unsigned char CoapSendMessageIndex;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 1800;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
		/* NBIOT Module Poweroff */
		NBIOT_Neul_NBxx_HardwarePoweroff(pClient);
		/* Send Message Index */
		CoapSendMessageIndex = NET_Coap_Message_SendDataRear();
	}
	
	if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
		/* Dictate TimeOut */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
	}
	else {
		/* Dictate isn't TimeOut */
		if (NET_Coap_Message_SendDataRear() != CoapSendMessageIndex) {
			/* Have new coap message need to Send */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		}
		else {
			pClient->DictateRunCtl.dictateEvent = STOP_MODE;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_HardwareReboot(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_HardwareReboot	: 硬件重启
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_HardwareReboot(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_HardwareReboot(pClient, 8000) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = MODULE_CHECK;
		pClient->DictateRunCtl.dictateRebootFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT HDReboot Ok, Baud : %d", NBIOTBaudRate.Baud);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateRebootFailureCnt++;
			if (pClient->DictateRunCtl.dictateRebootFailureCnt > 3) {
				pClient->DictateRunCtl.dictateRebootFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT HDReboot Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_ModuleCheck(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_ModuleCheck		: 模块检测
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_ModuleCheck(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if ((NBIOT_Neul_NBxx_CheckReadManufacturer(pClient) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadManufacturerModel(pClient) == NBIOT_OK)) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
		pClient->DictateRunCtl.dictateModuleCheckFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ModuleCheck Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateModuleCheckFailureCnt++;
			if (pClient->DictateRunCtl.dictateModuleCheckFailureCnt > 3) {
				pClient->DictateRunCtl.dictateModuleCheckFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = MODULE_CHECK;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ModuleCheck Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_ParameterConfig(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_ParameterConfig	: 参数配置
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_ParameterConfig(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadConfigUE(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = ICCID_CHECK;
		pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ConfigRead Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateParameterConfigFailureCnt++;
			if (pClient->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
				pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ConfigRead Fail");
#endif
		return;
	}
	
	if (pClient->Parameter.nconfig.autoConnect != NConfigTrue) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient, AutoConnect, NConfigTrue) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT AutoConnectSet %d Ok", NConfigTrue);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT AutoConnectSet %d Fail", NConfigTrue);
#endif
			return;
		}
	}
	
	if (pClient->Parameter.nconfig.crScrambling != NConfigTrue) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient, CrScrambling, NConfigTrue) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CrScramblingSet %d Ok", NConfigTrue);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CrScramblingSet %d Fail", NConfigTrue);
#endif
			return;
		}
	}
	
	if (pClient->Parameter.nconfig.crSiAvoid != NConfigTrue) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient, CrSiAvoid, NConfigTrue) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CrSiAvoidSet %d Ok", NConfigTrue);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CrSiAvoidSet %d Fail", NConfigTrue);
#endif
			return;
		}
	}
	
	if (pClient->Parameter.nconfig.combineAttach != NConfigFalse) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient, CombineAttach, NConfigFalse) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CombineAttachSet %d Ok", NConfigFalse);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CombineAttachSet %d Fail", NConfigFalse);
#endif
			return;
		}
	}
	
	if (pClient->Parameter.nconfig.cellReselection != NConfigFalse) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient, CellReselection, NConfigFalse) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CellReselectionSet %d Ok", NConfigFalse);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT CellReselectionSet %d Fail", NConfigFalse);
#endif
			return;
		}
	}
	
	if (pClient->Parameter.nconfig.enableBip != NConfigFalse) {
		if (NBIOT_Neul_NBxx_SetConfigUE(pClient, EnableBip, NConfigFalse) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ICCID_CHECK;
			pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT EnableBipSet %d Ok", NConfigFalse);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateParameterConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateParameterConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateParameterConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = PARAMETER_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("NBIOT EnableBipSet %d Fail", NConfigFalse);
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_SimICCIDCheck(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_SimICCIDCheck		: Sim卡检测
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_SimICCIDCheck(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadICCID(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = FULL_FUNCTIONALITY;
		pClient->DictateRunCtl.dictateSimICCIDCheckFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ICCIDCheck Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateSimICCIDCheckFailureCnt++;
			if (pClient->DictateRunCtl.dictateSimICCIDCheckFailureCnt > 3) {
				pClient->DictateRunCtl.dictateSimICCIDCheckFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = ICCID_CHECK;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("NBIOT ICCIDCheck Fail");
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_FullFunctionality(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_FullFunctionality	: 完整功能
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_FullFunctionality(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadMinOrFullFunc(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = CDP_SERVER_CHECK;
		pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap FullFuncCheck Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt++;
			if (pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt > 3) {
				pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = FULL_FUNCTIONALITY;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap FullFuncCheck Fail");
#endif
		return;
	}
	
	if (pClient->Parameter.functionality != FullFunc) {
		if (NBIOT_Neul_NBxx_SetMinOrFullFunc(pClient, FullFunc) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = CDP_SERVER_CHECK;
			pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap FullFuncSet Ok");
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt++;
				if (pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt > 3) {
					pClient->DictateRunCtl.dictateFullFunctionalityFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = FULL_FUNCTIONALITY;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap FullFuncSet Fail");
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_MinimumFunctionality(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_MinimumFunctionality	: 最小功能
 @Input				pClient								: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_MinimumFunctionality(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadMinOrFullFunc(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = CDP_SERVER_CONFIG;
		pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap MinFuncCheck Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt++;
			if (pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt > 3) {
				pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = MINIMUM_FUNCTIONALITY;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap MinFuncCheck Fail");
#endif
		return;
	}
	
	if (pClient->Parameter.functionality != MinFunc) {
		if (NBIOT_Neul_NBxx_SetMinOrFullFunc(pClient, MinFunc) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = CDP_SERVER_CONFIG;
			pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap MinFuncSet Ok");
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt++;
				if (pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt > 3) {
					pClient->DictateRunCtl.dictateMinimumFunctionalityFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = MINIMUM_FUNCTIONALITY;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap MinFuncSet Fail");
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_CDPServerCheck(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_CDPServerCheck	: CDP服务器查询
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_CDPServerCheck(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadCDPServer(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = CDP_SERVER_CHECK;
		pClient->DictateRunCtl.dictateCDPServerCheckFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap CDPRead Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateCDPServerCheckFailureCnt++;
			if (pClient->DictateRunCtl.dictateCDPServerCheckFailureCnt > 3) {
				pClient->DictateRunCtl.dictateCDPServerCheckFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = CDP_SERVER_CHECK;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap CDPRead Fail");
#endif
	}
	
	if ((strcmp((const char*)pClient->Parameter.cdpserver.CDPServerHost, COAPCDPADDR) != 0) || (pClient->Parameter.cdpserver.CDPServerPort != COAPCDPPORT)) {
		/* CDP Server Mast be Config */
		pClient->DictateRunCtl.dictateEvent = MINIMUM_FUNCTIONALITY;
	}
	else {
		/* CDP Server Needn't be Config */
		pClient->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_CDPServerConfig(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_CDPServerConfig	: CDP服务器配置
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_CDPServerConfig(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadCDPServer(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = FULL_FUNCTIONALITY;
		pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap CDPRead Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt++;
			if (pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt > 3) {
				pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = CDP_SERVER_CONFIG;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap CDPRead Fail");
#endif
	}
	
	if ((strcmp((const char*)pClient->Parameter.cdpserver.CDPServerHost, COAPCDPADDR) != 0) || (pClient->Parameter.cdpserver.CDPServerPort != COAPCDPPORT)) {
		/* CDP Server Mast be Config */
		if (NBIOT_Neul_NBxx_SetCDPServer(pClient, COAPCDPADDR, COAPCDPPORT) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = FULL_FUNCTIONALITY;
			pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap CDPSet Ok");
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateCDPServerConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = CDP_SERVER_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap CDPSet Fail");
#endif
			return;
		}
	}
	else {
		/* CDP Server Needn't be Config */
		pClient->DictateRunCtl.dictateEvent = FULL_FUNCTIONALITY;
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_MiscEquipConfig(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_MiscEquipConfig	: 其他配置
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_MiscEquipConfig(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if ((NBIOT_Neul_NBxx_CheckReadSupportedBands(pClient) == NBIOT_OK) &&
	    (NBIOT_Neul_NBxx_CheckReadNewMessageIndications(pClient) == NBIOT_OK) &&
	    (NBIOT_Neul_NBxx_CheckReadSentMessageIndications(pClient) == NBIOT_OK)) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap MiscRead Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt++;
			if (pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt > 3) {
				pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap MiscRead Fail");
#endif
		return;
	}
	
	if (pClient->Parameter.band != COAP_NBIOT_BAND) {
		if (NBIOT_Neul_NBxx_SetSupportedBands(pClient, COAP_NBIOT_BAND) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ATTACH_CHECK;
			pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap BandSet %d Ok", COAP_NBIOT_BAND);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap BandSet %d Fail", COAP_NBIOT_BAND);
#endif
			return;
		}
	}
	
	if (pClient->Parameter.nnmistate != CloseFunc) {
		if (NBIOT_Neul_NBxx_SetNewMessageIndications(pClient, CloseFunc) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ATTACH_CHECK;
			pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap NNMISet %d Ok", CloseFunc);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap NNMISet %d Fail", CloseFunc);
#endif
			return;
		}
	}
	
	if (pClient->Parameter.nsmistate != CloseFunc) {
		if (NBIOT_Neul_NBxx_SetSentMessageIndications(pClient, CloseFunc) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = ATTACH_CHECK;
			pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap NSMISet %d Ok", CloseFunc);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt++;
				if (pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt > 3) {
					pClient->DictateRunCtl.dictateMiscEquipConfigFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = MISC_EQUIP_CONFIG;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap NSMISet %d Fail", CloseFunc);
#endif
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_AttachCheck(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_AttachCheck		: 注网检查
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_AttachCheck(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadAttachOrDetach(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		pClient->DictateRunCtl.dictateAttachCheckFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap CGATT %d Ok", pClient->Parameter.netstate);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateAttachCheckFailureCnt++;
			if (pClient->DictateRunCtl.dictateAttachCheckFailureCnt > 3) {
				pClient->DictateRunCtl.dictateAttachCheckFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = ATTACH_CHECK;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap CGATT %d Fail", pClient->Parameter.netstate);
#endif
		return;
	}
	
	if (pClient->Parameter.netstate != Attach) {
		pClient->DictateRunCtl.dictateEvent = ATTACH_EXECUTE;
	}
	else {
		pClient->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_AttachExecute(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_AttachExecute		: 注网进行
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_AttachExecute(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_SetAttachOrDetach(pClient, Attach) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		pClient->DictateRunCtl.dictateAttachExecuteFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap Set CGATT %d Ok", Attach);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateAttachExecuteFailureCnt++;
			if (pClient->DictateRunCtl.dictateAttachExecuteFailureCnt > 3) {
				pClient->DictateRunCtl.dictateAttachExecuteFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = ATTACH_EXECUTE;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap Set CGATT %d Fail", Attach);
#endif
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_AttachInquire(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_AttachInquire		: 注网查询
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_AttachInquire(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 60;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_CheckReadAttachOrDetach(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap CGATT %d Ok", pClient->Parameter.netstate);
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateAttachInquireFailureCnt++;
			if (pClient->DictateRunCtl.dictateAttachInquireFailureCnt > 3) {
				pClient->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap CGATT %d Fail", pClient->Parameter.netstate);
#endif
		return;
	}
	
	if (pClient->Parameter.netstate != Attach) {
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateAttachInquireFailureCnt++;
			if (pClient->DictateRunCtl.dictateAttachInquireFailureCnt > 3) {
				pClient->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			pClient->DictateRunCtl.dictateEvent = ATTACH_INQUIRE;
		}
	}
	else {
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = PARAMETER_CHECKOUT;
		pClient->DictateRunCtl.dictateAttachInquireFailureCnt = 0;
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_PatameterCheckOut(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_PatameterCheckOut	: 参数检出
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_PatameterCheckOut(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if ((NBIOT_Neul_NBxx_CheckReadIMEI(pClient) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadIMEISV(pClient) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadIMSI(pClient) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadCGPADDR(pClient) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadCGDCONT(pClient) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadRSSI(pClient) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadStatisticsCELL(pClient) == NBIOT_OK) && 
	    (NBIOT_Neul_NBxx_CheckReadDateTime(pClient) == NBIOT_OK)) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = SEND_DATA;
		pClient->DictateRunCtl.dictatePatameterCheckOutFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap Patameter Check Ok");
#endif
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictatePatameterCheckOutFailureCnt++;
			if (pClient->DictateRunCtl.dictatePatameterCheckOutFailureCnt > 3) {
				pClient->DictateRunCtl.dictatePatameterCheckOutFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = PARAMETER_CHECKOUT;
		}
#ifdef COAP_DEBUG_LOG_RF_PRINT
		Radio_Trf_Debug_Printf("Coap Patameter Check Fail");
#endif
		return;
	}
	
	/* Set System Time */
	RTC_Set_Date(pClient->Parameter.dataTime.year, pClient->Parameter.dataTime.month, pClient->Parameter.dataTime.day);
	RTC_Set_Time(pClient->Parameter.dataTime.hour, pClient->Parameter.dataTime.min,   pClient->Parameter.dataTime.sec);
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_SendData(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_SendData			: 发送数据
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_SendData(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	int SendSentNum = 0;
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	/* Data packets need to be sent*/
	if (NET_Coap_Message_SendDataDequeue(pClient->Sendbuf, (unsigned short *)&pClient->Sendlen) == true) {
		if (NBIOT_Neul_NBxx_CheckReadAttachOrDetach(pClient) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEvent = SEND_DATA;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap CGATT %d Ok", pClient->Parameter.netstate);
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateSendDataFailureCnt++;
				if (pClient->DictateRunCtl.dictateSendDataFailureCnt > 3) {
					pClient->DictateRunCtl.dictateSendDataFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = SEND_DATA;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap CGATT %d Fail", pClient->Parameter.netstate);
#endif
			return;
		}
		
		if (pClient->Parameter.netstate != Attach) {
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateSendDataFailureCnt++;
				if (pClient->DictateRunCtl.dictateSendDataFailureCnt > 3) {
					pClient->DictateRunCtl.dictateSendDataFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				pClient->DictateRunCtl.dictateEvent = SEND_DATA;
			}
			return;
		}
		
		if ((NBIOT_Neul_NBxx_QuerySendMessageCOAPPayload(pClient) == NBIOT_OK) && 
		    (NBIOT_Neul_NBxx_QueryReadMessageCOAPPayload(pClient) == NBIOT_OK)) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEvent = SEND_DATA;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap NQMGS NQMGR Ok");
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateSendDataFailureCnt++;
				if (pClient->DictateRunCtl.dictateSendDataFailureCnt > 3) {
					pClient->DictateRunCtl.dictateSendDataFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = SEND_DATA;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap NQMGS NQMGR Fail");
#endif
			return;
		}
		
		SendSentNum = pClient->Parameter.coapSendMessage.sent;
		
		/* 发送负载数据 */
		if (NBIOT_Neul_NBxx_SendCOAPPayload(pClient) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEvent = SEND_DATA;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap Send Payload Ok");
#endif
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateSendDataFailureCnt++;
				if (pClient->DictateRunCtl.dictateSendDataFailureCnt > 3) {
					pClient->DictateRunCtl.dictateSendDataFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = SEND_DATA;
			}
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap Send Payload Fail");
#endif
			return;
		}
		
		if (NBIOT_Neul_NBxx_QuerySendMessageCOAPPayload(pClient) == NBIOT_OK) {
			/* Dictate execute is Success */
			pClient->DictateRunCtl.dictateEvent = SEND_DATA;
		}
		else {
			/* Dictate execute is Fail */
			if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
				/* Dictate TimeOut */
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
				pClient->DictateRunCtl.dictateSendDataFailureCnt++;
				if (pClient->DictateRunCtl.dictateSendDataFailureCnt > 3) {
					pClient->DictateRunCtl.dictateSendDataFailureCnt = 0;
					pClient->DictateRunCtl.dictateEvent = STOP_MODE;
				}
			}
			else {
				/* Dictate isn't TimeOut */
				pClient->DictateRunCtl.dictateEvent = SEND_DATA;
			}
			return;
		}
		
		if ((SendSentNum + 1) != pClient->Parameter.coapSendMessage.sent) {
			/* Send Data Fail */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateSendDataFailureCnt++;
			if (pClient->DictateRunCtl.dictateSendDataFailureCnt > 3) {
				pClient->DictateRunCtl.dictateSendDataFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Send Data Success */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = RECV_DATA;
			pClient->DictateRunCtl.dictateSendDataFailureCnt = 0;
		}
	}
	/* No packets need to be sent */
	else {
		pClient->DictateRunCtl.dictateEnable = false;
		pClient->DictateRunCtl.dictateEvent = EXECUT_DOWNLINK_DATA;
		pClient->DictateRunCtl.dictateSendDataFailureCnt = 0;
	}
}

/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_RecvData(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_RecvData			: 接收数据
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_RecvData(NBIOT_ClientsTypeDef* pClient)
{
	Stm32_CalculagraphTypeDef dictateRunTime;
	u8 COAPFeedBackData[] = {0xAA, 0xBB};									//COAP反馈包数据
	
	/* It is the first time to execute */
	if (pClient->DictateRunCtl.dictateEnable != true) {
		pClient->DictateRunCtl.dictateEnable = true;
		pClient->DictateRunCtl.dictateTimeoutSec = 30;
		Stm32_Calculagraph_CountdownSec(&dictateRunTime, pClient->DictateRunCtl.dictateTimeoutSec);
		pClient->DictateRunCtl.dictateRunTime = dictateRunTime;
	}
	
	if (NBIOT_Neul_NBxx_QueryReadMessageCOAPPayload(pClient) == NBIOT_OK) {
		/* Dictate execute is Success */
		pClient->DictateRunCtl.dictateEvent = RECV_DATA;
	}
	else {
		/* Dictate execute is Fail */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateRecvDataFailureCnt++;
			if (pClient->DictateRunCtl.dictateRecvDataFailureCnt > 3) {
				pClient->DictateRunCtl.dictateRecvDataFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = RECV_DATA;
		}
		return;
	}
	
	if (pClient->Parameter.coapReadMessage.buffered != 0) {
		/* Has Data Need Receive */
		for (int index = 0; index < pClient->Parameter.coapReadMessage.buffered; index++) {
			
			/* 读取负载数据 */
			if (NBIOT_Neul_NBxx_ReadCOAPPayload(pClient) == NBIOT_OK) {
				/* Dictate execute is Success */
				pClient->DictateRunCtl.dictateEvent = pClient->DictateRunCtl.dictateEvent;
			}
			else {
				/* Dictate execute is Fail */
				if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
					/* Dictate TimeOut */
					pClient->DictateRunCtl.dictateEnable = false;
					pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
					pClient->DictateRunCtl.dictateRecvDataFailureCnt++;
					if (pClient->DictateRunCtl.dictateRecvDataFailureCnt > 3) {
						pClient->DictateRunCtl.dictateRecvDataFailureCnt = 0;
						pClient->DictateRunCtl.dictateEvent = STOP_MODE;
					}
				}
				else {
					/* Dictate isn't TimeOut */
					pClient->DictateRunCtl.dictateEvent = pClient->DictateRunCtl.dictateEvent;
				}
				return;
			}
			
			if (memcmp((const char*)pClient->Recvbuf, COAPFeedBackData, sizeof(COAPFeedBackData)) == 0) {
				/* Is Feedback */
				NET_Coap_Message_SendDataOffSet();
				pClient->DictateRunCtl.dictateEnable = false;
				pClient->DictateRunCtl.dictateEvent = SEND_DATA;
				pClient->DictateRunCtl.dictateRecvDataFailureCnt = 0;
#ifdef COAP_DEBUG_LOG_RF_PRINT
				Radio_Trf_Debug_Printf("Coap Recv Feedback Ok");
#endif
			}
			else {
				/* Not Feedback */
				NET_Coap_Message_RecvDataEnqueue(pClient->Recvbuf, pClient->Recvlen);
#ifdef COAP_DEBUG_LOG_RF_PRINT
				Radio_Trf_Debug_Printf("Coap Recv Data Ok");
#endif
			}
		}
	}
	else {
		/* Not Data Need Receive */
		if (Stm32_Calculagraph_IsExpiredSec(&pClient->DictateRunCtl.dictateRunTime) == true) {
			/* Dictate TimeOut */
			pClient->DictateRunCtl.dictateEnable = false;
			pClient->DictateRunCtl.dictateEvent = HARDWARE_REBOOT;
			pClient->DictateRunCtl.dictateRecvDataFailureCnt++;
			if (pClient->DictateRunCtl.dictateRecvDataFailureCnt > 3) {
				pClient->DictateRunCtl.dictateRecvDataFailureCnt = 0;
				pClient->DictateRunCtl.dictateEvent = STOP_MODE;
			}
		}
		else {
			/* Dictate isn't TimeOut */
			pClient->DictateRunCtl.dictateEvent = RECV_DATA;
#ifdef COAP_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("Coap Wait Recv Feedback");
#endif
		}
	}
}











/**********************************************************************************************************
 @Function			void NET_COAP_NBIOT_Event_ExecutDownlinkData(NBIOT_ClientsTypeDef* pClient)
 @Description			NET_COAP_NBIOT_Event_ExecutDownlinkData	: 下行数据处理
 @Input				pClient							: NBIOT客户端实例
 @Return				void
**********************************************************************************************************/
void NET_COAP_NBIOT_Event_ExecutDownlinkData(NBIOT_ClientsTypeDef* pClient)
{
	if (NET_Coap_Message_RecvDataDequeue(pClient->Recvbuf, (unsigned short*)&pClient->Recvlen) == true) {
		NET_Coap_Message_RecvDataOffSet();
		
		BEEP_CtrlRepeat_Extend(5, 25, 25);
		
		
		//Todo
		
		
		
		
		
		
		
	}
	
	pClient->DictateRunCtl.dictateEvent = SEND_DATA;
}




















/********************************************** END OF FLEE **********************************************/
