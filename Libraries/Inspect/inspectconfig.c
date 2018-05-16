/**
  *********************************************************************************************************
  * @file    inspectconfig.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-04-02
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "inspectconfig.h"
#include "inspectmessageoperate.h"
#include "stm32l1xx_config.h"
#include "platform_config.h"
#include "platform_map.h"
#include "hal_qmc5883l.h"
#include "hal_rtc.h"
#include "hal_beep.h"
#include "hal_temperature.h"
#include "radar_api.h"
#include "radio_rf_app.h"

SpotStatusTypedef				SpotStatusDataBackUp;

/**********************************************************************************************************
 @Function			void Inspect_Spot_Init(void)
 @Description			Inspect_Spot_Init			: 车位检测算法初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Inspect_Spot_Init(void)
{
	unsigned char i;
	
	talgo_register_mag_callback(QMC5883L_ReadData_Single);
	
	talgo_register_radar_callback(Radar_GetData);
	
	talgo_register_dprintf_callback(Radio_Trf_Debug_Printf);
	
	talgo_register_runningtime_callback(Stm32_GetSecondTick);
	
	talgo_register_unixtime_callback(RTC_GetUnixTimeToStamp);
	
	tradar_register_temperature_callback(TEMPERATURE_ADC_Read);
	
	talgo_register_rssi_callback(TCFG_Utility_Get_Nbiot_Rssi_UnCharVal);
	
#if RADAR_MODEL_TYPE == RADAR_MODEL_V1
	talgo_set_model(TRADAR_V1);
#elif RADAR_MODEL_TYPE == RADAR_MODEL_V2
	tradar_set_sensetivity(TCFG_SystemData.Sensitivity);
	talgo_set_model(TRADAR_V2);
#else
	#error RADAR_MODEL_TYPE Define Error
#endif
	
	talgo_set_magmod(TCFG_EEPROM_GetMagMode());
	
	i = 4 + TCFG_EEPROM_GetRadarRange();
	if ((i < TRADAR_RANGE_0M5) || (i > TRADAR_RANGE_1M6)) {
		i = TRADAR_RANGE_1M1;
	}
	tradar_set_distance_range(i);
}

/**********************************************************************************************************
 @Function			void Inspect_Spot_ExistenceDetect(void)
 @Description			Inspect_Spot_ExistenceDetect	: 车位检测算法处理
 @Input				void
 @Return				void
**********************************************************************************************************/
void Inspect_Spot_ExistenceDetect(void)
{
	TALGO_SPOTSTATUS SpotStatus;
	SpotStatusTypedef SpotStatusData;
	static char noStatusSent = 1;
	static uint32_t time2send_spot = 0;
	static uint8_t status_pre = SPOT_CAR_UNKNOWN;
	
	talgo_set_sensitivity(TCFG_SystemData.Sensitivity - 1);
	
	SpotStatus = (TALGO_SPOTSTATUS)talgro_vechile_judge(&SpotStatusData);
	
	/* 车位状态改变(实时) */
	if ((SpotStatus == SPOT_OCCUPY2FREE) || (SpotStatus == SPOT_FREE2OCCUPY)) {
		/* -先记录状态数据, 等连续5~10秒磁场无波动, 那么进行数据发送- */
		SpotStatusDataBackUp = SpotStatusData;
		noStatusSent = 0;
		Radio_Trf_Do_Heartbeat();
		
		if (SpotStatus == SPOT_OCCUPY2FREE) {
			BEEP_CtrlRepeat_Extend(1, 30, 70);
		}
		else if (SpotStatus == SPOT_FREE2OCCUPY) {
			BEEP_CtrlRepeat_Extend(2, 30, 70);
		}
	}
	/* 车位状态保持不变(等待) */
	else if (((SpotStatus == SPOT_LONGFREE) || (SpotStatus == SPOT_LONGOCCUPY)) && noStatusSent) {
		SpotStatusData.spot_count = 0;
		noStatusSent = 0;
		
		if (SpotStatus == SPOT_LONGFREE) {
			SpotStatusData.spot_status = SPOT_CAR_FREE;
		}
		else if (SpotStatus == SPOT_LONGOCCUPY) {
			SpotStatusData.spot_status = SPOT_CAR_OCCUPY;
		}
		
		SpotStatusData.qmc5883lData.X_Now					= Qmc5883lData.X_Now;
		SpotStatusData.qmc5883lData.Y_Now					= Qmc5883lData.Y_Now;
		SpotStatusData.qmc5883lData.Z_Now					= Qmc5883lData.Z_Now;
		SpotStatusData.qmc5883lData.X_Back					= Qmc5883lData.X_Back;
		SpotStatusData.qmc5883lData.Y_Back					= Qmc5883lData.Y_Back;
		SpotStatusData.qmc5883lData.Z_Back					= Qmc5883lData.Z_Back;
		
		SpotStatusData.qmc5883lDiff.BackVal_Diff			= Qmc5883lDiff.BackVal_Diff;
		SpotStatusData.qmc5883lDiff.GraduaVal_Diff			= Qmc5883lDiff.GraduaVal_Diff;
		
		SpotStatusData.radarData.NotargetNum				= sRadarData.NotargetNum;
		SpotStatusData.radarData.DismagNum					= sRadarData.DismagNum;
		SpotStatusData.radarData.CoverNum					= sRadarData.CoverNum;
		SpotStatusData.radarData.DisVal					= sRadarData.DisVal;
		SpotStatusData.radarData.MagVal					= sRadarData.MagVal;
		SpotStatusData.radarData.Diff						= sRadarData.Diff;
		SpotStatusData.radarData.Diff_v2					= sRadarData.Diff_v2;
		SpotStatusData.radarData.timedomain_square_dif		= sRadarData.timedomain_square_dif;
		
		SpotStatusData.unixTime							= TCFG_EEPROM_GetEventTime();
		SpotStatusData.timeCounter						= Stm32_GetSecondTick();
		
		time2send_spot = Stm32_GetSecondTick();
		Inspect_Message_SpotStatusEnqueue(SpotStatusData);
	}
	
	/* 心跳数据包 */
	if ((Stm32_GetSecondTick() > (time2send_spot + 3600 * TCFG_EEPROM_GetNbiotHeart())) && ((Inspect_Detect_LastTime + 10) > Stm32_GetSecondTick())) {
		Inspect_Message_SpotStatusDequeueEx(&SpotStatusData);
		if (SpotStatusData.timeCounter != 0) {
			SpotStatusData.spot_count = 0;
			
			SpotStatusData.qmc5883lData.X_Now				= Qmc5883lData.X_Now;
			SpotStatusData.qmc5883lData.Y_Now				= Qmc5883lData.Y_Now;
			SpotStatusData.qmc5883lData.Z_Now				= Qmc5883lData.Z_Now;
			SpotStatusData.qmc5883lData.X_Back				= Qmc5883lData.X_Back;
			SpotStatusData.qmc5883lData.Y_Back				= Qmc5883lData.Y_Back;
			SpotStatusData.qmc5883lData.Z_Back				= Qmc5883lData.Z_Back;
			
			SpotStatusData.qmc5883lDiff.BackVal_Diff		= Qmc5883lDiff.BackVal_Diff;
			SpotStatusData.qmc5883lDiff.GraduaVal_Diff		= Qmc5883lDiff.GraduaVal_Diff;
			
			SpotStatusData.radarData.NotargetNum			= sRadarData.NotargetNum;
			SpotStatusData.radarData.DismagNum				= sRadarData.DismagNum;
			SpotStatusData.radarData.CoverNum				= sRadarData.CoverNum;
			SpotStatusData.radarData.DisVal				= sRadarData.DisVal;
			SpotStatusData.radarData.MagVal				= sRadarData.MagVal;
			SpotStatusData.radarData.Diff					= sRadarData.Diff;
			SpotStatusData.radarData.Diff_v2				= sRadarData.Diff_v2;
			SpotStatusData.radarData.timedomain_square_dif	= sRadarData.timedomain_square_dif;
			
			SpotStatusData.timeCounter					= Stm32_GetSecondTick();
			
			Inspect_Message_SpotStatusEnqueue(SpotStatusData);
		}
		time2send_spot = Stm32_GetSecondTick();
	}
	
	/* 数据存于缓存 */
	if (SpotStatusDataBackUp.timeCounter != 0) {
		if ((time2send_spot + 8) < Stm32_GetSecondTick()) {
			if (status_pre != SpotStatusDataBackUp.spot_status) {
				Inspect_Message_SpotStatusEnqueue(SpotStatusDataBackUp);
				status_pre = SpotStatusDataBackUp.spot_status;
				time2send_spot = Stm32_GetSecondTick();
			}
			SpotStatusDataBackUp.timeCounter = 0;
		}
	}
}

/********************************************** END OF FLEE **********************************************/
