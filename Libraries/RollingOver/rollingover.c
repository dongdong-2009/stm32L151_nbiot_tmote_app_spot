/**
  *********************************************************************************************************
  * @file    rollingover.c
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

#include "rollingover.h"
#include "platform_config.h"
#include "platform_map.h"
#include "hal_switch.h"
#include "hal_beep.h"
#include "radar_api.h"
#include "radio_rf_app.h"
#include "inspectconfig.h"
#include "delay.h"
#include "usart.h"
#include "main.h"

unsigned int TimeBeforeIdle	= 0;											//TimeBeforeIdle
unsigned char InitSensorBackgroundCntdown = 0;								//InitSensorBackgroundCntdown

/**********************************************************************************************************
 @Function			void RollingOverPollExecution(void)
 @Description			RollingOverPollExecution
 @Input				void
 @Return				void
 @attention			翻转检测处理
**********************************************************************************************************/
void RollingOverPollExecution(void)
{
	/* 水银开关工作模式选择 */
	if (Mercury_Read() != 0) {
		/* 断开 */
		RollingOverMercuryBreak();
	}
	else {
		/* 闭合 */
		RollingOverMercuryClose();
	}
}

/**********************************************************************************************************
 @Function			bool RollingOverCheckToActive(void)
 @Description			RollingOverCheckToActive
 @Input				void
 @Return				bool
 @attention			翻转检测
**********************************************************************************************************/
bool RollingOverCheckToActive(void)
{
	static int32_t rollingover_time[5] = {-100, -100, -100, -100, -100};
	u8 SetEEPOMTick = 0;
	
	if (TCFG_EEPROM_GetActiveDevice()) {
		DeviceActivedMode = true;
		rollingover_time[0] = rollingover_time[1];
		rollingover_time[1] = rollingover_time[2];
		rollingover_time[2] = rollingover_time[3];
		rollingover_time[3] = rollingover_time[4];
		rollingover_time[4] = Stm32_GetSecondTick();
#ifdef ROLLINGOVER_LOG_RF_PRINT
		Radio_Trf_Printf("rolup:%d,%d,%d,%d,%d", rollingover_time[0], rollingover_time[1], rollingover_time[2], rollingover_time[3], rollingover_time[4]);
		Radio_Trf_Printf("rolup:[4]-[0]=%d", (rollingover_time[4] - rollingover_time[0]));
#endif
		if (((rollingover_time[4] - rollingover_time[0]) < 24) && ((rollingover_time[4] - rollingover_time[0]) > 8)) {
			InitSensorBackgroundCntdown = 3;
			rollingover_time[0] = 0;
			rollingover_time[1] = 0;
			rollingover_time[2] = 0;
			rollingover_time[3] = 0;
			rollingover_time[4] = 0;
#ifdef ROLLINGOVER_LOG_RF_PRINT
			Radio_Trf_Printf("Up Rolling OK Cntdown:%d", InitSensorBackgroundCntdown);
#endif
		}
	}
	else {
		rollingover_time[0] = rollingover_time[1];
		rollingover_time[1] = rollingover_time[2];
		rollingover_time[2] = rollingover_time[3];
		rollingover_time[3] = rollingover_time[4];
		rollingover_time[4] = Stm32_GetSecondTick();
#ifdef ROLLINGOVER_LOG_RF_PRINT
		Radio_Trf_Printf("roldw:%d,%d,%d,%d,%d", rollingover_time[0], rollingover_time[1], rollingover_time[2], rollingover_time[3], rollingover_time[4]);
		Radio_Trf_Printf("rolup:[4]-[0]=%d", (rollingover_time[4] - rollingover_time[0]));
#endif
		if (((rollingover_time[4] - rollingover_time[0]) < 12) && ((rollingover_time[4] - rollingover_time[0]) > 8)) {
			do {
				Delay_MS(10);
				TCFG_EEPROM_SetActiveDevice(1);
				SetEEPOMTick++;
			}
			while ((TCFG_EEPROM_GetActiveDevice() != 1) && (SetEEPOMTick <= 100));
			rollingover_time[0] = 0;
			rollingover_time[1] = 0;
			rollingover_time[2] = 0;
			rollingover_time[3] = 0;
			rollingover_time[4] = 0;
#ifdef ROLLINGOVER_LOG_RF_PRINT
			Radio_Trf_Printf("Down Rolling OK EEPROM:%d", TCFG_EEPROM_GetActiveDevice());
#endif
		}
		else {
			DeviceActivedMode = false;
		}
	}
	
	return DeviceActivedMode;
}

/**********************************************************************************************************
 @Function			void RollingOverMercuryBreak(void)
 @Description			RollingOverMercuryBreak
 @Input				void
 @Return				void
 @attention			水银开关断开工作
**********************************************************************************************************/
void RollingOverMercuryBreak(void)
{
	if (DeviceIdleMode == true) {														//上次为倒置
		Delay_MS(100);
		if (Mercury_Read() != 0) {
			if (RollingOverCheckToActive() == true) {
				/* 进入正置工作 (Idle : false | Active : true) */
				MainRollingEnteredUpWork();												//已进入正放工作状态
			}
			else {
				/* 未进入正置工作 (Idle : false | Active : false) */
				MainRollingEnteringUpWork();												//将进入正放工作状态
			}
			DeviceIdleMode = false;
		}
	}
	else {																		//上次为正置
		if (DeviceActivedMode == true) {
			/* 正置工作 (Idle : false | Active : true) */
			MainRollingUpwardsActived();													//正放工作
			if (InitSensorBackgroundCntdown > 0) {
				RollingOverCheckToInitSensorBackground();
				InitSensorBackgroundCntdown--;
			}
		}
		else {
			/* 正置休眠 (Idle : false | Active : false) */
			MainRollingUpwardsSleep();													//正放休眠
		}
		
		TimeBeforeIdle = Stm32_GetSecondTick();
	}
}

/**********************************************************************************************************
 @Function			void RollingOverMercuryClose(void)
 @Description			RollingOverMercuryClose
 @Input				void
 @Return				void
 @attention			水银开关闭合休眠
**********************************************************************************************************/
void RollingOverMercuryClose(void)
{
	if (DeviceIdleMode == false) {
		BEEP_CtrlRepeat_Extend(1, 100, 0);
	}
	
	if (DeviceActivedMode == true) {
		/* 倒置工作 */
		if (DeviceIdleMode == false) {
			MainRollingEnteredDownWork();													//已进入倒放工作状态
			DeviceIdleMode = true;
		}
		
		/* 15Min 后进入休眠 */
		if ((TimeBeforeIdle + ROLLINGOVER_ENTER_SLEEP_TIME) < Stm32_GetSecondTick()) {
			DeviceActivedMode = false;
			TCFG_EEPROM_SetActiveDevice(0);
			MainRollingEnteredDownSleep();												//已进入倒放休眠状态
#ifdef ROLLINGOVER_LOG_RF_PRINT
			Radio_Trf_Printf("Sleep Begin TBI:%d, SYS:%d, EEPROM:%d", TimeBeforeIdle, Stm32_GetSecondTick(), TCFG_EEPROM_GetActiveDevice());
#endif
		}
		/* 15Sec 内处理 */
		else if ((TimeBeforeIdle + ROLLINGOVER_WAIT_SLEEP_TIME) > Stm32_GetSecondTick()) {
			MainRollingEnteredDownSleepKeepActived();										//将进入倒放休眠状态前保持工作
#ifdef ROLLINGOVER_LOG_RF_PRINT
			Radio_Trf_Printf("Sleep Before TBI:%d, SYS:%d, EEPROM:%d", TimeBeforeIdle, Stm32_GetSecondTick(), TCFG_EEPROM_GetActiveDevice());
#endif
		}
		/* 15Sec ~ 15Min */
		else {
			MainRollingEnteringDownSleep();												//将进入倒放休眠
#ifdef ROLLINGOVER_LOG_RF_PRINT
			Radio_Trf_Printf("Sleep Right TBI:%d, SYS:%d, EEPROM:%d", TimeBeforeIdle, Stm32_GetSecondTick(), TCFG_EEPROM_GetActiveDevice());
#endif
		}
	}
	else {
		/* 倒置休眠 */
		MainRollingDownSleep();															//倒放休眠
	}
	
	DeviceIdleMode = true;
}

/**********************************************************************************************************
 @Function			void RollingOverCheckToInitSensorBackground(void)
 @Description			RollingOverCheckToInitSensorBackground
 @Input				void
 @Return				void
 @attention			检查是否需要初始化传感器背景
**********************************************************************************************************/
void RollingOverCheckToInitSensorBackground(void)
{
	if (InitSensorBackgroundCntdown > 1) {
		BEEP_CtrlRepeat_Extend(6, 75, 150);
	}
	else if (InitSensorBackgroundCntdown > 0) {
		Radar_InitBackground(TO_SAVE_RADAR_BACKGROUND);
		QMC5883L_InitBackgroud();
		Radio_Trf_Printf("Init Sensor Background");
		BEEP_CtrlRepeat_Extend(1, 800, 0);
	}
}

/********************************************** END OF FLEE **********************************************/
