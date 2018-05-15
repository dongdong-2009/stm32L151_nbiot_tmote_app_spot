/**
  *********************************************************************************************************
  * @file    tmesh_radar_algorithm.h
  * @author  Movebroad -- Bell Zhong
  * @version V0.2
  * @date    2017-11-14
  * @brief   雷达算法库
  *********************************************************************************************************
  * @attention
  *
  *	171114 : 根据3.3V供电mcu进行调整,对应运放(R23,R25) = (10K,100R),旧款雷达模块,VPTAT没有接到VTUNE,对应DAC输出值变化为: 50->(50+304*6).
  *	180128 : 有车判断更加灵敏.
  *	180130 : 增加对自制雷达模块的支持,通过tradar_set_model选择雷达模块,并纠正了一个频率排序时的bug.
  *	180131 : 版本19,diff值改为有符号数,当这个值小于-10时就认为是覆盖了.
  *	180211 : 版本20,diff值小于-20时才认为是覆盖了.
  *		    版本21,微调了灵敏度.
  *		    版本22,提高了积水判断的灵敏度.
  *		    版本24,提高了积水判断的灵敏度.
  *		    版本27,增加了基准雷达背景,和动态雷达背景,基准背景是人为设置时候保存的,动态背景是在动态调整的.
  *		    版本28,总增加的diff值要超过20才认为有车.
  *		    版本29,V1版本雷达也可以输出频率特性.
  *		    版本30,RV2版本,覆盖的判断灵敏度在middle时,负值为-20,灵敏度为lowest时,负值为27.
  *		    版本31,RV2版本,距离超过64时,radardif不得小于30.
  *		    版本34,RV2版本,根据差值减小程度来判断覆盖的逻辑更加严格.
  *		    版本36,可以设定检测距离,0.5米到1.6米,默认为1.1米,为了抵消检测距离增大而带来的噪声,15~20频点的值的误差要大于4时才开始记录.
  *********************************************************************************************************
  */

#ifndef __TMESH_RADER_ALGORITHM_H
#define   __TMESH_RADER_ALGORITHM_H

#include "sys.h"

#define NOT_COVERED						0
#define BE_COVERED						1
#define MAY_COVERED						2

#define STATUS_NO_TARGET					0
#define STATUS_COVER_WATER				1
#define STATUS_COVER_SOLID				2
#define STATUS_HAS_TARGET				3

#define TRADAR_SAMPLE_NUM				(256+8)
#define TRADAR_BACKGROUND_NUM				20

enum TRADAR_DETECT_STATUS
{
	TRADAR_NO_TARGET					= 0x00,
	TRADAR_BE_COVERED					= 0x01,
	TRADAR_HAS_TARGET					= 0x02
};

enum TRADAR_DISTANCE_RANGE
{
	TRADAR_RANGE_0M5					= 0x09,
	TRADAR_RANGE_0M6					= 0x0A,
	TRADAR_RANGE_0M7					= 0x0B,
	TRADAR_RANGE_0M8					= 0x0C,
	TRADAR_RANGE_0M9					= 0x0D,
	TRADAR_RANGE_1M0					= 0x0E,
	TRADAR_RANGE_1M1					= 0x0F,
	TRADAR_RANGE_1M2					= 0x10,
	TRADAR_RANGE_1M3					= 0x11,
	TRADAR_RANGE_1M4					= 0x12,
	TRADAR_RANGE_1M5					= 0x13,
	TRADAR_RANGE_1M6					= 0x14
	
};

enum TRADAR_ERRCODE
{
	TRADAR_OK							= 0x00,
	TRADAR_ERROR						= 0x01,
	TRADAR_NO_BACKGROUND				= 0x02,
	TRADAR_UNKNOWN						= 0x03
};

enum TRADAR_VDD
{
	TRADAR_3V3						= 0x00,
	TRADAR_2V8						= 0x01
};

enum TRADAR_MODEL
{
	TRADAR_V1							= 0x01,
	TRADAR_V2							= 0x02
};

typedef struct
{
	unsigned int						signal_strength;
	unsigned char						distance_cm;
	unsigned char						status;
	short							strenth_total_diff;
	short							strenth_total_diff_v2;
	short							strenth_total_diff_base;
	short							strenth_total_diff_base_v2;
	unsigned short						debuginfo[10];
	unsigned short						fre2_mag[2];
	unsigned short						overnum[2];
	unsigned short						overnum_bg[2];
	unsigned char						cover_status;
	signed int						time_total_square_diff;
	signed short						time_total_diff;
	short							*pMagNow;
	short							*pMagBG;
}tradar_targetinfo_s;

enum TRADAR_SENSITIVITY
{
	RADAR_HIGHEST						= 0x01,
	RADAR_HIGH						= 0x02,
	RADAR_MIDDLE						= 0x03,
	RADAR_LOW							= 0x04,
	RADAR_LOWEST						= 0x05
};

extern short tradar_fre_mag_buff[TRADAR_BACKGROUND_NUM];

/**********************************************************************************************************
 @Function			tradar_background_transform
 @Purpose				get the backgroud frequency spectrum
 @Input				datain_t			: input, the amplitude in time domain
					in_num			: input, the number of datain_t,it must the same with TRADAR_SAMPLE_NUM
					fre_strenth_out	: output, the amplitude in frequency domain
					out_num			: output, the number of fre_strenth_out,it must be the same with TRADAR_BACKGROUND_NUM
 @Return				TRADAR_ERROR or TRADAR_OK
**********************************************************************************************************/
char tradar_background_transform(u32 datain_t[], u16 in_num, u16 fre_strenth_out[], s16 time_domain_out[], u8 len);

/**********************************************************************************************************
 @Function			tradar_timedomain_background_set
 @Purpose				set the background of time domain
 @Input				time_magBG		: input, the amplitude in time domain
					in_num			: input, the number of time_magBG,it must the same with TRADAR_SAMPLE_NUM
 @Return				TRADAR_ERROR or TRADAR_OK
**********************************************************************************************************/
char tradar_timedomain_background_set(s16* time_magBG, u8 len);

/**********************************************************************************************************
 @Function			tradar_background_set
 @Purpose				set the background frequency spectrum
 @Input				fre_magBG			: input, the amplitude in time domain
					in_num			: input, the number of fre_magBG,it must the same with TRADAR_SAMPLE_NUM
 @Return				TRADAR_ERROR or TRADAR_OK
**********************************************************************************************************/
char tradar_background_set(u16* fre_magBG, u8 in_num);

/**********************************************************************************************************
 @Function			tradar_base_background_set
 @Purpose				set the base background frequency spectrum of manual operation
 @Input				fre_magBG			: input, the amplitude in time domain
					in_num			: input, the number of fre_magBG,it must the same with TRADAR_SAMPLE_NUM
 @Return				TRADAR_ERROR or TRADAR_OK
**********************************************************************************************************/
char tradar_base_background_set(u16* fre_magBG, u8 len);

/**********************************************************************************************************
 @Function			tradar_target_detect
 @Purpose				detect the target info
 @Input				datain_t			: input, the amplitude in time domain
					in_num			: input, the number of datain_t,it must the same with TRADAR_SAMPLE_NUM
					targetinfo		: output, the target info
 @Return				TRADAR_ERROR or TRADAR_OK
**********************************************************************************************************/
char tradar_target_detect(u32 datain_t[], u16 in_mum, tradar_targetinfo_s* targetinfo);

/**********************************************************************************************************
 @Function			tradar_get_lib_version
 @Purpose				get the library version
 @Input				void
 @Return				the version description
**********************************************************************************************************/
char* tradar_get_lib_version(void);

/**********************************************************************************************************
 @Function			tradar_get_lib_num
 @Purpose				get the library num
 @Input				void
 @Return				the version numer
**********************************************************************************************************/
char tradar_get_lib_num(void);

/**********************************************************************************************************
 @Function			tradar_set_vdd
 @Purpose				set the vdd of mcu
 @Input				TRADAR_3V3,TRADAR_2V8
 @Return				void
**********************************************************************************************************/
void tradar_set_vdd(char vdd_type);

/**********************************************************************************************************
 @Function			tradar_set_sensetivity
 @Purpose				set sensetivity of radar
 @Input				sensetivity
 @Return				void
**********************************************************************************************************/
void tradar_set_sensetivity(unsigned char sense);

/**********************************************************************************************************
 @Function			tradar_get_sensetivity
 @Purpose				get sensetivity of radar
 @Input				void
 @Return				sensetivity
**********************************************************************************************************/
unsigned char tradar_get_sensetivity(void);

/**********************************************************************************************************
 @Function			tradar_set_model
 @Purpose				set version of radar model
 @Input				TRADAR_V1 = 1,	TRADAR_V2 = 2
 @Return				void
**********************************************************************************************************/
void tradar_set_model(char val);

/**********************************************************************************************************
 @Function			calculate_time_domain_total_square_diff
 @Purpose				获取时域变化值的平方和
 @Input				void
 @Return				s32
**********************************************************************************************************/
s32 calculate_time_domain_total_square_diff(void);

/**********************************************************************************************************
 @Function			tradar_set_distance_range
 @Purpose				set the distance range of radar detection
 @Input				val : TRADAR_RANGE_0M5 ~ TRADAR_RANGE_1M6
 @Return				void
**********************************************************************************************************/
void tradar_set_distance_range(unsigned char val);

/**********************************************************************************************************
 @Function			tradar_get_distance_range
 @Purpose				get the distance range of radar detection
 @Input				void
 @Return				TRADAR_RANGE_0M5 ~ TRADAR_RANGE_1M6
**********************************************************************************************************/
unsigned char tradar_get_distance_range(void);

#endif
