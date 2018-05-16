/**
  *********************************************************************************************************
  * @file    tmesh_algorithm.h
  * @author  Movebroad -- Bell Zhong
  * @version V0.11
  * @date    2018-03-01
  * @brief   车位检测算法库
  *********************************************************************************************************
  * @attention
  *
  *		    版本2 : 解决无车是radardif输出为0的bug,去掉有车判断对幅值的要求.
  *		    版本3 : 优化覆盖状态下,磁场在subOCCUPY附近波动导致平方触发雷达,并平方切换有车无车状态的问题.
  *		    版本4 : 精简了下代码.
  *		    版本6 : radardiff 改为有符号数.
  *		    版本7 : 有车变化到无车的diff值必须要相差8以上,已解决雷达检测在临界值无车有车反复波动的情况,
  *				  连续600次中有590次磁场比较稳定,那么会进行地磁背景初始化,避免之前1分钟磁场稳定就初始化导致经常出现背景有误的情况.
  *		    版本8 : 对磁场初始化的时机做了调整,当前雷达的频域和时域差值都比较小时,才进行初始化.
  *		    版本9 : 磁场变化为主,当磁场测出来有车,但是雷达的频域和时域都无车时,那么认为无车,或者认为有车;当磁场测出来无车时,那就是无车.
  *		    版本10: 过30秒测一次雷达,再过60秒测一次雷达,接着就10分钟测一次雷达.
  *		    版本11: 经过诸多改动.
  *		    版本15: 当雷达没有波动时,两个雷达之间的检测间隔需要1分钟以上
  *		    版本16: 取6次雷达值,把dif值最大的那次去掉,然后进行取平均值
  *		    版本17: 雷达dif小于5的时候就强制判定为有车
  *
  *********************************************************************************************************
  */

#ifndef __TMESH_ALGORITHM_H
#define   __TMESH_ALGORITHM_H

#include "sys.h"
#include "tmesh_radar_algorithm.h"

#define TALGO_FALSE						0
#define TALGO_TRUE						1

enum TRADAR_MAGMOD
{
	TRADAR_MAG_GENERAL					= 0x00,
	TRADAR_MAG_SUBWAY					= 0x01
};

typedef enum
{
	SPOT_CAR_FREE						= 0x00,
	SPOT_CAR_OCCUPY					= 0x01,
	SPOT_CAR_SUBOCCUPY					= 0x02,
	SPOT_CAR_UNKNOWN					= 0x03,
	
	SPOT_QMC_ERROR						= 0x04,
	SPOT_RADAR_ERROR					= 0x05,
	SPOT_MAG_UNKNOWN					= 0x06,
	SPOT_RADAR_UNKNOWN					= 0x07,
	SPOT_RADAR_WEEKOCCUPY				= 0x08,
	SPOT_RADAR_STRONGOCCUPY				= 0x09
}SPOTCarType;

typedef __packed struct
{
	signed short int					X_Now;
	signed short int					Y_Now;
	signed short int					Z_Now;
	
	signed short int					X_Last[5];
	signed short int					Y_Last[5];
	signed short int					Z_Last[5];
	
	signed short int					X_Back;
	signed short int					Y_Back;
	signed short int					Z_Back;
}QMC5883L_TypeDef;

typedef __packed struct
{
	unsigned int						BackVal_Diff;
	unsigned int						GraduaVal_Diff;
	unsigned int						ArrayVal_Diff[9];
}QMC5883LDIFF_TypeDef;

typedef __packed struct
{
	unsigned char						NotargetNum;
	unsigned char						CoverNum;
	unsigned char						DismagNum;
	unsigned int						DisVal;
	unsigned int						MagVal;
	signed short int					Diff;
	signed short int					Diff_v2;
	signed short int					Diff_base;
	signed short int					Diff_base_v2;
	signed int						timedomain_square_dif;
	signed int						timedomain_dif;
}RADAR_DataStruct;

typedef __packed struct
{
	SPOTCarType						spot_status;
	unsigned int						spot_count;
	
	QMC5883L_TypeDef					qmc5883lData;
	QMC5883LDIFF_TypeDef				qmc5883lDiff;
	RADAR_DataStruct					radarData;
	
	unsigned char						timebuf[20];
	unsigned int						timeCounter;
	unsigned int						unixTime;
}SpotStatusTypedef;

typedef enum
{
	SPOT_LONGFREE						= 0x00,
	SPOT_FREE2OCCUPY					= 0x01,
	SPOT_LONGOCCUPY					= 0x02,
	SPOT_OCCUPY2FREE					= 0x03,
	SPOT_UNKNOWN						= 0x04
}TALGO_SPOTSTATUS;

typedef enum
{
	SPOT_MUST_FREE						= 0x00,
	SPOT_MAY_FREE						= 0x01,
	SPOT_MAY_OCCUY						= 0x02
}TALGO_SPOTFREE_CORE;

typedef enum
{
	RADAR_NO_CHANGE					= 0x00,
	RADAR_LITTLE_CHANGE					= 0x01,
	RADAR_LARGE_CHANGE					= 0x02
}TALGO_RADAR_VARIETY;

typedef enum
{
	MAG_NO_CHANGE						= 0x00,
	MAG_LITTLE_CHANGE					= 0x01,
	MAG_LARGE_CHANGE					= 0x02
}TALGO_MAG_VARIETY;

typedef enum
{
	TARGET_NO_CHANGE					= 0x00,
	TARGET_DO_CHANGE					= 0x01
}TALGO_TARGET_VARIETY;

typedef __packed struct
{
	unsigned char						current_day;
	unsigned short int					fredomain_dif[3];
	signed int						timedomain_square_dif[3];
}radar_least_s;

typedef __packed struct
{
	unsigned char						current_day;
	unsigned char						rssi_min[3];
	unsigned char						rssi_max[3];
}rssi_peak_s;

typedef __packed struct
{
	unsigned char						current_quarter;
	unsigned short int					fredomain_dif[3];
}radar_least_last_halfhour_s;

typedef __packed struct
{
	unsigned short int					fre_last2_max;
	unsigned short int					fre_last2_min;
	unsigned short int					fre_last2_latest;
	signed int						time_last2_max;
	signed int						time_last2_min;
	signed int						time_last2_latest;
	unsigned int						last2_minutes;
	
	unsigned short int					fre_last_max;
	unsigned short int					fre_last_min;
	unsigned short int					fre_last_latest;
	signed int						time_last_max;
	signed int						time_last_min;
	signed int						time_last_latest;
	unsigned int						last_minutes;
	
	unsigned short int					fre_now_max;
	unsigned short int					fre_now_min;
	unsigned short int					fre_now_latest;
	signed int						time_now_max;
	signed int						time_now_min;
	signed int						time_now_latest;
	unsigned int						now_minutes;
}radar_latest_s;

extern QMC5883L_TypeDef					Qmc5883lData;
extern QMC5883LDIFF_TypeDef				Qmc5883lDiff;
extern RADAR_DataStruct					sRadarData;
extern radar_least_s					s_radar_least;
extern u8  RADARGETPACK;
extern u32 Inspect_Detect_LastTime;
extern u16 qmc_backgroud_times;

/**********************************************************************************************************
 @Function			talgo_set_sensitivity
 @Purpose				set the magnetic sensitivity
 @Input				val				: 0~4 ; 0 is most sensitive
 @Return				void
**********************************************************************************************************/
void talgo_set_sensitivity(u8 val);

/**********************************************************************************************************
 @Function			talgro_vechile_judge
 @Purpose				judge the spot status
 @Input				pSpotStatusData	: output the information of the judgement
 @Return				SPOT_LONGFREE = 0,	SPOT_FREE2OCCUPY,	SPOT_LONGOCCUPY,	SPOT_OCCUPY2FREE,	SPOT_UNKNOWN,
**********************************************************************************************************/
char talgro_vechile_judge(SpotStatusTypedef* pSpotStatusData);

/**********************************************************************************************************
 @Function			talgo_init_background
 @Purpose				init the spot background
 @Input				void
 @Return				void
**********************************************************************************************************/
void talgo_init_background(void);

/**********************************************************************************************************
 @Function			QMC5883L_InitBackgroud
 @Purpose				init the magnetic background
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_InitBackgroud(void);

/**********************************************************************************************************
 @Function			Inspect_Get_SpotStatus
 @Purpose				get the current spot status
 @Input				void
 @Return				SpotStatus
**********************************************************************************************************/
char talgo_get_spotstatus(void);

/**********************************************************************************************************
 @Function			talgo_get_radartunebase
 @Purpose				get the base tunning voltage for radar
 @Input				void
 @Return				radartunebase
**********************************************************************************************************/
u16 talgo_get_radartunebase(void);

/**********************************************************************************************************
 @Function			tradar_register_temperature_callback
 @Purpose				register the temperature read function.this func is used to get the magnetic information
 @Input				void
 @Return				void
**********************************************************************************************************/
void talgo_register_mag_callback(void (* get_magdata_cb)(short *x_out, short *y_out, short *z_out));

/**********************************************************************************************************
 @Function			talgo_register_radar_callback
 @Purpose				register the radar data read function.this func is used to get the information detected by radar
 @Input				void
 @Return				void
**********************************************************************************************************/
void talgo_register_radar_callback(u8 (* get_radardata_cb)(tradar_targetinfo_s* targetinfo[], u8 count));

/**********************************************************************************************************
 @Function			talgo_register_dprintf_callback
 @Purpose				register the printf function.this func is used to printf the debug infomation
 @Input				void
 @Return				void
**********************************************************************************************************/
void talgo_register_dprintf_callback(void (*dprintf_cb)(const char *fmt, ...));

/**********************************************************************************************************
 @Function			talgo_register_runningtime_callback
 @Purpose				register the running time read function.this func is used to get the passed seconds since the device boot up
 @Input				void
 @Return				void
**********************************************************************************************************/
void talgo_register_runningtime_callback(u32 (*get_runningtime_cb)(void));

/**********************************************************************************************************
 @Function			talgo_register_unixtime_callback
 @Purpose				register the unix time get function.this func is used to get the passed seconds since 1970-1-1 00:00:00
 @Input				void
 @Return				void
**********************************************************************************************************/
void talgo_register_unixtime_callback(u32 (*get_unix_cb)(void));

/**********************************************************************************************************
 @Function			tradar_register_temperature_callback
 @Purpose				register the temperature read function.this func is used to get the temperature of radar module
 @Input				void
 @Return				void
**********************************************************************************************************/
void tradar_register_temperature_callback(short (*get_temperature_cb)(u32 timeout));

/**********************************************************************************************************
 @Function			talgo_register_rssi_callback
 @Purpose				register the rssi read function.this func is used to get the rssi of nbiot signal
 @Input				void
 @Return				void
**********************************************************************************************************/
void talgo_register_rssi_callback(u8 (*get_rssi_cb)(void));

/**********************************************************************************************************
 @Function			talgo_get_lib_num
 @Purpose				get the library version
 @Input				void
 @Return				void
**********************************************************************************************************/
char talgo_get_lib_num(void);

/**********************************************************************************************************
 @Function			talgo_check_spotfree_score
 @Purpose				get the spot free score
 @Input				void
 @Return				0=SPOT_MUST_FREE, 1=SPOT_MAY_FREE, 2=SPOT_MAY_OCCUY.
**********************************************************************************************************/
u8 talgo_check_spotfree_score(u16 fredomain_dif, s32 timedomain_square_dif, u8* score);

/**********************************************************************************************************
 @Function			talgo_check_radar_motion
 @Description			check the variety of radar data 
 @Input				none
 @Return				0=no change, 1=little change, 2=large change.
**********************************************************************************************************/
u8 talgo_check_radar_motion(void);

/**********************************************************************************************************
 @Function			talgo_check_target_motion
 @Description			check the variety of target above
 @Input				none
 @Return				0=no change, 1 = do change.
**********************************************************************************************************/
u8 talgo_check_target_motion(void);

/**********************************************************************************************************
 @Function			talgo_get_timedomain_least
 @Description			get the least dif value of time domain in three days
 @Input				none
 @Return				least dif value of time domain
**********************************************************************************************************/
u16 talgo_get_fredomain_least(void);

/**********************************************************************************************************
 @Function			talgo_get_fredomain_least_inhalfhour
 @Description			get the least dif value of frequency domain in half hour
 @Input				none
 @Return				least dif value of frequency domain
**********************************************************************************************************/
u16 talgo_get_fredomain_least_inhalfhour(void);

/**********************************************************************************************************
 @Function			talgo_get_timedomain_least
 @Description			get the least dif value of time domain in three days
 @Input				none
 @Return				least dif value of time domain
**********************************************************************************************************/
u32 talgo_get_timedomain_least(void);

/**********************************************************************************************************
 @Function			talgo_set_hours_per_unit
 @Description			set hours per unit for radar check,we will store the least fredomain and timedomain diff value in every unit.
 @Input				val				: hours
 @Return				void
**********************************************************************************************************/
void talgo_set_hours_per_unit(u8 val);

/**********************************************************************************************************
 @Function			talgo_set_model
 @Description			set version of radar model
 @Input				val				: TRADAR_V1, TRADAR_V2
 @Return				void
**********************************************************************************************************/
void talgo_set_model(char val);

/**********************************************************************************************************
 @Function			talgo_check_radar_free_score_inchange
 @Description			when the radar data is in change,check whether the fre diff is in the lowest zone
 @Input				fredomain_dif		: current frequency domain diff;
 @Return				0=SPOT_MUST_FREE, 1=SPOT_MAY_FREE, 2=SPOT_MAY_OCCUY.
**********************************************************************************************************/
u8 talgo_check_radar_free_score_inchange(u16 fredomain_dif, u8* score);

/**********************************************************************************************************
 @Function			talgo_check_radar_free_score_instatic
 @Description			when the radar data is in change,check whether the fre diff is changed a lot with background
 @Input				fredomain_dif		: current frequency domain diff
 @Return				0=SPOT_MUST_FREE, 1=SPOT_MAY_FREE, 2=SPOT_MAY_OCCUY.
**********************************************************************************************************/
u8 talgo_check_radar_free_score_instatic(u16 fredomain_dif);

/**********************************************************************************************************
 @Function			talgo_set_magmod
 @Description			set magnetic mode
 @Input				val				:0 general, 1 subway
 @Return				void
**********************************************************************************************************/
void talgo_set_magmod(char val);

/**********************************************************************************************************
 @Function			talgo_check_mag_motion_lately
 @Description			check the variety of magnetic data in last two minute
 @Input				val
 @Return				0=no change, 1=little change, 2=large change.
**********************************************************************************************************/
u8 talgo_check_mag_motion_lately(u8 val);

/**********************************************************************************************************
 @Function			QMC5883L_InitBackgroud_cmd
 @Description			init the mag background wit special value
 @Input				x y z
 @Return				none
**********************************************************************************************************/
void QMC5883L_InitBackgroud_cmd(u16 x, u16 y, u16 z);

/**********************************************************************************************************
 @Function			talgo_init
 @Description			init the parameters
 @Input				none
 @Return				none
**********************************************************************************************************/
void talgo_init(void);

#endif
