#ifndef __PLATFORM_MAP_H
#define   __PLATFORM_MAP_H

#include "sys.h"

#define TCFG_ENV_BOOTMODE_TOUPDATE			1
#define TCFG_ENV_BOOTMODE_NORMAL			2
#define TCFG_ENV_BOOTMODE_UPDATING			3

#define TCFG_FLAGTYPE_GENERAL				0
#define TCFG_FLAGTYPE_MAGAlGORITHM			1
#define TCFG_FLAGTYPE_SECU				2

#define TCFG_X_AXIS						0
#define TCFG_Y_AXIS						1
#define TCFG_Z_AXIS						2

#define EEPROM_BASE_ADDRESS				0x08080000
#define EEPROM_CONFIG_OFFSET				0x08080000
#define EEPROM_CONFIG_SIZE				0x0300

#define TCFG_FACTORY_VENDER_OFFSET			0x0C00
#define TCFG_FACTORY_VENDER_LENGTH			4

#define TCFG_FACTORY_MAC_SN_OFFSET			0x0C04
#define TCFG_FACTORY_MAC_SN_LENGTH			4

#define TCFG_FACTORY_BRAND_SN_OFFSET		0x0400											//0x08080400
#define TCFG_FACTORY_BRAND_SN_LENGTH		8												//SN Brand

/* The environment parameters are used both by bootLoader and application */
#define TCFG_ENVFLAG_OFFSET				TCFG_FACTORY_BRAND_SN_OFFSET + TCFG_FACTORY_BRAND_SN_LENGTH	//0x08080408
#define TCFG_ENVFLAG_LENGTH				4												//"TCLD"
#define TCFG_ENV_BOOTMODE_OFFSET			TCFG_ENVFLAG_OFFSET + TCFG_ENVFLAG_LENGTH				//0x0808040C
#define TCFG_ENV_BOOTMODE_LENGTH			1												//boot app from flash or load app from air
/********************************** end ***********************************/

#define TCFG_HEADFLAG_OFFSET				TCFG_ENV_BOOTMODE_OFFSET + TCFG_ENV_BOOTMODE_LENGTH		//0x0808040D
#define TCFG_HEADFLAG_LENGTH				4												//"TCLD"				状态码
#define TCFG_SN_OFFSET					TCFG_HEADFLAG_OFFSET + TCFG_HEADFLAG_LENGTH				//0x08080411
#define TCFG_SN_LENGTH					4												//SN					SN号
#define TCFG_SPOTID_OFFSET				TCFG_SN_OFFSET + TCFG_SN_LENGTH						//0x08080415
#define TCFG_SPOTID_LENGTH				16												//SPOTID				车位ID
#define TCFG_HEARTINTERVAL_OFFSET			TCFG_SPOTID_OFFSET + TCFG_SPOTID_LENGTH					//0x08080425
#define TCFG_HEARTINTERVAL_LENGTH			2												//Heart Interval		心跳间隔时间
#define TCFG_MAG_SENSITIVITY_OFFSET		TCFG_HEARTINTERVAL_OFFSET + TCFG_HEARTINTERVAL_LENGTH		//0x08080427
#define TCFG_MAG_SENSITIVITY_LENGTH		1												//Sensitivity			传感器灵敏度
#define TCFG_MAG_FREQ_OFFSET				TCFG_MAG_SENSITIVITY_OFFSET + TCFG_MAG_SENSITIVITY_LENGTH	//0x08080428
#define TCFG_MAG_FREQ_LENGTH				1												//Freq				地磁扫描频率

#define TCFG_MAGFLAG_OFFSET				TCFG_MAG_FREQ_OFFSET + TCFG_MAG_FREQ_LENGTH				//0x08080429
#define TCFG_MAGFLAG_LENGTH				1												//'T' = 0x54			状态码
#define TCFG_MAG_BACK_X_OFFSET			TCFG_MAGFLAG_OFFSET + TCFG_MAGFLAG_LENGTH				//0x0808042A
#define TCFG_MAG_BACK_X_LENGTH			2												//Mag X Back			地磁X背景值
#define TCFG_MAG_BACK_Y_OFFSET			TCFG_MAG_BACK_X_OFFSET + TCFG_MAG_BACK_X_LENGTH			//0x0808042C
#define TCFG_MAG_BACK_Y_LENGTH			2												//Mag Y Back			地磁Y背景值
#define TCFG_MAG_BACK_Z_OFFSET			TCFG_MAG_BACK_Y_OFFSET + TCFG_MAG_BACK_Y_LENGTH			//0x0808042E
#define TCFG_MAG_BACK_Z_LENGTH			2												//Mag Z Back			地磁Z背景值

#define TCFG_SECUFLAG_OFFSET				TCFG_MAG_BACK_Z_OFFSET + TCFG_MAG_BACK_Z_LENGTH			//0x08080430
#define TCFG_SECUFLAG_LENGTH				1												//'T' = 0x54			状态码
#define TCFG_SECU_BRAND_OFFSET			TCFG_SECUFLAG_OFFSET + TCFG_SECUFLAG_LENGTH				//0x08080431
#define TCFG_SECU_BRAND_LENGTH			6												//BrandCode			产生码
#define TCFG_SECU_BRANDKEY_OFFSET			TCFG_SECU_BRAND_OFFSET + TCFG_SECU_BRAND_LENGTH			//0x08080437
#define TCFG_SECU_BRANDKEY_LENGTH			32												//Reserved			未使用
#define TCFG_SECU_BRANCHKEY_OFFSET			TCFG_SECU_BRANDKEY_OFFSET + TCFG_SECU_BRANDKEY_LENGTH		//0x08080457
#define TCFG_SECU_BRANCHKEY_LENGTH			32												//Reserved			未使用

#define TCFG_RECORD_RUNTIME_OFFSET			TCFG_SECU_BRANCHKEY_OFFSET + TCFG_SECU_BRANCHKEY_LENGTH	//0x08080477
#define TCFG_RECORD_RUNTIME_LENGTH			4												//Reserved			未使用

#define TCFG_RECORD_BOOTCNT_OFFSET			TCFG_RECORD_RUNTIME_OFFSET + TCFG_RECORD_RUNTIME_LENGTH	//0x0808047B
#define TCFG_RECORD_BOOTCNT_LENGTH			1												//BootCnt				正常运行标志

#define TCFG_APP_ENV0_OFFSET				TCFG_RECORD_BOOTCNT_OFFSET + TCFG_RECORD_BOOTCNT_LENGTH	//0x0808047C	偏移地址
#define TCFG_APP_ENV0_ADDRESS				EEPROM_BASE_ADDRESS + TCFG_APP_ENV0_OFFSET				//0x0808047C	绝对地址
#define TCFG_APP_ENV0_SIZE				24												//参数区域0
#define TCFG_APP_ENV1_OFFSET				TCFG_APP_ENV0_OFFSET + TCFG_APP_ENV0_SIZE				//0x08080494	偏移地址
#define TCFG_APP_ENV1_ADDRESS				EEPROM_BASE_ADDRESS + TCFG_APP_ENV1_OFFSET				//0x08080494	绝对地址
#define TCFG_APP_ENV1_SIZE				24												//参数区域1

#define TCFG_RECORD_SERVER_OFFSET			TCFG_APP_ENV1_OFFSET + TCFG_APP_ENV1_SIZE				//0x080804AC
#define TCFG_RECORD_SERVER_LENGTH			6												//Server Coap			CDP服务器地址
#define TCFG_WORKMODE_OFFSET				TCFG_RECORD_SERVER_OFFSET + TCFG_RECORD_SERVER_LENGTH		//0x080804B2
#define TCFG_WORKMODE_LENGTH				1												//WorkMode			工作模式
#define TCFG_RADARCOUNT_OFFSET			TCFG_WORKMODE_OFFSET + TCFG_WORKMODE_LENGTH				//0x080804B3
#define TCFG_RADARCOUNT_LENGTH			4												//RadarCount			雷达次数
#define TCFG_RADARDBG_OFFSET				TCFG_RADARCOUNT_OFFSET + TCFG_RADARCOUNT_LENGTH			//0x080804B7
#define TCFG_RADARDBG_LENGTH				1												//RadarDbg			雷达调试
#define TCFG_STATUSCNT_OFFSET				TCFG_RADARDBG_OFFSET + TCFG_RADARDBG_LENGTH				//0x080804B8
#define TCFG_STATUSCNT_LENGTH				4												//StatusCnt			状态改变值
#define TCFG_RFCHANNEL_OFFSET				TCFG_STATUSCNT_OFFSET + TCFG_STATUSCNT_LENGTH			//0x080804BC
#define TCFG_RFCHANNEL_LENGTH				1												//RFChannel			无线通信通道
#define TCFG_ENABLE_NBIOTPSM_OFFSET		TCFG_RFCHANNEL_OFFSET + TCFG_RFCHANNEL_LENGTH			//0x080804BD
#define TCFG_ENABLE_NBIOTPSM_LENGTH		1												//NBIOTPSM			NBPSM模式
#define TCFG_ACTIVE_DEVICE_OFFSET			TCFG_ENABLE_NBIOTPSM_OFFSET + TCFG_ENABLE_NBIOTPSM_LENGTH	//0x080804BE
#define TCFG_ACTIVE_DEVICE_LENGTH			1												//ActiveDevice			设备工作模式
#define TCFG_NBIOT_BOOTCNT_OFFSET			TCFG_ACTIVE_DEVICE_OFFSET + TCFG_ACTIVE_DEVICE_LENGTH		//0x080804BF
#define TCFG_NBIOT_BOOTCNT_LENGTH			4												//NBIOTBootCnt			NB重启次数
#define TCFG_COAP_SENTCNT_OFFSET			TCFG_NBIOT_BOOTCNT_OFFSET + TCFG_NBIOT_BOOTCNT_LENGTH		//0x080804C3
#define TCFG_COAP_SENTCNT_LENGTH			4												//CoapSentCnt			COAP发送包数
#define TCFG_COAP_RECVCNT_OFFSET			TCFG_COAP_SENTCNT_OFFSET + TCFG_COAP_SENTCNT_LENGTH		//0x080804C7
#define TCFG_COAP_RECVCNT_LENGTH			4												//CoapRecvCnt			COAP接收包数
#define TCFG_MQTTSN_SENTCNT_OFFSET			TCFG_COAP_RECVCNT_OFFSET + TCFG_COAP_RECVCNT_LENGTH		//0x080804CB
#define TCFG_MQTTSN_SENTCNT_LENGTH			4												//MqttSentCnt			MQTTSN发送包数
#define TCFG_MQTTSN_RECVCNT_OFFSET			TCFG_MQTTSN_SENTCNT_OFFSET + TCFG_MQTTSN_SENTCNT_LENGTH	//0x080804CF
#define TCFG_MQTTSN_RECVCNT_LENGTH			4												//MqttRecvCnt			MQTTSN接收包数
#define TCFG_DEV_BOOTCNT_OFFSET			TCFG_MQTTSN_RECVCNT_OFFSET + TCFG_MQTTSN_RECVCNT_LENGTH	//0x080804D3
#define TCFG_DEV_BOOTCNT_LENGTH			2												//DevBootCnt			设备重启次数
#define TCFG_EVENT_TIME_OFFSET			TCFG_DEV_BOOTCNT_OFFSET + TCFG_DEV_BOOTCNT_LENGTH			//0x080804D5
#define TCFG_EVENT_TIME_LENGTH			4												//EventTime			设备运行事件时间
#define TCFG_TEMP_BACKGROUND_OFFSET		TCFG_EVENT_TIME_OFFSET + TCFG_EVENT_TIME_LENGTH			//0x080804D9
#define TCFG_TEMP_BACKGROUND_LENGTH		2												//BackgroundTemp		雷达背景
#define TCFG_MAG_MODE_OFFSET				TCFG_TEMP_BACKGROUND_OFFSET + TCFG_TEMP_BACKGROUND_LENGTH	//0x080804DB
#define TCFG_MAG_MODE_LENGTH				1												//MagMode				地磁模式
#define TCFG_NB_HEART_OFFSET				TCFG_MAG_MODE_OFFSET + TCFG_MAG_MODE_LENGTH				//0x080804DC
#define TCFG_NB_HEART_LENGTH				1												//NBHeart				NB心跳
#define TCFG_RADAR_RANGE_OFFSET			TCFG_NB_HEART_OFFSET + TCFG_NB_HEART_LENGTH				//0x080804DD
#define TCFG_RADAR_RANGE_LENGTH			1												//RadarRange			雷达范围

enum TCFG_SENSITIVITY																	//传感器灵敏度
{
	SENSE_HIGHEST 						= 0x01,
	SENSE_HIGH						= 0x02,
	SENSE_MIDDLE						= 0x03,
	SENSE_LOW							= 0x04,
	SENSE_LOWEST						= 0x05
};

typedef struct
{
	unsigned int						SubSn;											//设备号
	unsigned char						SubMacSN[9];										//设备标识号
	unsigned char						SubVender[4];										//设备厂商号
	unsigned char						BuildTime[16];										//编译日期
	unsigned short						Heartinterval;										//心跳间隔
	unsigned char						Sensitivity;										//灵敏度
	unsigned char						MagFreq;											//地磁扫描频率
	unsigned short						MagBackgroundX;									//地磁背景值X
	unsigned short						MagBackgroundY;									//地磁背景值Y
	unsigned short						MagBackgroundZ;									//地磁背景值Z
	unsigned char						WorkMode;											//工作模式
	unsigned char						WorkModeStr[10];									//工作模式名
	unsigned char						RFChannel;										//无线通道
	unsigned char						MagMode;											//地磁模式
	unsigned int						RadarCount;										//雷达次数
	unsigned char						RadarDbgMode;										//雷达调试模式
	unsigned char						RadarRange;										//雷达检测范围
	unsigned int						SpotStatusCount;									//车位检测车辆次数
	unsigned char						NBIotHeart;										//NBIot心跳间隔
	unsigned int						NBIotBootCount;									//NBIot重启次数
	unsigned char						NBIotPSMEnable;									//NBIotPSM模式
	unsigned int						CoapSentCount;										//Coap发送包数
	unsigned int						CoapRecvCount;										//Coap接收包数
	unsigned int						MqttSNSentCount;									//MqttSN发送包数
	unsigned int						MqttSNRecvCount;									//MqttSN接收包数
	unsigned short						DeviceBootCount;									//设备重启次数
}TCFG_SystemDataTypeDef;

extern TCFG_SystemDataTypeDef				TCFG_SystemData;


void			TCFG_EEPROM_SystemInfo_Init(void);												//系统信息初始化
void			TCFG_EEPROM_WriteConfigData(void);												//写入系统配置信息
void			TCFG_EEPROM_ReadConfigData(void);												//读取系统配置信息

void			TCFG_EEPROM_SetBootMode(char bootmode);											//保存Boot启动模式
char			TCFG_EEPROM_GetBootMode(void);												//读取Boot启动模式

void			TCFG_EEPROM_SetSpotID(char* spotID);											//保存SpotID
void			TCFG_EEPROM_GetSpotID(char* spotID);											//读取SpotID

void			TCFG_EEPROM_SetHeartinterval(unsigned short val);									//保存Heartinterval
unsigned short	TCFG_EEPROM_GetHeartinterval(void);											//读取Heartinterval

void			TCFG_EEPROM_SetSavedSensitivity(unsigned char val);								//保存Sensitivity
unsigned char	TCFG_EEPROM_GetSavedSensitivity(void);											//读取Sensitivity

void			TCFG_EEPROM_SetMagFreq(unsigned char val);										//保存Freq
unsigned char	TCFG_EEPROM_GetMagFreq(void);													//读取Freq

void			TCFG_EEPROM_SetMagBackgroud(int16_t x_axis, int16_t y_axis, int16_t z_axis);			//保存地磁背景值
unsigned short	TCFG_EEPROM_GetMagBackgroud(char axis);											//读取地磁背景值

void			TCFG_EEPROM_SetSubSN(unsigned int subsn);										//保存SubSN
unsigned int	TCFG_EEPROM_GetSubSN(void);													//读取SubSN

void			TCFG_EEPROM_SetBrandCode(unsigned int brand);									//保存BrandCode
unsigned int	TCFG_EEPROM_GetBrandCode(void);												//读取BrandCode

void			TCFG_EEPROM_SetBootCount(unsigned char count);									//保存BootCount
unsigned char	TCFG_EEPROM_GetBootCount(void);												//读取BootCount

void			TCFG_EEPROM_SetServerIP(unsigned int val);										//保存ServerIP
unsigned int	TCFG_EEPROM_GetServerIP(void);												//读取ServerIP

void			TCFG_EEPROM_SetServerPort(unsigned short val);									//保存ServerPort
unsigned short	TCFG_EEPROM_GetServerPort(void);												//读取ServerPort

void			TCFG_EEPROM_SetWorkMode(unsigned char val);										//保存WorkMode
unsigned char	TCFG_EEPROM_GetWorkMode(void);												//读取WorkMode
char*		TCFG_EEPROM_Get_WorkMode_String(void);											//读取WorkMode字符串

void			TCFG_EEPROM_SetRadarCount(unsigned int val);										//保存RadarCount
unsigned int	TCFG_EEPROM_GetRadarCount(void);												//读取RadarCount
void			TCFG_AddRadarCount(void);													//RadarCount++
unsigned int	TCFG_GetRadarCount(void);													//获取RadarCount

void			TCFG_EEPROM_SetRadarDbgMode(unsigned char val);									//保存RadarDbgMode
unsigned char	TCFG_EEPROM_GetRadarDbgMode(void);												//读取RadarDbgMode

void			TCFG_EEPROM_SetStatusCount(unsigned int val);									//保存StatusCount
unsigned int	TCFG_EEPROM_GetStatusCount(void);												//读取StatusCount

void			TCFG_EEPROM_SetRfChannel(unsigned char val);										//保存RfChannel
unsigned char	TCFG_EEPROM_GetRfChannel(void);												//读取RfChannel

void			TCFG_EEPROM_SetEnableNBiotPSM(unsigned char val);									//保存EnableNBiotPSM
unsigned char	TCFG_EEPROM_GetEnableNBiotPSM(void);											//读取EnableNBiotPSM

void			TCFG_EEPROM_SetActiveDevice(unsigned char val);									//保存ActiveDevice
unsigned char	TCFG_EEPROM_GetActiveDevice(void);												//读取ActiveDevice

void			TCFG_EEPROM_SetNbiotBootCnt(unsigned int val);									//保存NbiotBootCnt
unsigned int	TCFG_EEPROM_GetNbiotBootCnt(void);												//读取NbiotBootCnt

void			TCFG_EEPROM_SetCoapSentCnt(unsigned int val);									//保存CoapSentCnt
unsigned int	TCFG_EEPROM_GetCoapSentCnt(void);												//读取CoapSentCnt

void			TCFG_EEPROM_SetCoapRecvCnt(unsigned int val);									//保存CoapRecvCnt
unsigned int	TCFG_EEPROM_GetCoapRecvCnt(void);												//读取CoapRecvCnt

void			TCFG_EEPROM_SetMqttSNSentCnt(unsigned int val);									//保存MqttSNSentCnt
unsigned int	TCFG_EEPROM_GetMqttSNSentCnt(void);											//读取MqttSNSentCnt

void			TCFG_EEPROM_SetMqttSNRecvCnt(unsigned int val);									//保存MqttSNRecvCnt
unsigned int	TCFG_EEPROM_GetMqttSNRecvCnt(void);											//读取MqttSNRecvCnt

void			TCFG_EEPROM_SetDevBootCnt(unsigned short val);									//保存DevBootCnt
unsigned short	TCFG_EEPROM_GetDevBootCnt(void);												//读取DevBootCnt

void			TCFG_EEPROM_SetEventTime(unsigned int val);										//保存EventTime
unsigned int	TCFG_EEPROM_GetEventTime(void);												//读取EventTime

void			TCFG_EEPROM_SetBackgroundTemp(int16_t val);										//保存BackgroundTemp
int16_t		TCFG_EEPROM_GetBackgroundTemp(void);											//读取BackgroundTemp

void			TCFG_EEPROM_SaveConfigInformation(unsigned int subSN);								//保存配置信息和SN
bool			TCFG_EEPROM_CheckHeadFlag(char flagtype);										//检测保存信息标志位

bool			TCFG_EEPROM_CheckInfoBurned(void);												//检测生产商信息
bool			TCFG_EEPROM_CheckNewSNorBrand(void);											//检测新的设备号或厂牌信息
void			TCFG_EEPROM_SetSNfromBrandKey(unsigned int val);									//保存SNfromBrandKey
unsigned int	TCFG_EEPROM_GetSNfromBrandKey(void);											//读取SNfromBrandKey
void			TCFG_EEPROM_SetFactoryBrand(unsigned int val);									//保存FactoryBrand
unsigned int	TCFG_EEPROM_GetFactoryBrand(void);												//读取FactoryBrand

void			TCFG_EEPROM_Set_MAC_SN(unsigned int sn);										//保存MAC SN
unsigned int	TCFG_EEPROM_Get_MAC_SN(void);													//读取MAC SN
char*		TCFG_EEPROM_Get_MAC_SN_String(void);											//读取MAC SN字符串

void			TCFG_EEPROM_SetVender(char* vender);											//保存Vender
void			TCFG_EEPROM_GetVender(char* vender);											//获取Verder
char*		TCFG_EEPROM_Get_Vender_String(void);											//读取vender字符串

void			TCFG_EEPROM_SetMagMode(uint8_t val);											//保存MagMode
unsigned char	TCFG_EEPROM_GetMagMode(void);													//读取MagMode

void			TCFG_EEPROM_SetNbiotHeart(uint8_t val);											//保存NbiotHeart
unsigned char	TCFG_EEPROM_GetNbiotHeart(void);												//读取NbiotHeart

void			TCFG_EEPROM_SetRadarRange(uint8_t val);											//保存RadarRange
unsigned char	TCFG_EEPROM_GetRadarRange(void);												//读取RadarRange

void			TCFG_Utility_Add_Device_BootCount(void);										//Device重启次数累加
unsigned short TCFG_Utility_Get_Device_BootCount(void);										//Device重启次数获取
void			TCFG_Utility_Add_Nbiot_BootCount(void);											//NBIot重启次数累加
unsigned int	TCFG_Utility_Get_Nbiot_BootCount(void);											//NBIot重启次数获取
void			TCFG_Utility_Add_Coap_SentCount(void);											//NBIot Coap发送次数累加
unsigned int	TCFG_Utility_Get_Coap_SentCount(void);											//NBIot Coap发送次数获取
void			TCFG_Utility_Add_Coap_RecvCount(void);											//NBIot Coap接收次数累加
unsigned int	TCFG_Utility_Get_Coap_RecvCount(void);											//NBIot Coap接收次数获取
void			TCFG_Utility_Add_MqttSN_SentCount(void);										//NBIot MqttSN发送次数累加
unsigned int	TCFG_Utility_Get_MqttSN_SentCount(void);										//NBIot MqttSN发送次数获取
void			TCFG_Utility_Add_MqttSN_RecvCount(void);										//NBIot MqttSN接收次数累加
unsigned int	TCFG_Utility_Get_MqttSN_RecvCount(void);										//NBIot MqttSN接收次数获取

char*		TCFG_Utility_Get_Nbiot_Iccid_String(void);										//读取Nbiot Iccid字符串
char*		TCFG_Utility_Get_Nbiot_Imei_String(void);										//读取Nbiot Imei字符串
int			TCFG_Utility_Get_Nbiot_Rssi_IntVal(void);										//读取Nbiot Rssi值
unsigned char	TCFG_Utility_Get_Nbiot_Rssi_UnCharVal(void);										//读取Nbiot Rssi值
int			TCFG_Utility_Get_Nbiot_CellSnr(void);											//读取Nbiot CellSnr值
unsigned int	TCFG_Utility_Get_Nbiot_SentCount(void);											//读取Nbiot SentCount值
unsigned int	TCFG_Utility_Get_Nbiot_RecvCount(void);											//读取Nbiot RecvCount值
unsigned short	TCFG_Utility_Get_Device_Batt_ShortVal(void);										//读取Device Batt值
short		TCFG_Utility_Get_Device_Temperature(void);										//读取Device Temperature值
char*		TCFG_Utility_Get_Build_Time_String(void);										//读取Build Time字符串
unsigned int	TCFG_Utility_Get_Run_Time(void);												//读取系统运行时间
unsigned short TCFG_Utility_Get_Mvb_ModelType(void);											//读取设备类型
unsigned char	TCFG_Utility_Get_RadarLibNum(void);											//读取雷达库版本号
unsigned char	TCFG_Utility_Get_AlgoLibNum(void);												//读取检测算法库版本号
unsigned char	TCFG_Utility_Get_SoftResetFlag(void);											//读取设备重启标识符
unsigned short TCFG_Utility_Get_ReInitModuleCount(void);										//读取模块异常初始化次数
unsigned short TCFG_Utility_Get_DistanceRange(void);											//读取雷达检测范围
unsigned char	TCFG_Utility_Get_Major_Softnumber(void);										//读取Major_Softnumber
unsigned char	TCFG_Utility_Get_Sub_Softnumber(void);											//读取Sub_Softnumber
unsigned char	TCFG_Utility_Get_Major_Hardnumber(void);										//读取Major_Hardnumber

#endif
