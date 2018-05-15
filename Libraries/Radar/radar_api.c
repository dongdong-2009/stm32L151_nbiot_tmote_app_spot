/**
  *********************************************************************************************************
  * @file    radar_api.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-31
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  * 
  *
  *********************************************************************************************************
  */

#include "radar_api.h"
#include "radar_adc.h"
#include "radar_dac.h"
#include "radar_dma.h"
#include "radar_timer.h"
#include "stm32_dsp.h"
#include "platform_config.h"
#include "platform_map.h"
#include "stm32l1xx_config.h"
#include "hal_eeprom.h"
#include "hal_beep.h"
#include "hal_vptat.h"
#include "hal_temperature.h"
#include "radio_rf_app.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include <math.h>

#define SAMPLE_NUM	TRADAR_SAMPLE_NUM

#define	VDDA2V8														//VDDA = 2.8V 0~2633 -> 0~1.8V
//#define	VDDA3V3														//VDDA = 3.3V 0~2234 -> 0~1.8V
//#define	RADAR_VPTAT

#ifdef VDDA3V3
	#define	RADER_RANGE				6
#else
	#define	RADER_RANGE				8
#endif

short RADER_LOW = 250;
short val_vptat, val_vptat_adjust;
short val_temp, val_temp_adjust;

u16 fre_magBG[TRADAR_BACKGROUND_NUM] = {10};
s16 time_magBG[TRADAR_BACKGROUND_NUM] = {10};

u32 sample_array0[SAMPLE_NUM] = {0};
u32 sample_array1[SAMPLE_NUM] = {0};

int n_array = 0;
int flag_in_array = 0;
int flag_main_go = 0;
u8  bgTimes = 1;
u8  radar_trigged_again = 0;

/**********************************************************************************************************
 @Function			void Radar_Init(void)
 @Description			雷达初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_Init(void)
{
#if RADAR_MODEL_TYPE == RADAR_MODEL_V1
	tradar_set_model(TRADAR_V1);
#elif RADAR_MODEL_TYPE == RADAR_MODEL_V2
	tradar_set_model(TRADAR_V2);
#else
	#error RADAR_MODEL_TYPE Define Error
#endif
	
	Radar_TIM2_Init(10-1, 32-1);																	//10us中断一次
	__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);															//先关闭TIM2,等待使用雷达时开启
	
	val_temp_adjust = TCFG_EEPROM_GetBackgroundTemp();
	
	FLASH_EEPROM_ReadBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));							//读取EEPROM背景值
	FLASH_EEPROM_ReadBuffer(EEPROM_BASE_ADDR1+256, (u8 *)time_magBG, sizeof(time_magBG));					//读取EEPROM背景值
	
	tradar_timedomain_background_set(time_magBG, (sizeof(time_magBG))/2);
	tradar_base_background_set(fre_magBG, (sizeof(fre_magBG))/2);
	tradar_background_set(fre_magBG, (sizeof(fre_magBG))/2);
}

/**********************************************************************************************************
 @Function			u8 Radar_CheckData(u32 datain_t[], u16 in_num)
 @Description			check whether the model is in use
 @Input				datain_t			: sample data
					in_num			: the data number
 @Return				TRADAR_ERROR		: 异常
					TRADAR_OK			: 正确
**********************************************************************************************************/
u8 Radar_CheckData(u32 datain_t[], u16 in_num)
{
	uint16_t i, count = 0;
	
	for (i = 0; i < in_num; i++) {
#ifdef VDDA2V8
		if ((datain_t[i] < 2390) || (datain_t[i] > 2480)) {
			count++;
		}
#else
		if ((datain_t[i] < 2390) || (datain_t[i] > 2470)) {
			count++;
		}
#endif		
	}
	
	if (count < 10) {
		return TRADAR_ERROR;
	}
	else {
		return TRADAR_OK;
	}
}

/**********************************************************************************************************
 @Function			void Radar_InitBG_Cmd(u32 v23456, u32 v7890a, u32 vbcdef, u32 vg)
 @Description			Radar_InitBG_Cmd
 @Input				
 @Return				void
**********************************************************************************************************/
void Radar_InitBG_Cmd(u32 v23456, u32 v7890a, u32 vbcdef, u32 vg)
{
	fre_magBG[2] = (v23456/100000000)%100;
	fre_magBG[3] = (v23456/1000000)%100;
	fre_magBG[4] = (v23456/10000)%100;
	fre_magBG[5] = (v23456/100)%100;
	fre_magBG[6] = v23456%100;
	
	fre_magBG[7] = (v7890a/100000000)%100;
	fre_magBG[8] = (v7890a/1000000)%100;
	fre_magBG[9] = (v7890a/10000)%100;
	fre_magBG[10] = (v7890a/100)%100;
	fre_magBG[11] = v7890a%100;
	
	fre_magBG[12] = (vbcdef/100000000)%100;
	fre_magBG[13] = (vbcdef/1000000)%100;
	fre_magBG[14] = (vbcdef/10000)%100;
	fre_magBG[15] = (vbcdef/100)%100;
	fre_magBG[16] = vbcdef%100;
	
	fre_magBG[17] = vg%100;
	
	FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));
	FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1+256, (u8 *)time_magBG, sizeof(time_magBG));
	TCFG_EEPROM_SetBackgroundTemp(val_temp);
	
	tradar_base_background_set(fre_magBG, (sizeof(fre_magBG))/2);
	tradar_background_set(fre_magBG, (sizeof(fre_magBG))/2);
}

/**********************************************************************************************************
 @Function			u8 Radar_InitBackground(char mode)
 @Description			雷达初始化背景值
 @Input				mode
 @Return				void
**********************************************************************************************************/
u8 Radar_InitBackground(char mode)
{
	RADARPOWER(ON);																			//开启雷达电源
	Delay_MS(500);
	RADAR_ENTER_CRITICAL_SECTION();
	
	n_array = 0;
	flag_in_array = 0;
	flag_main_go = 0;
	
	__HAL_TIM_ENABLE(&RADAR_TIM2_Handler);															//启动雷达定时器
	
	memset((void*)fre_magBG, 0, sizeof(fre_magBG));
	memset((void*)time_magBG, 0, sizeof(time_magBG));
	
	bgTimes = 31;																				//标记需初始化雷达背景
	
	while (1) {
		while (flag_main_go != 0) {
			if (Radar_CheckData(sample_array0, SAMPLE_NUM) == TRADAR_OK) {								//雷达数据校验
				if (bgTimes > 1) {
					if (flag_main_go == 1) {
						tradar_background_transform(sample_array0, SAMPLE_NUM, fre_magBG, time_magBG, (sizeof(fre_magBG))/2);
					}
					else {
						tradar_background_transform(sample_array1, SAMPLE_NUM, fre_magBG, time_magBG, (sizeof(fre_magBG))/2);
					}
					
					bgTimes--;
					if (bgTimes == 1) {															//初始化背景完成
						if (mode == TO_SAVE_RADAR_BACKGROUND) {										//保存背景
							FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));
							FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1+256, (u8 *)time_magBG, sizeof(time_magBG));
							TCFG_EEPROM_SetBackgroundTemp(val_temp);
							tradar_timedomain_background_set(time_magBG,(sizeof(time_magBG))/2);
							tradar_base_background_set(fre_magBG,(sizeof(fre_magBG))/2);
							tradar_background_set(fre_magBG, (sizeof(fre_magBG))/2);
							val_vptat_adjust = val_vptat;
							val_temp_adjust  = val_temp;
						}
						else if (mode == NOT_SAVE_RADAR_BACKGROUND) {								//不保存背景
							//微调雷达背景时,雷达背景值不能和之前的背景相差太多.
						}
						
						flag_main_go = 0;
						
						__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);										//雷达工作结束关闭定时器
						RADARPOWER(OFF);														//关闭雷达电源
						
						RADAR_EXIT_CRITICAL_SECTION();
						
						return TRADAR_OK;
					}
				}
			}
			else {
				__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);												//雷达工作结束关闭定时器
				RADARPOWER(OFF);																//关闭雷达电源
				
				RADAR_EXIT_CRITICAL_SECTION();
				
				return TRADAR_ERROR;
			}

#if RADAR_MODEL_TYPE == RADAR_MODEL_V1
			
#elif RADAR_MODEL_TYPE == RADAR_MODEL_V2
			RADARPOWER(OFF);																	//关闭雷达电源
#else
	#error RADAR_MODEL_TYPE Define Error
#endif
			BEEP_CtrlRepeat_Extend(1, 30, 0);
			
#if RADAR_MODEL_TYPE == RADAR_MODEL_V1
			
#elif RADAR_MODEL_TYPE == RADAR_MODEL_V2
			RADARPOWER(ON);																	//开启雷达电源
#else
	#error RADAR_MODEL_TYPE Define Error
#endif
			Delay_MS(30);
			
			flag_main_go = 0;
		}
	}
}

extern s16 tradar_time_mag_buff[TRADAR_BACKGROUND_NUM];
extern s16 tradar_time_magBG[TRADAR_BACKGROUND_NUM];
tradar_targetinfo_s radar_targetinfo;

/**********************************************************************************************************
 @Function			u8 Radar_GetData(tradar_targetinfo_s* pTargetinfo[], u8 dataNum)
 @Description			Radar_GetData	: 获取雷达数据
 @Input				dataNum 		: 雷达数据条数
 @Return				1 			: 异常
					0 			: 正确
**********************************************************************************************************/
u8 Radar_GetData(tradar_targetinfo_s* pTargetinfo[], u8 dataNum)
{
	u16 tradar_time_mag_buff_test[TRADAR_BACKGROUND_NUM];
	
	TCFG_AddRadarCount();
	RADARPOWER(ON);																			//开启雷达电源
	Delay_MS(10);
	RADAR_ENTER_CRITICAL_SECTION();
	
	n_array = 0;
	flag_in_array = 0;
	flag_main_go = 0;
	
	__HAL_TIM_ENABLE(&RADAR_TIM2_Handler);															//启动雷达定时器
	
	radar_trigged_again = 1;
	
	while (dataNum) {
		while (flag_main_go != 0) {
			if (flag_main_go == 1) {
				tradar_target_detect(sample_array0, SAMPLE_NUM, pTargetinfo[dataNum-1]);
			}
			else {
				tradar_target_detect(sample_array1, SAMPLE_NUM, pTargetinfo[dataNum-1]);
			}
			
			if (pTargetinfo[dataNum - 1]->strenth_total_diff > 255) {
				pTargetinfo[dataNum - 1]->strenth_total_diff = 255;
			}
			
			if ((pTargetinfo[dataNum-1]->pMagNow[2] <= 3)&&(pTargetinfo[dataNum-1]->pMagNow[3] <= 3)&&(pTargetinfo[dataNum-1]->pMagNow[4] <= 3)) {
				__NOP();
			}
			else {
				__NOP();
			}
			
			dataNum--;
					
			flag_main_go = 0;
			
			if (dataNum == 0) {
				break;
			}
			
			if (dataNum == 2) {
				memcpy(tradar_time_mag_buff_test, tradar_time_mag_buff, sizeof(tradar_time_mag_buff));
			}
		}
	}
	
	__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);															//雷达工作结束关闭定时器
	RADARPOWER(OFF);																			//关闭雷达电源
	
	RADAR_EXIT_CRITICAL_SECTION();
	
	memcpy(&radar_targetinfo, pTargetinfo[0], sizeof(radar_targetinfo));
	
#ifdef RADAR_DEBUG_LOG_RF_PRINT
	if (DEBUG_WORK == Radio_Trf_Get_Workmode()) {													//调试信息
		Radio_Trf_Debug_Printf("%dlow%d.%d-%dst%dds%ddf%d&%d;tdf%d.%d;%d,%d,%d,%d ",dataNum,RADER_LOW,val_temp,val_vptat,(uint32_t)radar_targetinfo.status,
				(uint32_t)radar_targetinfo.distance_cm,
				radar_targetinfo.strenth_total_diff,radar_targetinfo.strenth_total_diff_v2,
				radar_targetinfo.time_total_square_diff,radar_targetinfo.time_total_diff,
				radar_targetinfo.overnum[0],radar_targetinfo.overnum[1],radar_targetinfo.overnum_bg[0],radar_targetinfo.overnum_bg[1]);
		
		Radio_Trf_Debug_Printf("n:%02d%02d%02d%02d%02d;%02d%02d%02d%02d%02d;%02d%02d%02d%02d%02d%02d.%d.%d",
				pTargetinfo[0]->pMagNow[2],
				pTargetinfo[0]->pMagNow[3],pTargetinfo[0]->pMagNow[4],pTargetinfo[0]->pMagNow[5],
				pTargetinfo[0]->pMagNow[6],pTargetinfo[0]->pMagNow[7],pTargetinfo[0]->pMagNow[8],
				pTargetinfo[0]->pMagNow[9],pTargetinfo[0]->pMagNow[10],pTargetinfo[0]->pMagNow[11],
				pTargetinfo[0]->pMagNow[12],pTargetinfo[0]->pMagNow[13],pTargetinfo[0]->pMagNow[14],
				pTargetinfo[0]->pMagNow[15],pTargetinfo[0]->pMagNow[16],pTargetinfo[0]->pMagNow[17],
				talgo_get_fredomain_least(),Qmc5883lDiff.BackVal_Diff);
		
		Radio_Trf_Debug_Printf("b:%02d%02d%02d%02d%02d;%02d%02d%02d%02d%02d;%02d%02d%02d%02d%02d%02d.%d",
				pTargetinfo[0]->pMagBG[2],
				pTargetinfo[0]->pMagBG[3],pTargetinfo[0]->pMagBG[4],pTargetinfo[0]->pMagBG[5],
				pTargetinfo[0]->pMagBG[6],pTargetinfo[0]->pMagBG[7],pTargetinfo[0]->pMagBG[8],
				pTargetinfo[0]->pMagBG[9],pTargetinfo[0]->pMagBG[10],pTargetinfo[0]->pMagBG[11],
				pTargetinfo[0]->pMagBG[12],pTargetinfo[0]->pMagBG[13],pTargetinfo[0]->pMagBG[14],
				pTargetinfo[0]->pMagBG[15],pTargetinfo[0]->pMagBG[16],pTargetinfo[0]->pMagBG[17],
				talgo_get_timedomain_least());
		
		Radio_Trf_Debug_Printf("t1:%04d %04d %04d %04d;%04d %04d %04d %04d",
				tradar_time_mag_buff_test[0],tradar_time_mag_buff_test[1],tradar_time_mag_buff_test[2],
				tradar_time_mag_buff_test[3],tradar_time_mag_buff_test[4],tradar_time_mag_buff_test[5],
				tradar_time_mag_buff_test[6],tradar_time_mag_buff_test[7]);
		
		Radio_Trf_Debug_Printf("t2:%04d %04d %04d %04d;%04d %04d %04d",
				tradar_time_mag_buff_test[8],tradar_time_mag_buff_test[9],tradar_time_mag_buff_test[10],
				tradar_time_mag_buff_test[11],tradar_time_mag_buff_test[12],tradar_time_mag_buff_test[13],
				tradar_time_mag_buff_test[14]);
	}
#endif
	
	return 0;
}

/**********************************************************************************************************
 @Function			void Radar_GetSample(void)
 @Description			雷达采样
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_GetSample(void)
{
	if (n_array >= SAMPLE_NUM) {
		if (flag_main_go != 0) {
			return;
		}
		n_array = 0;
		flag_main_go = 1 + flag_in_array;
		flag_in_array = 1 - flag_in_array;
	}
	
	if (flag_in_array == 0) {
		
		sample_array0[n_array] = RADAR_ADC_ConvertedValue;
		
		/* 设置DAC通道值 */
		HAL_DAC_SetValue(&RADAR_DAC_Handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (SAMPLE_NUM - n_array) * RADER_RANGE + RADER_LOW);
		
		/* 启动DAC */
		HAL_DAC_Start(&RADAR_DAC_Handler, DAC_CHANNEL_1);
		
		n_array++;
	}
	else {
		
		sample_array1[n_array] = RADAR_ADC_ConvertedValue;
		
		/* 设置DAC通道值 */
		HAL_DAC_SetValue(&RADAR_DAC_Handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (SAMPLE_NUM - n_array) * RADER_RANGE + RADER_LOW);
		
		/* 启动DAC */
		HAL_DAC_Start(&RADAR_DAC_Handler, DAC_CHANNEL_1);
		
		n_array++;
	}
}

/**********************************************************************************************************
 @Function			void Radar_GetSample_Time(void)
 @Description			雷达采样节拍
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_GetSample_Time(void)
{
	static int xx = 0;
	
	if (xx < 1) {
		xx = 13;
		Radar_GetSample();
	}
	else {
		xx--;
	}
}

/**********************************************************************************************************
 @Function			u16 talgo_get_radartunebase_test(void)
 @Description			talgo_get_radartunebase_test
 @Input				void
 @Return				void
**********************************************************************************************************/
u16 talgo_get_radartunebase_test(void)
{
	u16 tune_base;
	
	val_temp = TEMPERATURE_ADC_Read(100);
	
	tune_base = (20 + val_temp) * 5 + 100;
	
	if (tune_base < 100) {
		tune_base = 100;
	}
	else if (tune_base > 500) {
		tune_base = 500;
	}
	
	return tune_base;
}

/**********************************************************************************************************
 @Function			u16 talgo_get_radartunebase_vptat(void)
 @Description			talgo_get_radartunebase_vptat
 @Input				void
 @Return				void
**********************************************************************************************************/
u16 talgo_get_radartunebase_vptat(void)
{
	u16 tune_base;
	
	val_vptat = VPTAT_ADC_Read(100);
	
	val_temp = TEMPERATURE_ADC_Read(100);
	
	if (val_vptat < 50) {
		tune_base = (20 + val_temp) * 5 + 100;
	}
	else {
		tune_base = (val_vptat - 105) + 100;
	}
	
	if (tune_base < 100) {
		tune_base = 100;
	}
	else if (tune_base > 500) {
		tune_base = 500;
	}
	
	return tune_base;
}

/**********************************************************************************************************
 @Function			void Radar_EnterCriticalSection(void)
 @Description			雷达启动初始化配置系统时钟为32MHz HSI
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_EnterCriticalSection(void)
{
#if RADAR_MODEL_TYPE == RADAR_MODEL_V1
	talgo_get_radartunebase_test();
#elif RADAR_MODEL_TYPE == RADAR_MODEL_V2
	talgo_get_radartunebase_vptat();
	__NOP();
#else
	#error RADAR_MODEL_TYPE Define Error
#endif
	
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == SYSTEMCLOCKMSI)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);
	Delay_Init(32000);
#endif
#endif
	
	Radar_DMA_Init();
	Radar_ADC_Init();
	Radar_DAC_Init();
}

/**********************************************************************************************************
 @Function			void Radar_EnterCriticalSection(void)
 @Description			雷达退出反初始化配置为原系统时钟MSI
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_ExitCriticalSection(void)
{
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == SYSTEMCLOCKMSI)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);
	Delay_Init(4194);
#endif
#endif
	
	Radar_ADC_DeInit();
	Radar_DMA_DeInit();
	Radar_DAC_DeInit();
}

extern short tradar_fre_magBG[TRADAR_BACKGROUND_NUM];
extern short tradar_fre_magBG_base[TRADAR_BACKGROUND_NUM];

/**********************************************************************************************************
 @Function			u16 Radar_get_increase_sum(short* array0, short* array1, u8 count)
 @Description			获取两个数组的正差值和
 @Input				
 @Return				
**********************************************************************************************************/
u16 Radar_get_increase_sum(short* array0, short* array1, u8 count)
{
	u16 total_diff = 0;
	u8 i;
	
	for (i = 0; i < count; i++) {
		if (array0[i] > (array1[i] + 1)) {
			total_diff += (array0[i] - array1[i]);
		}
	}
	
	return total_diff;
}

/**********************************************************************************************************
 @Function			u16 Radar_get_decrease_sum(short* array0, short* array1, u8 count)
 @Description			获取两个数组的负差值和
 @Input				
 @Return				
**********************************************************************************************************/
u16 Radar_get_decrease_sum(short* array0, short* array1, u8 count)
{
	u16 total_diff = 0;
	u8 i;
	
	for (i = 0; i < count; i++) {
		if ((array0[i] + 1) < array1[i]) {
			total_diff += (array1[i] - array0[i]);
		}
	}
	
	return total_diff;
}

/**********************************************************************************************************
 @Function			void Radar_check_background(RADAR_DataStruct* pRadarData)
 @Description			check whether to adjust the radar background
 @Input				
 @Return				
**********************************************************************************************************/
void Radar_check_background(RADAR_DataStruct* pRadarData)
{
	s16 inc_diff, dec_diff, inc_diff_base, dec_diff_base;
	
	if (radar_trigged_again == 0) {
		return;
	}
	
	radar_trigged_again = 0;
	
	inc_diff		= Radar_get_increase_sum(tradar_fre_mag_buff, tradar_fre_magBG, 10);
	inc_diff_base	= Radar_get_increase_sum(tradar_fre_mag_buff, tradar_fre_magBG_base, 10);
	dec_diff		= Radar_get_decrease_sum(tradar_fre_mag_buff, tradar_fre_magBG, 10);
	dec_diff_base	= Radar_get_decrease_sum(tradar_fre_mag_buff, tradar_fre_magBG_base, 10);
	
#ifdef RADAR_DEBUG_LOG_RF_PRINT
	Radio_Trf_Debug_Printf("temp:%d~%d", val_temp, val_temp_adjust);
#endif
	
	/* the changed temperature value is less than 8 */
	if ((val_temp_adjust - val_temp) < 8 && (val_temp_adjust - val_temp) > -8) {
		return;
	}
	
	if ((pRadarData->Diff > 32) || ((pRadarData->Diff - pRadarData->Diff_v2) > 32)) {
		return;
	}
	
	if ((inc_diff > 32) || (dec_diff > 32)) {
		return;
	}
	
	if ((inc_diff_base > 32) || (dec_diff_base > 32)) {
		return;
	}
	
	if ((pRadarData->Diff <= 16) && ((pRadarData->Diff - pRadarData->Diff_v2) <= 16) && (inc_diff <= 16) && (dec_diff <= 16)) {
		if ((pRadarData->Diff > 8) || ((pRadarData->Diff - pRadarData->Diff_v2) > 8)) {
#ifdef RADAR_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("diff:%d,%dbase:%d,%d", inc_diff, dec_diff, inc_diff_base, dec_diff_base);
#endif
			tradar_background_set((u16*)tradar_fre_mag_buff, (sizeof(tradar_fre_mag_buff)) / 2);
			val_vptat_adjust = val_vptat;
			val_temp_adjust  = val_temp;
			return;
		}
	}
	
	if ((pRadarData->Diff_base <= 16) && ((pRadarData->Diff_base - pRadarData->Diff_base_v2) <= 16) && (inc_diff_base <= 16) && (dec_diff_base <= 16)) {
		if ((pRadarData->Diff_base > 8) || ((pRadarData->Diff_base - pRadarData->Diff_base_v2) > 8)) {
#ifdef RADAR_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("diff:%d,%dbase:%d,%d", inc_diff, dec_diff, inc_diff_base, dec_diff_base);
#endif
			tradar_background_set((u16*)tradar_fre_mag_buff, (sizeof(tradar_fre_mag_buff)) / 2);
			val_vptat_adjust = val_vptat;
			val_temp_adjust  = val_temp;
			return;
		}
	}
}

/**********************************************************************************************************
 @Function			void Radar_check_timedomain_background(RADAR_DataStruct* pRadarData)
 @Description			check whether to adjust the radar timedomain background
 @Input				
 @Return				
**********************************************************************************************************/
void Radar_check_timedomain_background(RADAR_DataStruct* pRadarData)
{
	s16 inc_diff, dec_diff, inc_diff_base, dec_diff_base;
	
	if (radar_trigged_again == 0) {
		return;
	}
	
	dec_diff = dec_diff;
	dec_diff_base = dec_diff_base;
	
	radar_trigged_again = 0;
	
	inc_diff		= calculate_time_domain_total_square_diff();
	
#ifdef RADAR_DEBUG_LOG_RF_PRINT
	Radio_Trf_Debug_Printf("temp:%d~%d", val_temp, val_temp_adjust);
#endif
	
	/* the changed temperature value is less than 8 */
	if ((val_temp_adjust - val_temp) < 8 && (val_temp_adjust - val_temp) > -8) {
		return;
	}
	
	if (pRadarData->timedomain_square_dif > 32) {
		return;
	}
	
	if (inc_diff > 32) {
		return;
	}
	
	if (inc_diff_base > 32) {
		return;
	}
	
	if ((pRadarData->Diff <= 16) && (inc_diff <= 16)) {
		if (pRadarData->Diff > 8) {
#ifdef RADAR_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("diff:%d,%dbase:%d,%d", inc_diff, dec_diff, inc_diff_base, dec_diff_base);
#endif
			tradar_background_set((u16*)tradar_fre_mag_buff, (sizeof(tradar_fre_mag_buff)) / 2);
			val_vptat_adjust = val_vptat;
			val_temp_adjust  = val_temp;
			return;
		}
	}
	
	if ((pRadarData->Diff_base <= 16) && (inc_diff_base <= 16)) {
		if (pRadarData->Diff_base > 8) {
#ifdef RADAR_DEBUG_LOG_RF_PRINT
			Radio_Trf_Debug_Printf("diff:%d,%dbase:%d,%d", inc_diff, dec_diff, inc_diff_base, dec_diff_base);
#endif
			tradar_background_set((u16*)tradar_fre_mag_buff, (sizeof(tradar_fre_mag_buff)) / 2);
			val_vptat_adjust = val_vptat;
			val_temp_adjust  = val_temp;
			return;
		}
	}
}

/********************************************** END OF FLEE **********************************************/
